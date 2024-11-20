// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPBRLightingPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Materials/RZMaterial.h"

#include "Razix/Gfx/Passes/Data/BRDFData.h"
#include "Razix/Gfx/Passes/Data/FrameData.h"
#include "Razix/Gfx/Passes/Data/GBufferData.h"
#include "Razix/Gfx/Passes/Data/GlobalData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"

#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Gfx {

        void RZPBRLightingPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            auto pbrShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);

            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "PBR Lighting Pipeline";
            pipelineInfo.cullMode               = Gfx::CullMode::Front;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.shader                 = pbrShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Gfx::TextureFormat::RGBA16F};
            pipelineInfo.depthFormat            = Gfx::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock       = framegraph.getBlackboard().get<FrameData>();
            auto& sceneLightsDataBlock = framegraph.getBlackboard().get<SceneLightsData>();
            auto& shadowData           = framegraph.getBlackboard().get<SimpleShadowPassData>();
            auto& globalLightProbes    = framegraph.getBlackboard().get<GlobalLightProbeData>();
            auto& brdfData             = framegraph.getBlackboard().get<BRDFData>();

            framegraph.getBlackboard().add<SceneData>() = framegraph.addCallbackPass<SceneData>(
                "Pass.Builtin.Code.PBRForwardLighting",
                [&](SceneData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc textureDesc{};
                    textureDesc.name   = "SceneHDR";
                    textureDesc.width  = ResolutionToExtentsMap[Resolution::k1440p].x;
                    textureDesc.height = ResolutionToExtentsMap[Resolution::k1440p].y;
                    textureDesc.type   = TextureType::Texture_2D;
                    textureDesc.format = TextureFormat::RGBA16F;

                    data.sceneHDR = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    textureDesc.name       = "SceneDepth";
                    textureDesc.format     = TextureFormat::DEPTH32F;
                    textureDesc.filtering  = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    textureDesc.type       = TextureType::Texture_Depth;
                    textureDesc.enableMips = false;

                    data.sceneDepth = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    data.sceneHDR   = builder.write(data.sceneHDR);
                    data.sceneDepth = builder.write(data.sceneDepth);

                    builder.read(frameDataBlock.frameData);
                    builder.read(sceneLightsDataBlock.lightsDataBuffer);
                    builder.read(shadowData.shadowMap);
                    builder.read(shadowData.lightVP);
                    builder.read(globalLightProbes.environmentMap);
                    builder.read(globalLightProbes.diffuseIrradianceMap);
                    builder.read(globalLightProbes.specularPreFilteredMap);
                    builder.read(brdfData.lut);
                },
                [=](const SceneData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("PBR Lighting Pass");
                    RAZIX_MARK_BEGIN("Pass.Builtin.PBRLighting", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.sceneHDR).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(data.sceneDepth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
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
                    RAZIX_TIME_STAMP_END();
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