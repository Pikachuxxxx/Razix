// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShadowPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
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

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Lighting/RZLight.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZShadowPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Load the shader
            auto shader   = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::DepthPreTest);
            auto setInfos = RZResourceManager::Get().getShaderResource(shader)->getDescriptorsPerHeapMap();

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                = "Shadow Pass Pipeline";
            pipelineInfo.cullMode            = Graphics::CullMode::Back;
            pipelineInfo.drawType            = Graphics::DrawType::Triangle;
            pipelineInfo.shader              = shader;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthFormat         = {Graphics::TextureFormat::DEPTH32F};
            m_Pipeline                       = RZResourceManager::Get().createPipeline(pipelineInfo);

            framegraph.getBlackboard().add<SimpleShadowPassData>() = framegraph.addCallbackPass<SimpleShadowPassData>(
                "Pass.Builtin.Code.RenderShadows",
                [&](SimpleShadowPassData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    data.shadowMap = builder.create<FrameGraph::RZFrameGraphTexture>("ShadowMap", {.name = "ShadowMap", .width = kShadowMapSize, .height = kShadowMapSize, .type = TextureType::Texture_Depth, .format = TextureFormat::DEPTH32F, .wrapping = Wrapping::CLAMP_TO_BORDER, .enableMips = false});

                    data.lightVP = builder.create<FrameGraph::RZFrameGraphBuffer>("LightSpaceMatrix", {"LightSpaceMatrix", sizeof(LightVPUBOData)});

                    data.shadowMap = builder.write(data.shadowMap);
                    data.lightVP   = builder.write(data.lightVP);
                },
                [=](const SimpleShadowPassData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.RenderShadows", glm::vec4(0.65, 0.73, 0.22f, 1.0f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    LightVPUBOData light_data{};
                    // Get the Light direction
                    auto lights = scene->GetComponentsOfType<LightComponent>();
                    // Use the first directional light and currently only one Dir Light casts shadows, multiple just won't do anything in the scene not even light contribution
                    RZLight dir_light;
                    for (auto& light: lights) {
                        if (light.light.getType() == LightType::DIRECTIONAL) {
                            dir_light = light.light;
                            break;
                        }
                    }

                    glm::mat4 lightView  = glm::lookAt(dir_light.getPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    float     near_plane = -50.0f, far_plane = 50.0f;
                    glm::mat4 lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
                    lightProjection[1][1] *= -1;
                    light_data.lightViewProj = lightProjection * lightView;

                    auto lightVPHandle = resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightVP).getHandle();
                    RZResourceManager::Get().getUniformBufferResource(lightVPHandle)->SetData(sizeof(LightVPUBOData), &light_data);

                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        auto descriptor = shaderBindVars[resources.getResourceName<FrameGraph::RZFrameGraphBuffer>(data.lightVP)];
                        if (descriptor)
                            descriptor->uniformBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightVP).getHandle();

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    // Begin Rendering
                    RenderingInfo info{};    // No color attachment
                    info.resolution      = Resolution::kCustom;
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.shadowMap).getHandle(), {true, ClearColorPresets::OpaqueBlack}};
                    info.extent          = {kShadowMapSize, kShadowMapSize};
                    info.layerCount      = 1;
                    info.resize          = false;
                    RHI::BeginRendering(cmdBuffer, info);

                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Draw calls
                    // Get the meshes from the Scene and render them
                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::SceneGeometry);

                    // End Rendering
                    RHI::EndRendering(cmdBuffer);
                    RAZIX_MARK_END();
                });
        }

        void RZShadowPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix