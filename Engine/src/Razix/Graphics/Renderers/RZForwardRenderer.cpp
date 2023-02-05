// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZForwardRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZFramebuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZRenderPass.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZForwardRenderer::Init()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererName = "Forward Renderer";

            // Init the width and height of RT
            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            // This is a override shader that won't be used
            // Giving the shader to the renderer is not something I can think will be useful I think material will decide that
            // So what does the renderer do then? it's job is to enforce some rules on the shader (on the UBO data and samplers) and it handles how it updates the data in a way that is appropriate for that renderer to operate
            m_OverrideGlobalRHIShader = Graphics::RZShaderLibrary::Get().getShader("forward_renderer.rzsf");

            // Create the uniform buffers
            // 1. Create the View Projection UBOs
            m_SystemMVPUBO = Graphics::RZUniformBuffer::Create(sizeof(ModelViewProjectionSystemUBOData), &mvpData RZ_DEBUG_NAME_TAG_STR_E_ARG("System_MVP_UBO"));

            // 2. Lighting data
            m_ForwardLightsUBO = Graphics::RZUniformBuffer::Create(sizeof(GPULightsData), &gpuLightsData RZ_DEBUG_NAME_TAG_STR_E_ARG("Forward Renderer Light Data"));

            // Now create the descriptor sets for this and assign the UBOs for it
            // get the descriptor infos to create the descriptor sets
            auto setInfos = m_OverrideGlobalRHIShader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                        if (setInfo.first == BindingTable_System::BINDING_SET_SYSTEM_VIEW_PROJECTION) {
                            descriptor.uniformBuffer = m_SystemMVPUBO;
                            m_MVPDescriptorSet       = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_SYSTEM_VIEW_PROJECTION"));
                        } else if (setInfo.first == BindingTable_System::BINDING_SET_SYSTEM_LIGHTING_DATA) {
                            descriptor.uniformBuffer = m_ForwardLightsUBO;
                            m_GPULightsDescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_SYSTEM_LIGHTING_DATA"));
                        }
                    }
                }
            }

            // TODO: This is also to be moved to the renderer static initialization
            for (size_t i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_MainCommandBuffers[i] = RZCommandBuffer::Create();
                m_MainCommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Forward Renderer Main Command Buffers"));
            }

            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::NONE;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                 = m_OverrideGlobalRHIShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTexture::Format::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::RZTexture::Format::DEPTH16_UNORM;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;

            m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Forward Pipeline"));
        }

        void RZForwardRenderer::Begin(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            // Begin recording the command buffers
            Graphics::RHI::Begin(m_MainCommandBuffers[Graphics::RHI::getSwapchain()->getCurrentImageIndex()]);

            RAZIX_MARK_BEGIN("Forward Lighting Pass", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            // Update the viewport
            Graphics::RHI::getCurrentCommandBuffer()->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

            auto& registry = scene->getRegistry();

            // Get the camera
            // ! the view itself is the entity
            auto cameraView = registry.view<CameraComponent>();
            if (!cameraView.empty()) {
                // By using front we get the one and only or the first one in the list of camera entities
                m_Camera = &cameraView.get<CameraComponent>(cameraView.front()).Camera;
            }

            m_Camera->setAspectRatio((float) m_ScreenBufferWidth / (float) m_ScreenBufferHeight);

            mvpData.viewProjection = m_Camera->getViewProjection();

            // Upload the lights data
            auto group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
            for (auto entity: group) {
                const auto& [lightComponent, transformComponent] = group.get<LightComponent, TransformComponent>(entity);

                // Set the Position of the light using this transform component
                lightComponent.light.getLightData().position     = transformComponent.Translation;
                gpuLightsData.lightData[gpuLightsData.numLights] = lightComponent.light.getLightData();

                gpuLightsData.numLights++;
            }

            m_ForwardLightsUBO->SetData(sizeof(GPULightsData), &gpuLightsData);

            gpuLightsData = {};

            m_CurrentScene = scene;
        }

        void RZForwardRenderer::Draw(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Bind the pipeline
            m_Pipeline->Bind(cmdBuffer);

            // Get the list of entities and their transform component together
            auto& group = m_CurrentScene->getRegistry().group<Razix::Graphics::RZModel>(entt::get<TransformComponent>);
            for (auto entity: group) {
                const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);

                auto& meshes = model.getMeshes();

                // Bind push constants, VBO, IBO and draw
                glm::mat4 transform = trans.GetTransform();

                mvpData.model = transform;
                m_SystemMVPUBO->SetData(sizeof(ModelViewProjectionSystemUBOData), &mvpData);

                // Bind IBO and VBO
                for (auto& mesh: meshes) {
                    mesh->getVertexBuffer()->Bind(cmdBuffer);
                    mesh->getIndexBuffer()->Bind(cmdBuffer);

                    mesh->getMaterial()->Bind();

                    // Combine System Desc sets with material sets and Bind them
                    std::vector<RZDescriptorSet*> setsToBindInOrder = {m_MVPDescriptorSet, mesh->getMaterial()->getDescriptorSet(), m_GPULightsDescriptorSet};
                    Graphics::RHI::BindDescriptorSets(m_Pipeline, cmdBuffer, setsToBindInOrder);

                    Graphics::RHI::DrawIndexed(Graphics::RHI::getCurrentCommandBuffer(), mesh->getIndexCount());
                }
            }

            auto& mesh_group = m_CurrentScene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
            for (auto entity: mesh_group) {
                // Draw the mesh renderer components
                const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                // Bind push constants, VBO, IBO and draw
                glm::mat4 transform = mesh_trans.GetTransform();

                mvpData.model = transform;
                m_SystemMVPUBO->SetData(sizeof(ModelViewProjectionSystemUBOData), &mvpData);

                // Combine System Desc sets with material sets and Bind them
                std::vector<RZDescriptorSet*> setsToBindInOrder = {m_MVPDescriptorSet, mrc.Mesh->getMaterial()->getDescriptorSet(), m_GPULightsDescriptorSet};
                Graphics::RHI::BindDescriptorSets(m_Pipeline, cmdBuffer, setsToBindInOrder);

                mrc.Mesh->getVertexBuffer()->Bind(cmdBuffer);
                mrc.Mesh->getIndexBuffer()->Bind(cmdBuffer);

                Graphics::RHI::DrawIndexed(Graphics::RHI::getCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
            }
        }

        void RZForwardRenderer::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RHI::EndRendering(Graphics::RHI::getCurrentCommandBuffer());

            RAZIX_MARK_END();
        }

        void RZForwardRenderer::Resize(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = width;
            m_ScreenBufferHeight = height;

            // Destroy the resources first
            m_DepthTexture->Release(true);
            //m_Pipeline->Destroy();
        }

        void RZForwardRenderer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy the resources first
            m_DepthTexture->Release(true);

            m_GPULightsDescriptorSet->Destroy();
            m_MVPDescriptorSet->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix
