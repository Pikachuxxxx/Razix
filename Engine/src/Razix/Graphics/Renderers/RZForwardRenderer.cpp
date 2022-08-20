// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZForwardRenderer.h"

#include "Razix/Core/RZApplication.h"

#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZFramebuffer.h"
#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZRenderPass.h"
#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZSwapchain.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"

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
            m_OverrideGlobalRHIShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/forward_renderer.rzsf");

            // Create the uniform buffers
            // 1. Create the View Projection UBOs
            m_ViewProjectionSystemUBO = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData, "System_ViewProjUBO");

            // 2. Lighting data
            m_ForwardLightUBO = Graphics::RZUniformBuffer::Create(sizeof(ForwardLightData), &m_ForwardLightData, "Forward Renderer Light Data");

            // 3. Material data
            m_TempMatUBO = Graphics::RZUniformBuffer::Create(sizeof(PBRMaterialProperties), &m_TempMatProps, "Temp Mat");

            // Now create the descriptor sets for this and assign the UBOs for it
            // get the descriptor infos to create the descriptor sets
            auto setInfos = m_OverrideGlobalRHIShader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                        if (setInfo.first == 0)
                            descriptor.uniformBuffer = m_ViewProjectionSystemUBO;
                        else if (setInfo.first == 1)
                            descriptor.uniformBuffer = m_ForwardLightUBO;
                        else if (setInfo.first == 2) {
                            descriptor.uniformBuffer = m_TempMatUBO;
                        }
                    } else if (descriptor.bindingInfo.type == DescriptorType::IMAGE_SAMPLER) {
                        uint32_t pinkTextureData = 0xffff00ff;    // is it ABGR
                        // All maps will have the same pink texture
                        descriptor.texture = Graphics::RZTexture2D::CreateFromFile("//Textures/Avocado_baseColor.png", "Albedo", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);

                        //Graphics::RZTexture2D::Create("Default Texture", 1, 1, &pinkTextureData, RZTexture::Format::RGBA8);
                    }
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second);
                m_DescriptorSets.push_back(descSet);
            }

            InitDisposableResources();

            // TODO: This is also to be moved to the renderer static initialization
            for (size_t i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_MainCommandBuffers[i] = RZCommandBuffer::Create();
                m_MainCommandBuffers[i]->Init();
            }
        }

        void RZForwardRenderer::InitDisposableResources()
        {
            // Render pass
            Graphics::AttachmentInfo textureTypes[2] = {
                {Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::BGRA8_UNORM},
                {Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH}};

            Graphics::RenderPassInfo renderPassInfo{};
            renderPassInfo.attachmentCount = 2;
            renderPassInfo.textureType     = textureTypes;
            renderPassInfo.name            = "Forward rendering";
            renderPassInfo.clear           = false;

            m_RenderPass = Graphics::RZRenderPass::Create(renderPassInfo);

            // Create the graphics pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.renderpass          = m_RenderPass;
            pipelineInfo.shader              = m_OverrideGlobalRHIShader;
            pipelineInfo.transparencyEnabled = true;

            m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo);

            // Framebuffer (we need on per frame ==> 3 in total)
            // Create the framebuffer
            Graphics::RZTexture::Type attachmentTypes[2];
            attachmentTypes[0] = Graphics::RZTexture::Type::COLOR;
            attachmentTypes[1] = Graphics::RZTexture::Type::DEPTH;

            auto swapImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();
            m_DepthTexture    = Graphics::RZDepthTexture::Create(m_ScreenBufferWidth, m_ScreenBufferHeight);

            m_Framebuffers.clear();
            for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
                Graphics::RZTexture* attachments[2];
                attachments[0] = Graphics::RZAPIRenderer::getSwapchain()->GetImage(i);
                attachments[1] = m_DepthTexture;

                Graphics::FramebufferInfo frameBufInfo{};
                frameBufInfo.width           = m_ScreenBufferWidth;
                frameBufInfo.height          = m_ScreenBufferHeight;
                frameBufInfo.attachmentCount = 2;
                frameBufInfo.renderPass      = m_RenderPass;
                frameBufInfo.attachments     = attachments;

                m_Framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
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

            // Update the viewport
            Graphics::RZAPIRenderer::getCurrentCommandBuffer()->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

            // Begin the render pass
            m_RenderPass->BeginRenderPass(Graphics::RZAPIRenderer::getCurrentCommandBuffer(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), m_Framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, m_ScreenBufferWidth, m_ScreenBufferHeight);
        }

        void RZForwardRenderer::BeginScene(RZScene* scene)
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
            //m_Camera->setPerspectiveFarClip(6000.0f);
            m_ViewProjSystemUBOData.view       = m_Camera->getViewMatrix();
            m_ViewProjSystemUBOData.projection = m_Camera->getProjection();
            if (Graphics::RZGraphicsContext::GetRenderAPI() == RenderAPI::VULKAN)
                m_ViewProjSystemUBOData.projection[1][1] *= -1;
            m_ViewProjectionSystemUBO->SetData(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData);

            // Update the lighting information
            m_ForwardLightData.viewPos = m_Camera->getPosition();
            m_ForwardLightUBO->SetData(sizeof(ForwardLightData), &m_ForwardLightData);
            //auto lightEntities = registry.view<LightComponent>();
            // TODO: Iterate and get the light components + Material data from the mesh and update the UBO
        }

        void RZForwardRenderer::Submit(RZCommandBuffer* cmdBuf)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Bind the pipeline
            m_Pipeline->Bind(cmdBuf);
            // Bind the sets
            Graphics::RZAPIRenderer::BindDescriptorSets(m_Pipeline, cmdBuf, m_DescriptorSets);

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

                modelMatrix.data = glm::value_ptr(transform);
                modelMatrix.size = sizeof(glm::mat4);

                Graphics::RZAPIRenderer::BindPushConstant(m_Pipeline, cmdBuf, modelMatrix);

                // Bind IBO and VBO
                for (auto& mesh: meshes) {
                    mesh->getVertexBuffer()->Bind(cmdBuf);
                    mesh->getIndexBuffer()->Bind(cmdBuf);

                    Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getCurrentCommandBuffer(), mesh->getIndexCount());
                }
            }
        }

        void RZForwardRenderer::EndScene(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZForwardRenderer::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // End the render pass and recording
            m_RenderPass->EndRenderPass(Graphics::RZAPIRenderer::getCurrentCommandBuffer());

            //Graphics::RZAPIRenderer::Submit(Graphics::RZAPIRenderer::getCurrentCommandBuffer());
        }

        void RZForwardRenderer::Present()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

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

            for (auto frameBuf: m_Framebuffers)
                frameBuf->Destroy();

            m_RenderPass->Destroy();

            m_Pipeline->Destroy();

            Graphics::RZAPIRenderer::OnResize(width, height);

            InitDisposableResources();
        }

        void RZForwardRenderer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy the resources first
            m_DepthTexture->Release(true);

            m_OverrideGlobalRHIShader->Destroy();

            m_ViewProjectionSystemUBO->Destroy();

            m_ForwardLightUBO->Destroy();
            m_TempMatUBO->Destroy();

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