// clang-format off
#include "rzxpch.h"
// clang-format on
#if 0
#include "RZForwardRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

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
            m_ViewProjectionSystemUBO = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData RZ_DEBUG_NAME_TAG_STR_E_ARG("System_ViewProjUBO"));

            // 2. Lighting data
            m_ForwardLightUBO = Graphics::RZUniformBuffer::Create(sizeof(ForwardLightData), &m_ForwardLightData RZ_DEBUG_NAME_TAG_STR_E_ARG("Forward Renderer Light Data"));

            // Now create the descriptor sets for this and assign the UBOs for it
            // get the descriptor infos to create the descriptor sets
            auto setInfos = m_OverrideGlobalRHIShader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                        if (setInfo.first == BindingTable_System::BINDING_SET_SYSTEM_VIEW_PROJECTION) {
                            descriptor.uniformBuffer = m_ViewProjectionSystemUBO;
                            auto descSet             = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_SYSTEM_VIEW_PROJECTION"));
                            m_DescriptorSets.push_back(descSet);
                        } else if (setInfo.first == BindingTable_System::BINDING_SET_SYSTEM_FORWARD_LIGHTING) {
                            descriptor.uniformBuffer = m_ForwardLightUBO;
                            auto descSet             = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_SYSTEM_FORWARD_LIGHTING"));
                            m_DescriptorSets.push_back(descSet);
                        }
                    }
                }
            }

            InitDisposableResources();

            // TODO: This is also to be moved to the renderer static initialization
            for (size_t i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_MainCommandBuffers[i] = RZCommandBuffer::Create();
                m_MainCommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Forward Renderer Main Command Buffers"));
            }
        }

        void RZForwardRenderer::InitDisposableResources()
        {
            constexpr uint32_t attachmentsCount = 3;
            // Render pass
            Graphics::RenderPassAttachmentInfo textureTypes[attachmentsCount] = {
                {Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::BGRA8_UNORM, false},
                {Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::R32_INT, true},
                {Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH, true}};

            Graphics::RenderPassInfo renderPassInfo{};
            renderPassInfo.attachmentCount = attachmentsCount;
            renderPassInfo.attachmentInfos = textureTypes;
            renderPassInfo.name            = "Forward rendering";

            m_RenderPass = Graphics::RZRenderPass::Create(renderPassInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Forward Pass"));

            // Create the graphics pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode         = Graphics::CullMode::NONE;
            pipelineInfo.depthBiasEnabled = false;
            pipelineInfo.drawType         = Graphics::DrawType::TRIANGLE;
            pipelineInfo.renderpass       = m_RenderPass;
            pipelineInfo.shader           = m_OverrideGlobalRHIShader;
            // This causes validation errors for some VK image formats that are not the typical color attachments
            pipelineInfo.transparencyEnabled = false;    // TODO: This should be configurable for each attachment in the renderpass

            m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Forward Pipeline"));

            // Framebuffer (we need on per frame ==> 3 in total)
            // Create the framebuffer
            Graphics::RZTexture::Type attachmentTypes[attachmentsCount];
            attachmentTypes[0] = Graphics::RZTexture::Type::COLOR;
            attachmentTypes[1] = Graphics::RZTexture::Type::COLOR;
            attachmentTypes[2] = Graphics::RZTexture::Type::DEPTH;

            auto swapImgCount = Graphics::RZRHI::getSwapchain()->GetSwapchainImageCount();
            m_EntityIDsRT     = Graphics::RZRenderTexture::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Entity IDs RT") m_ScreenBufferWidth, m_ScreenBufferHeight, RZTexture::Format::R32_INT);
            m_DepthTexture    = Graphics::RZDepthTexture::Create(m_ScreenBufferWidth, m_ScreenBufferHeight);

            m_Framebuffers.clear();
            for (size_t i = 0; i < Graphics::RZRHI::getSwapchain()->GetSwapchainImageCount(); i++) {
                Graphics::RZTexture* attachments[attachmentsCount];
                attachments[0] = Graphics::RZRHI::getSwapchain()->GetImage(i);
                attachments[1] = m_EntityIDsRT;
                attachments[2] = m_DepthTexture;

                Graphics::FramebufferInfo frameBufInfo{};
                frameBufInfo.width           = m_ScreenBufferWidth;
                frameBufInfo.height          = m_ScreenBufferHeight;
                frameBufInfo.attachmentCount = attachmentsCount;
                frameBufInfo.renderPass      = m_RenderPass;
                frameBufInfo.attachments     = attachments;

                m_Framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Forward Renderer FB")));
            }
        }

        void RZForwardRenderer::Begin()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            //Graphics::RZAPIRenderer::AcquireImage();

            // Begin recording the command buffers
            //Graphics::RZAPIRenderer::Begin(m_MainCommandBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]);

            RAZIX_MARK_BEGIN(*(VkCommandBuffer*) Graphics::RZRHI::getCurrentCommandBuffer()->getAPIBuffer(), "FOrward Renderer Pass", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            // Update the viewport
            Graphics::RZRHI::getCurrentCommandBuffer()->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

            // Begin the render pass
            m_RenderPass->BeginRenderPass(Graphics::RZRHI::getCurrentCommandBuffer(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), m_Framebuffers[Graphics::RZRHI::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, m_ScreenBufferWidth, m_ScreenBufferHeight);
        }

        void RZForwardRenderer::BeginScene(Razix::RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_CurrentScene = scene;

            auto& registry = scene->getRegistry();

            TransformComponent* m_CamTransform = nullptr;

            // Get the camera
            // ! the view itself is the entity
            auto cameraView = registry.view<CameraComponent>();
            if (!cameraView.empty()) {
                // By using front we get the one and only or the first one in the list of camera entities
                m_Camera       = &cameraView.get<CameraComponent>(cameraView.front()).Camera;
                m_CamTransform = registry.try_get<TransformComponent>(cameraView.front());
            }

            // Update the View Projection UBO
            m_Camera->setPerspectiveFarClip(6000.0f);
            m_Camera->setAspectRatio((float) m_ScreenBufferWidth / (float) m_ScreenBufferHeight);
            m_ViewProjSystemUBOData.view       = m_Camera->getViewMatrix();
            m_ViewProjSystemUBOData.projection = m_Camera->getProjection();
            if (Graphics::RZGraphicsContext::GetRenderAPI() == RenderAPI::VULKAN)
                m_ViewProjSystemUBOData.projection[1][1] *= -1;
            m_ViewProjectionSystemUBO->SetData(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData);

            // Update the lighting information
            m_ForwardLightData.viewPos = m_Camera->getPosition();
            //auto lightEntities = registry.view<LightComponent>();
            auto group = registry.group<LightComponent>(entt::get<TransformComponent>);
            for (auto entity: group) {
                const auto& [light, trans]   = group.get<LightComponent, TransformComponent>(entity);
                m_ForwardLightData.lightData = light.light.getLightData();
                m_ForwardLightData.position  = trans.Translation;
            }
            m_ForwardLightUBO->SetData(sizeof(ForwardLightData), &m_ForwardLightData);
        }

        void RZForwardRenderer::Submit(RZCommandBuffer* cmdBuf)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Bind the pipeline
            m_Pipeline->Bind(cmdBuf);

            auto& registry = m_CurrentScene->getRegistry();

            // Get the list of entities and their transform component together
            auto group = registry.group<Razix::Graphics::RZModel>(entt::get<TransformComponent, TagComponent>);
            for (auto entity: group) {
                const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);

                auto& meshes = model.getMeshes();

                // Bind push constants, VBO, IBO and draw
                glm::mat4 transform = trans.GetTransform();

                // Get the shader from the Mesh Material later
                // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                auto& modelMatrix = m_OverrideGlobalRHIShader->getPushConstants()[0];

                // Ehhh... not the neatest thing to be here but I'll let this one slide for now
                struct PCD
                {
                    glm::mat4 mat;
                    int32_t   ID;
                } pcData;
                pcData.mat       = transform;
                pcData.ID        = int32_t(entity);
                modelMatrix.data = &pcData;
                modelMatrix.size = sizeof(PCD);

                // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                Graphics::RZRHI::BindPushConstant(m_Pipeline, cmdBuf, modelMatrix);

                // Bind IBO and VBO
                for (auto& mesh: meshes) {
                    mesh->getVertexBuffer()->Bind(cmdBuf);
                    mesh->getIndexBuffer()->Bind(cmdBuf);

                    mesh->getMaterial()->Bind();

                    // Combine System Desc sets with material sets and Bind them
                    std::vector<RZDescriptorSet*> SystemMat = m_DescriptorSets;
                    std::vector<RZDescriptorSet*> MatSets   = mesh->getMaterial()->getDescriptorSets();
                    SystemMat.insert(SystemMat.end(), MatSets.begin(), MatSets.end());
                    Graphics::RZRHI::BindDescriptorSets(m_Pipeline, cmdBuf, SystemMat);

                    Graphics::RZRHI::DrawIndexed(Graphics::RZRHI::getCurrentCommandBuffer(), mesh->getIndexCount());
                }
            }

            auto mesh_group = registry.group<MeshRendererComponent>(entt::get<TransformComponent, TagComponent>);
            for (auto entity: mesh_group) {
                // Draw the mesh renderer components
                const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                // Bind push constants, VBO, IBO and draw
                glm::mat4 transform = mesh_trans.GetTransform();

                // Get the shader from the Mesh Material later
                // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                auto& modelMatrix = m_OverrideGlobalRHIShader->getPushConstants()[0];

                // Ehhh... not the neatest thing to be here but I'll let this one slide for now
                struct PCD
                {
                    glm::mat4 mat;
                    int32_t   ID;
                } pcData;
                pcData.mat       = transform;
                pcData.ID        = int32_t(entity);
                modelMatrix.data = &pcData;
                modelMatrix.size = sizeof(PCD);

                Graphics::RZRHI::BindPushConstant(m_Pipeline, cmdBuf, modelMatrix);

                std::vector<RZDescriptorSet*> SystemMat = m_DescriptorSets;
                std::vector<RZDescriptorSet*> MatSets   = mrc.Mesh->getMaterial()->getDescriptorSets();
                SystemMat.insert(SystemMat.end(), MatSets.begin(), MatSets.end());

                Graphics::RZRHI::BindDescriptorSets(m_Pipeline, cmdBuf, SystemMat);

                mrc.Mesh->getVertexBuffer()->Bind(cmdBuf);
                mrc.Mesh->getIndexBuffer()->Bind(cmdBuf);

                Graphics::RZRHI::DrawIndexed(Graphics::RZRHI::getCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
            }
        }

        void RZForwardRenderer::EndScene(Razix::RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZForwardRenderer::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // End the render pass and recording
            m_RenderPass->EndRenderPass(Graphics::RZRHI::getCurrentCommandBuffer());

            RAZIX_MARK_END(*(VkCommandBuffer*) Graphics::RZRHI::getCurrentCommandBuffer()->getAPIBuffer());

            //Graphics::RZAPIRenderer::Submit(Graphics::RZAPIRenderer::getCurrentCommandBuffer());
        }

        void RZForwardRenderer::Present()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            float now = m_RendererTimer.GetElapsedS();
            m_PassTimer.Update(now);
            RZEngine::Get().GetStatistics().ForwardLightingPass = abs(RZEngine::Get().GetStatistics().DeltaTime - m_PassTimer.GetTimestepMs());

            //Graphics::RZAPIRenderer::SubmitWork();
            //Graphics::RZAPIRenderer::Present();
        }

        void RZForwardRenderer::Resize(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = width;
            m_ScreenBufferHeight = height;

            // Destroy the resources first
            m_DepthTexture->Release(true);
            m_EntityIDsRT->Resize(width, height RZ_DEBUG_NAME_TAG_STR_E_ARG("Entity IDs RT Forward Renderer"));

            for (auto frameBuf: m_Framebuffers)
                frameBuf->Destroy();

            m_RenderPass->Destroy();

            m_Pipeline->Destroy();

            Graphics::RZRHI::OnResize(width, height);

            InitDisposableResources();
        }

        void RZForwardRenderer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy the resources first
            m_DepthTexture->Release(true);
            m_EntityIDsRT->Release(true);
            //m_OverrideGlobalRHIShader->Destroy();

            m_ViewProjectionSystemUBO->Destroy();

            m_ForwardLightUBO->Destroy();

            for (auto set: m_DescriptorSets)
                set->Destroy();

            for (auto frameBuf: m_Framebuffers)
                frameBuf->Destroy();

            m_RenderPass->Destroy();

            m_Pipeline->Destroy();
        }

        void RZForwardRenderer::OnEvent(RZEvent& event)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
        }

    }    // namespace Graphics
}    // namespace Razix
#endif