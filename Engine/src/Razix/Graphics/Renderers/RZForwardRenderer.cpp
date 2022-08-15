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

#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZMesh.h"

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
            // So what does the renderer do then? it's job is to enforce some rules on the shader and it handles how it updates the data in a way that is appropriate for that renderer to operate
            m_Shader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/forward_renderer.rzsf");

            // Create the uniform buffers
            // 1. Create the View Projection UBOs
            m_ViewProjectionSystemUBO = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData, "System_ViewProjUBO");

            // 2. Lighting data
            m_ForwardLightUBO = Graphics::RZUniformBuffer::Create(sizeof(ForwardLightData), &m_ForwardLightData, "Forward Renderer Light Data");

            // Now create the descriptor sets for this and assign the UBOs for it
            // get the descriptor infos to create the descriptor sets
            auto setInfos = m_Shader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                for (auto& descriptor: setInfo.second) {
                    if (setInfo.first == 0)
                        descriptor.uniformBuffer = m_ViewProjectionSystemUBO;
                    else
                        descriptor.uniformBuffer = m_ForwardLightUBO;
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second);
                m_DescriptorSets.push_back(descSet);
            }
        }

        void RZForwardRenderer::InitDisposableResources()
        {
            // Render pass
            Graphics::AttachmentInfo textureTypes[2] = {
                {Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN},
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
            pipelineInfo.shader              = m_Shader;
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

            // Begin recording the command buffers
            Graphics::RZAPIRenderer::Begin();

            // Update the viewport
            Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer()->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

            // Begin the render pass
            m_RenderPass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), m_Framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, m_ScreenBufferWidth, m_ScreenBufferHeight);
        }

        void RZForwardRenderer::BeginScene(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

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
            m_ViewProjSystemUBOData.view       = m_Camera->getViewMatrix();
            m_ViewProjSystemUBOData.projection = m_Camera->getProjection();
            if (Graphics::RZGraphicsContext::GetRenderAPI() == RenderAPI::VULKAN)
                m_ViewProjSystemUBOData.projection[1][1] *= -1;
            m_ViewProjectionSystemUBO->SetData(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData);
            
            // Update the lighting information
            m_ForwardLightData.viewPos = m_Camera->getPosition();
            m_ForwardLightUBO->SetData(sizeof(ForwardLightData), &m_ForwardLightData);
            //auto lightEntities = registry.view<LightComponent>();
            // TODO: Iterate and get the data and update the UBO
            
            // Get the list of entities and their transform component together
            auto group = registry.group<Razix::Graphics::RZModel>(entt::get<TransformComponent, TagComponent>);
            for (auto entity: group) {
                const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);


                auto& meshes = model.getMeshes();

                // Bind the desc set, push constants, VBO, IBO and draw (or stack them into queue to be submitted after starting the recording operations)
            }
        }

        void RZForwardRenderer::Submit(RZCommandBuffer* cmdBuf)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZForwardRenderer::EndScene(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        }

        void RZForwardRenderer::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZForwardRenderer::Present()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZForwardRenderer::Resize(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZForwardRenderer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZForwardRenderer::OnEvent(RZEvent& event)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
        }

    }    // namespace Graphics
}    // namespace Razix