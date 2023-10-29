// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPBRLightingPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GBufferData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"

#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZPBRLightingPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto pbrShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);

            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "PBR Lighting Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::Front;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = pbrShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock       = framegraph.getBlackboard().get<FrameData>();
            auto& sceneLightsDataBlock = framegraph.getBlackboard().get<SceneLightsData>();
            auto& shadowData           = framegraph.getBlackboard().get<SimpleShadowPassData>();
            auto& globalLightProbes    = framegraph.getBlackboard().get<GlobalLightProbeData>();
            auto& brdfData             = framegraph.getBlackboard().get<BRDFData>();
            //auto& gbufferData          = framegraph.getBlackboard().get<GBufferData>();

            framegraph.getBlackboard().add<SceneData>() = framegraph.addCallbackPass<SceneData>(
                "Pass.Builtin.Code.PBRLighting",
                [&](SceneData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc textureDesc{
                        .name   = "SceneHDR",
                        .width  = ResolutionToExtentsMap[Resolution::k1440p].x,
                        .height = ResolutionToExtentsMap[Resolution::k1440p].y,
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA32F};

                    data.outputHDR = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    textureDesc.name       = "SceneDepth";
                    textureDesc.format     = TextureFormat::DEPTH32F;
                    textureDesc.filtering  = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    textureDesc.type       = TextureType::Texture_Depth;
                    textureDesc.enableMips = false;

                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    data.outputHDR = builder.write(data.outputHDR);
                    data.depth     = builder.write(data.depth);

                    builder.read(frameDataBlock.frameData);
                    builder.read(sceneLightsDataBlock.lightsDataBuffer);
                    builder.read(shadowData.shadowMap);
                    builder.read(shadowData.lightVP);
                    builder.read(globalLightProbes.environmentMap);
                    builder.read(globalLightProbes.diffuseIrradianceMap);
                    builder.read(globalLightProbes.specularPreFilteredMap);
                    builder.read(brdfData.lut);
                    //builder.read(gbufferData.Albedo_PosY);
                    //builder.read(gbufferData.Emissive_PosZ);
                    //builder.read(gbufferData.Normal_PosX);
                    //builder.read(gbufferData.MetRougAOAlpha);
                    //builder.read(gbufferData.Depth);
                },
                [=](const SceneData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Pass.Builtin.PBRLighting", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.outputHDR).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    // Set the Descriptor Set once rendering starts
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(pbrShader)->getBindVars();

                        RZDescriptor* descriptor = nullptr;

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphTexture>(shadowData.shadowMap)];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(shadowData.shadowMap).getHandle();

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphBuffer>(shadowData.lightVP)];
                        if (descriptor)
                            descriptor->uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(shadowData.lightVP).getHandle();

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphTexture>(globalLightProbes.diffuseIrradianceMap)];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbes.diffuseIrradianceMap).getHandle();

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphTexture>(globalLightProbes.specularPreFilteredMap)];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbes.specularPreFilteredMap).getHandle();

                        descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphTexture>(brdfData.lut)];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(brdfData.lut).getHandle();

                        RZResourceManager::Get().getShaderResource(pbrShader)->updateBindVarsHeaps();
                    }

                    RHI::BindPipeline(m_Pipeline, RHI::GetCurrentCommandBuffer());

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::SceneGeometry);

                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                    RAZIX_MARK_END();
                });
        }

        void RZPBRLightingPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            //m_PBRBindingSet->Destroy();
            //m_PBRPassBindingUBO->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix