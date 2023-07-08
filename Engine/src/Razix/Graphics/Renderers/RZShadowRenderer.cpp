// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShadowRenderer.h"

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

        //-------------------------------------------------------------
        // IRZRenderer

        void RZShadowRenderer::Init()
        {
            m_LightViewProjUBO = RZUniformBuffer::Create(sizeof(LightVPUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("LightViewProj"));
        }

        void RZShadowRenderer::Begin(RZScene* scene)
        {
        }

        void RZShadowRenderer::Draw(RZCommandBuffer* cmdBuffer)
        {
        }

        void RZShadowRenderer::End()
        {
        }

        void RZShadowRenderer::Resize(u32 width, u32 height)
        {
        }

        void RZShadowRenderer::Destroy()
        {
            m_LightViewProjUBO->Destroy();
            m_Pipeline->Destroy();
            if (m_FrameDataSet)
                m_FrameDataSet->Destroy();
            m_LVPSet->Destroy();
        }

        void RZShadowRenderer::SetFrameDataHeap(RZDescriptorSet* frameDataSet)
        {
            m_FrameDataSet = frameDataSet;
        }

        //-------------------------------------------------------------
        // IRZPass

        void RZShadowRenderer::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // Load the shader
            auto shader   = RZShaderLibrary::Get().getShader("shadow_mapping.rzsf");
            auto setInfos = shader->getSetsCreateInfos();

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
            pipelineInfo.depthFormat         = {Graphics::RZTextureProperties::Format::DEPTH32F};
            m_Pipeline                       = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Shadow Pass Pipeline"));

            blackboard.add<SimpleShadowPassData>() = framegraph.addCallbackPass<SimpleShadowPassData>(
                "Simple Shadow map pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, SimpleShadowPassData& data) {
                    builder.setAsStandAlonePass();

                    data.shadowMap = builder.create<FrameGraph::RZFrameGraphTexture>("Shadow map", {.name = "Shadow map", .width = kShadowMapSize, .height = kShadowMapSize, .type = RZTextureProperties::Type::Texture_DepthTarget, .format = RZTextureProperties::Format::DEPTH32F});

                    data.lightVP = builder.create<FrameGraph::RZFrameGraphBuffer>("LightSpaceMatrix", {"LightSpaceMatrix", sizeof(LightVPUBOData)});

                    data.shadowMap = builder.write(data.shadowMap);
                    data.lightVP   = builder.write(data.lightVP);
                },
                [=](const SimpleShadowPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Shadow Pass", glm::vec4(0.65, 0.73, 0.22f, 1.0f));

                    // Update Viewport and Scissor Rect
                    RHI::GetCurrentCommandBuffer()->UpdateViewport(kShadowMapSize, kShadowMapSize);

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
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.shadowMap).getHandle(), {true, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}};
                    info.extent          = {kShadowMapSize, kShadowMapSize};
                    info.layerCount      = 1;
                    info.resize          = false;
                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    m_Pipeline->Bind(RHI::GetCurrentCommandBuffer());

                    m_LightViewProjUBO->SetData(sizeof(LightVPUBOData), &light_data);

                    //RHI::BindDescriptorSets(m_Pipeline, cmdBuf, &m_LVPSet, 1);

                    // Draw calls
                    // Get the meshes from the Scene and render them

                    scene->drawScene(m_Pipeline, nullptr, nullptr, {m_LVPSet}, nullptr, true);

                    // End Rendering
                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                    RAZIX_MARK_END();
                });
        }

        void RZShadowRenderer::destroy()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }    // namespace Graphics
}    // namespace Razix