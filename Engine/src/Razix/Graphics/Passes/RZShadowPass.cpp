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

        void RZShadowPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Load the shader
            auto shader   = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::ShadowMapping);
            auto setInfos = shader->getSetsCreateInfos();

            m_LightViewProjUBO = RZUniformBuffer::Create(sizeof(LightVPUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("LightViewProj"));

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER)
                        descriptor.uniformBuffer = m_LightViewProjUBO;
                }
                m_LVPSet = (Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Shadow Depth pass set")));
            }

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::BACK;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader              = shader;
            pipelineInfo.transparencyEnabled = false;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthFormat         = {Graphics::TextureFormat::DEPTH32F};
            m_Pipeline                       = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Shadow Pass Pipeline"));

            blackboard.add<SimpleShadowPassData>() = framegraph.addCallbackPass<SimpleShadowPassData>(
                "Simple Shadow map pass",
                [&](SimpleShadowPassData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    data.shadowMap = builder.create<FrameGraph::RZFrameGraphTexture>("Shadow map", {.name = "Shadow map", .width = kShadowMapSize, .height = kShadowMapSize, .type = TextureType::Texture_Depth, .format = TextureFormat::DEPTH32F, .enableMips = false});

                    data.lightVP = builder.create<FrameGraph::RZFrameGraphBuffer>("LightSpaceMatrix", {"LightSpaceMatrix", sizeof(LightVPUBOData)});

                    data.shadowMap = builder.write(data.shadowMap);
                    data.lightVP   = builder.write(data.lightVP);
                },
                [=](const SimpleShadowPassData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Shadow Pass", glm::vec4(0.65, 0.73, 0.22f, 1.0f));

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
                    float     near_plane = -100.0f, far_plane = 50.0f;
                    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
                    lightProjection[1][1] *= -1;
                    light_data.lightViewProj = lightProjection * lightView;

                    resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightVP).getHandle()->SetData(sizeof(LightVPUBOData), &light_data);

                    // Begin Rendering
                    RenderingInfo info{};    // No color attachment
                    info.resolution = Resolution::kCustom;
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.shadowMap).getHandle(), {true, ClearColorPresets::OpaqueBlack}};
                    info.extent          = {kShadowMapSize, kShadowMapSize};
                    info.layerCount      = 1;
                    info.resize          = false;
                    RHI::BeginRendering(cmdBuffer, info);

                    m_Pipeline->Bind(cmdBuffer);

                    m_LightViewProjUBO->SetData(sizeof(LightVPUBOData), &light_data);

                    RHI::BindDescriptorSet(m_Pipeline, cmdBuffer, m_LVPSet, BindingTable_System::SET_IDX_SYSTEM_START);

                    // Draw calls
                    // Get the meshes from the Scene and render them
                    scene->drawScene(m_Pipeline, {.disableFrameData = true, .disableBindlessTextures = true, .disableLights = true, .disableMaterials = true});

                    // End Rendering
                    RHI::EndRendering(cmdBuffer);
                    RAZIX_MARK_END();
                });
        }

        void RZShadowPass::destroy()
        {
            m_LightViewProjUBO->Destroy();
            m_Pipeline->Destroy();
            m_LVPSet->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix