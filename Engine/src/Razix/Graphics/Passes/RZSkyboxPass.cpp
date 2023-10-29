// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSkyboxPass.h"

#include "Razix/Core/RZApplication.h"
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

        void RZSkyboxPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto skyboxShader           = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Skybox);
            auto proceduralSkyboxShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::ProceduralSkybox);

            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Skybox.Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::Front;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = skyboxShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = false;
            pipelineInfo.depthOp                = CompareOp::LessOrEqual;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            pipelineInfo.name    = "ProceduralSkybox.Pipeline";
            pipelineInfo.shader  = proceduralSkyboxShader;
            m_ProceduralPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock  = framegraph.getBlackboard().get<FrameData>();
            auto& lightProbesData = framegraph.getBlackboard().get<GlobalLightProbeData>();
            auto& volumetricData  = framegraph.getBlackboard().get<VolumetricCloudsData>();
            auto& sceneData       = framegraph.getBlackboard().get<SceneData>();

            //framegraph.getBlackboard().add<SceneData>() = framegraph.addCallbackPass<SceneData>(
             framegraph.addCallbackPass(
                "Pass.Builtin.Code.Skybox",
                [&](auto& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    builder.read(frameDataBlock.frameData);
                    builder.read(lightProbesData.environmentMap);
                    builder.read(lightProbesData.diffuseIrradianceMap);
                    builder.read(lightProbesData.specularPreFilteredMap);
                    builder.read(volumetricData.noiseTexture);

                    builder.read(sceneData.outputHDR);
                    builder.read(sceneData.depth);

                    sceneData.outputHDR = builder.write(sceneData.outputHDR);
                    sceneData.depth     = builder.write(sceneData.depth);

#if ENABLE_DATA_DRIVEN_FG_PASSES
                    //builder.read(framegraph.getBlackboard().getID("SceneHDR"));
                    //builder.read(framegraph.getBlackboard().getID("SceneDepth"));

                    //data.outputHDR = builder.write(framegraph.getBlackboard().getID("SceneHDR"));
                    //data.depth     = builder.write(framegraph.getBlackboard().getID("SceneDepth"));
#endif
                },
                [=](const auto& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Skybox pass", glm::vec4(0.33f, 0.45f, 1.0f, 1.0f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    //info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.outputHDR).getHandle(), {false, ClearColorPresets::TransparentBlack}}};
                    //info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle(), {false, ClearColorPresets::DepthOneToZero}};
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.outputHDR).getHandle(), {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.depth).getHandle(), {false, ClearColorPresets::DepthOneToZero}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Set the Descriptor Set once rendering starts
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        if (!m_UseProceduralSkybox) {
                            auto& shaderBindVars = Graphics::RZResourceManager::Get().getShaderResource(skyboxShader)->getBindVars();
                            auto  descriptor     = shaderBindVars["environmentMap"];
                            if (descriptor)
                                descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(lightProbesData.environmentMap).getHandle();

                            Graphics::RZResourceManager::Get().getShaderResource(skyboxShader)->updateBindVarsHeaps();
                        } else {
                            auto& shaderBindVars = Graphics::RZResourceManager::Get().getShaderResource(proceduralSkyboxShader)->getBindVars();
                            auto  descriptor     = shaderBindVars["NoiseTexture"];
                            if (descriptor)
                                descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(volumetricData.noiseTexture).getHandle();

                            Graphics::RZResourceManager::Get().getShaderResource(proceduralSkyboxShader)->updateBindVarsHeaps();
                        }
                    }

                    if (!m_UseProceduralSkybox) {
                        //u32            envMapIdx = resources.get<FrameGraph::RZFrameGraphTexture>(lightProbesData.environmentMap).getHandle().getIndex();
                        //RZPushConstant pc;
                        //pc.data        = &envMapIdx;
                        //pc.size        = sizeof(u32);
                        //pc.shaderStage = ShaderStage::PIXEL;
                        //
                        //RHI::BindPushConstant(m_Pipeline, cmdBuffer, pc);
                        Graphics::RHI::BindPipeline(m_Pipeline, cmdBuffer);
                        scene->drawScene(m_Pipeline, SceneDrawGeometryMode::Cubemap);
                    } else {
                        // Since no skybox, we update the directional light direction
                        auto lights = scene->GetComponentsOfType<LightComponent>();
                        // We use the first found directional light
                        // TODO: Cache this
                        RZLight dirLight;
                        for (auto& lc: lights) {
                            if (lc.light.getType() == LightType::DIRECTIONAL)
                                dirLight = lc.light;
                            break;
                        }
                        struct PCData
                        {
                            glm::vec3 worldSpaceLightPos;
                            //u32       noiseTextureIdx;
                        } data{};
                        // FIXME: Use direction
                        data.worldSpaceLightPos = dirLight.getPosition();
                        //data.noiseTextureIdx    = resources.get<FrameGraph::RZFrameGraphTexture>(volumetricData.noiseTexture).getHandle().getIndex();
                        RZPushConstant pc;
                        pc.data        = &data;
                        pc.size        = sizeof(PCData);
                        pc.shaderStage = ShaderStage::Pixel;

                        Graphics::RHI::BindPipeline(m_ProceduralPipeline, cmdBuffer);
                        RHI::BindPushConstant(m_ProceduralPipeline, cmdBuffer, pc);
                        scene->drawScene(m_ProceduralPipeline, SceneDrawGeometryMode::Cubemap);
                    }

                    RHI::EndRendering(cmdBuffer);
                    RAZIX_MARK_END();
                });
        }

        void RZSkyboxPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            RZResourceManager::Get().destroyPipeline(m_ProceduralPipeline);
        }
    }    // namespace Graphics
}    // namespace Razix