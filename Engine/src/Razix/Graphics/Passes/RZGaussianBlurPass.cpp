// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGaussianBlurPass.h"

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

        struct GaussianBlurOutput
        {
            FrameGraph::RZFrameGraphResource blur;
        };

        struct GaussianBlurPCData
        {
            u32       tapFilter;
            f32       blurRadius;
            glm::vec2 direction;
        };

        void RZGaussianBlurPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto gaussiabBlurShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GaussianBlur);

            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Gaussian Blur FX Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = gaussiabBlurShader;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            if (m_IsTwoPass) {
                /**
                 * In a two pass Gaussian filter we do a Horizontal and then a Vertical blur on the input texture
                 */
                auto blurH      = addBlurPass(framegraph, scene, settings, m_InputTexture, m_BlurRadius, m_FilterTap, GaussianDirection::Horizontal);
                m_OutputTexture = addBlurPass(framegraph, scene, settings, blurH, m_BlurRadius, m_FilterTap, GaussianDirection::Vertical);
            } else
                m_OutputTexture = addBlurPass(framegraph, scene, settings, m_InputTexture, m_BlurRadius, m_FilterTap, m_Direction);
        }

        void RZGaussianBlurPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }

        FrameGraph::RZFrameGraphResource RZGaussianBlurPass::addBlurPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings, FrameGraph::RZFrameGraphResource inputTexture, f32 blurRadius, GaussianTap filterTap, GaussianDirection direction)
        {
            auto gaussiabBlurShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GaussianBlur);

            auto& pass = framegraph.addCallbackPass<GaussianBlurOutput>(
                "Pass.Builtin.Code.FX.GaussianBlur",
                [&](GaussianBlurOutput& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc textureDesc{
                        .name       = "GaussianBlurredTex",
                        .width      = ResolutionToExtentsMap[Resolution::k1440p].x,
                        .height     = ResolutionToExtentsMap[Resolution::k1440p].y,
                        .type       = TextureType::Texture_2D,
                        .format     = TextureFormat::RGBA32F,
                        .enableMips = false};

                    data.blur = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    builder.read(inputTexture);
                    data.blur = builder.write(data.blur);
                },
                [=](const GaussianBlurOutput& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("GaussianBlur");
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.FX.GaussianBlur", glm::vec4(178.0f, 190.0f, 181.0f, 255.0f) / 255.0f);

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.blur).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    // Set the Descriptor Set once rendering starts
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(gaussiabBlurShader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        descriptor = shaderBindVars["inputTexture"];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(inputTexture).getHandle();

                        RZResourceManager::Get().getShaderResource(gaussiabBlurShader)->updateBindVarsHeaps();
                    }

                    RHI::BindPipeline(m_Pipeline, RHI::GetCurrentCommandBuffer());

                    GaussianBlurPCData pcData{};
                    pcData.blurRadius = blurRadius;
                    pcData.direction  = direction == GaussianDirection::Horizontal ? glm::vec2(1, 0) : glm::vec2(0, 1);
                    pcData.tapFilter  = (u32) filterTap;

                    RZPushConstant pc;
                    pc.size        = sizeof(GaussianBlurPCData);
                    pc.data        = &pcData;
                    pc.shaderStage = ShaderStage::Pixel;
                    RHI::BindPushConstant(m_Pipeline, RHI::GetCurrentCommandBuffer(), pc);

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });

            return pass.blur;
        }
    }    // namespace Graphics
}    // namespace Razix