// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSSAOPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        static float lerp(float a, float b, float f)
        {
            return a + f * (b - a);
        }

        void RZSSAOPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto ssaoShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::SSAO);

            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "SSAO FX Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = ssaoShader;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock = framegraph.getBlackboard().get<FrameData>();
            auto& gbufferData    = framegraph.getBlackboard().get<GBufferData>();

            auto& ssaoData = framegraph.getBlackboard().add<FX::SSAOData>();

            // Generate some data to pass to SSAO shader
            std::default_random_engine            rndEngine((unsigned) time(nullptr));
            std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

            // Sample kernel
            std::vector<glm::vec4> ssaoKernel(SSAO_KERNEL_SIZE);
            for (uint32_t i = 0; i < SSAO_KERNEL_SIZE; ++i) {
                glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
                sample = glm::normalize(sample);
                sample *= rndDist(rndEngine);
                float scale   = float(i) / float(SSAO_KERNEL_SIZE);
                scale         = lerp(0.1f, 1.0f, scale * scale);
                ssaoKernel[i] = glm::vec4(sample * scale, 0.0f);
            }

            // Random noise
            std::vector<glm::vec4> ssaoNoise(SSAO_NOISE_DIM * SSAO_NOISE_DIM);
            for (uint32_t i = 0; i < static_cast<uint32_t>(ssaoNoise.size()); i++) {
                ssaoNoise[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
            }
            // SSAO kernel samples buffer
            RZBufferDesc samplesBufferDesc{
                .name  = "Kernel",
                .size  = static_cast<u32>(ssaoKernel.size()) * sizeof(glm::vec4),
                .data  = ssaoKernel.data(),
                .usage = BufferUsage::Static};
            auto ssaoKernelBuffer      = Graphics::RZResourceManager::Get().createUniformBuffer(samplesBufferDesc);
            ssaoData.SSAOKernelSamples = framegraph.import <FrameGraph::RZFrameGraphBuffer>(samplesBufferDesc.name, CAST_TO_FG_BUF_DESC samplesBufferDesc, {ssaoKernelBuffer});

            // SSAO Noise texture
            RZTextureDesc noiseTextureDesc{
                .name       = "SSAONoiseTex",
                .width      = 4,
                .height     = 4,
                .data       = ssaoNoise.data(),
                .size       = static_cast<u32>(ssaoNoise.size()) * sizeof(glm::vec4),
                .type       = TextureType::Texture_2D,
                .format     = TextureFormat::RGBA32F,
                .wrapping   = Wrapping::REPEAT,
                .enableMips = false,
                .dataSize   = sizeof(float)};
            auto ssaoNoiseTexture     = Graphics::RZResourceManager::Get().createTexture(noiseTextureDesc);
            ssaoData.SSAONoiseTexture = framegraph.import <FrameGraph::RZFrameGraphTexture>(noiseTextureDesc.name, CAST_TO_FG_TEX_DESC noiseTextureDesc, {ssaoNoiseTexture});

            framegraph.getBlackboard()
                .add<FX::SSAOPreBlurData>() = framegraph.addCallbackPass<FX::SSAOPreBlurData>(
                "Pass.Builtin.Code.FX.SSAO",
                [&](FX::SSAOPreBlurData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    // We use a single channel f32 texture
                    RZTextureDesc textureDesc{
                        .name   = "SSAOScene",
                        .width  = ResolutionToExtentsMap[Resolution::k1440p].x,
                        .height = ResolutionToExtentsMap[Resolution::k1440p].y,
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA32F};
                    data.SSAOPreBlurTexture = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    // For temporary debugging set this as the final output
                    framegraph.getBlackboard().setFinalOutputName(textureDesc.name);

                    data.SSAOPreBlurTexture = builder.write(data.SSAOPreBlurTexture);

                    builder.read(ssaoData.SSAONoiseTexture);
                    builder.read(ssaoData.SSAOKernelSamples);
                    builder.read(frameDataBlock.frameData);
                    builder.read(gbufferData.GBuffer0);
                    builder.read(gbufferData.GBuffer1);
                    builder.read(gbufferData.GBuffer2);
                },
                [=](const FX::SSAOPreBlurData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("SSAO");
                    RAZIX_MARK_BEGIN("Pass.Builtin.FX.SSAO", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.SSAOPreBlurTexture).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    // Set the Descriptor Set once rendering starts
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(ssaoShader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        // Bind the GBuffer textures
                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphTexture>(gbufferData.GBuffer0)];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(gbufferData.GBuffer0).getHandle();

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphTexture>(gbufferData.GBuffer1)];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(gbufferData.GBuffer1).getHandle();

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphTexture>(gbufferData.GBuffer2)];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(gbufferData.GBuffer2).getHandle();

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphTexture>(ssaoData.SSAONoiseTexture)];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(ssaoData.SSAONoiseTexture).getHandle();

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphBuffer>(ssaoData.SSAOKernelSamples)];
                        if (descriptor)
                            descriptor->uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(ssaoData.SSAOKernelSamples).getHandle();

                        RZResourceManager::Get().getShaderResource(ssaoShader)->updateBindVarsHeaps();
                    }

                    RHI::BindPipeline(m_Pipeline, RHI::GetCurrentCommandBuffer());

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZSSAOPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix