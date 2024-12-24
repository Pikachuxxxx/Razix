// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPBRDeferredLightingPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

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

        void RZPBRDeferredLightingPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            auto pbrShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRDeferredLighting);

            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "PBR Deferred Lighting Pipeline";
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.shader                 = pbrShader;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.colorAttachmentFormats = {Gfx::TextureFormat::RGBA16F};
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.depthWriteEnabled      = false;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock       = framegraph.getBlackboard().get<FrameData>();
            auto& sceneLightsDataBlock = framegraph.getBlackboard().get<SceneLightsData>();
            auto& shadowData           = framegraph.getBlackboard().get<SimpleShadowPassData>();
            auto& csmData              = framegraph.getBlackboard().get<CSMData>();
            auto& globalLightProbes    = framegraph.getBlackboard().get<GlobalLightProbeData>();
            auto& brdfData             = framegraph.getBlackboard().get<BRDFData>();
            auto& gbufferData          = framegraph.getBlackboard().get<GBufferData>();
            //auto& ssaoData             = framegraph.getBlackboard().get<FX::SSAOData>();

            framegraph.getBlackboard().add<SceneData>() = framegraph.addCallbackPass<SceneData>(
                "Pass.Builtin.Code.PBRDeferredLighting",
                [&](SceneData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc textureDesc{};
                    textureDesc.name      = "SceneHDR";
                    textureDesc.width     = ResolutionToExtentsMap[Resolution::k1440p].x;
                    textureDesc.height    = ResolutionToExtentsMap[Resolution::k1440p].y;
                    textureDesc.type      = TextureType::k2D;
                    textureDesc.format    = TextureFormat::RGBA16F;

                    data.sceneHDR = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    data.sceneHDR   = builder.write(data.sceneHDR);
                    data.sceneDepth = builder.write(gbufferData.GBufferDepth);

                    builder.read(frameDataBlock.frameData);
                    builder.read(sceneLightsDataBlock.lightsDataBuffer);
                    builder.read(shadowData.shadowMap);
                    builder.read(shadowData.lightVP);
                    builder.read(csmData.cascadedShadowMaps);
                    builder.read(csmData.viewProjMatrices);
                    builder.read(globalLightProbes.environmentMap);
                    builder.read(globalLightProbes.diffuseIrradianceMap);
                    builder.read(globalLightProbes.specularPreFilteredMap);
                    builder.read(brdfData.lut);
                    builder.read(gbufferData.GBuffer0);
                    builder.read(gbufferData.GBuffer1);
                    builder.read(gbufferData.GBuffer2);
                    //builder.read(ssaoData.SSAOSceneTexture);
                },
                [=](const SceneData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("Deferred Lighting (PBR)");
                    RAZIX_MARK_BEGIN("Pass.Builtin.PBRDeferredLighting", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.sceneHDR).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
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

                        // Scene SSAO Texture
                        //descriptor = shaderBindVars["SSAOSceneTexture"];
                        //if (descriptor)
                        //    descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(ssaoData.SSAOSceneTexture).getHandle();

                        // CSM Array Texture
                        descriptor = shaderBindVars["CSMArray"];
                        if (descriptor)
                            descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(csmData.cascadedShadowMaps).getHandle();

                        // CSM Matrices
                        descriptor = shaderBindVars["CSMMatrices"];
                        if (descriptor)
                            descriptor->uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(csmData.viewProjMatrices).getHandle();

                        RZResourceManager::Get().getShaderResource(pbrShader)->updateBindVarsHeaps();
                    }

                    RHI::BindPipeline(m_Pipeline, RHI::GetCurrentCommandBuffer());

                    struct PCData
                    {
                        glm::vec3 CameraViewPos;
                        bool      visCascades = true;
                        bool      _padding[3] = {0, 0, 0};
                        glm::mat4 camView;
                        f32       dt;
                        f32       biasScale = 0.005f;
                        f32       maxBias   = 0.0005f;
                    } pcData{};
                    pcData.CameraViewPos = scene->getSceneCamera().getPosition();
                    if (IS_BIT_SET(settings->debugFlags, RendererDebugFlag_VisCSMCascades))
                        pcData.visCascades = true;
                    else
                        pcData.visCascades = false;
                    pcData.camView   = scene->getSceneCamera().getViewMatrix();
                    pcData.dt        = RZEngine::Get().GetStatistics().DeltaTime;
                    pcData.biasScale = biasScale;
                    pcData.maxBias   = maxBias;
                    RZPushConstant pc;
                    pc.size        = sizeof(PCData);
                    pc.data        = &pcData;
                    pc.shaderStage = ShaderStage::kPixel;
                    RHI::BindPushConstant(m_Pipeline, RHI::GetCurrentCommandBuffer(), pc);

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZPBRDeferredLightingPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            //m_PBRBindingSet->Destroy();
            //m_PBRPassBindingUBO->Destroy();
        }
    }    // namespace Gfx
}    // namespace Razix