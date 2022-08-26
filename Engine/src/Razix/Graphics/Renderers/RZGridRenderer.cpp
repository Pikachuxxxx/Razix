// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGridRenderer.h"

#include "Razix/Core/RZApplication.h"

#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZFramebuffer.h"
#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZRenderPass.h"
#include "Razix/Graphics/API/RZSwapchain.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"

#include "Razix/Scene/Components/RZComponents.h"

namespace Razix {
    namespace Graphics {

        void RZGridRenderer::Init()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererName = "Grid Renderer";

            // Init the width and height of RT
            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            // Load the grid shader
            m_OverrideGlobalRHIShader = Graphics::RZShaderLibrary::Get().getShader("grid.rzsf");

            // TODO: Use a set of 2 UBOs and DescSets (per frame in flight)

            // Create the uniform buffers
            // 1. Create the View Projection UBOs
            m_ViewProjectionSystemUBO = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData, "System_ViewProjUBO");

            // 2. Create the Grid related UBO and data
            m_GridUBO = Graphics::RZUniformBuffer::Create(sizeof(GridUBOData), &m_GridUBOData, "Grid Data");

            // Now create the descriptor sets for this and assign the UBOs for it
            // get the descriptor infos to create the descriptor sets
            // Well don't use any binding tables here this is fine!!!! fuckin smart ass mf!! FUCK OFF!!!!
            auto setInfos = m_OverrideGlobalRHIShader->getSetsCreateInfos();
            int  j        = 0;
            for (auto& setInfo: setInfos) {
                for (auto& descriptor: setInfo.second) {
                    if (j == 0)
                        descriptor.uniformBuffer = m_ViewProjectionSystemUBO;
                    else
                        descriptor.uniformBuffer = m_GridUBO;
                    j++;
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second);
                m_DescriptorSets.push_back(descSet);
            }

            InitDisposableResources();

            float vertices[8 * 4] = {-2000.5f, 0.0f, -2000.5f, 0.0f, 0.0f, 2000.5f, 0.0f, -2000.5f, 1.0f, 0.0f, 2000.5f, 0.0f, 2000.5f, 1.0f, 1.0f, -2000.5f, 0.0f, 2000.5f, 0.0f, 1.0f};

            uint16_t indices[6] = {0, 1, 2, 2, 3, 0};

            // This buffer layout will be somehow combined with the vertex buffers and passed to the pipeline for the Input Assembly stage
            RZVertexBufferLayout bufferLayout;
            bufferLayout.push<glm::vec3>("Position");
            bufferLayout.push<glm::vec2>("TexCoord");

            gridVBO = Graphics::RZVertexBuffer::Create(sizeof(float) * 8 * 4, vertices, Graphics::BufferUsage::STATIC, "Grid VBO");
            gridVBO->AddBufferLayout(bufferLayout);
            gridIBO = Graphics::RZIndexBuffer::Create(indices, 6, "Grid IBO");

            // TODO: This is also to be moved to the renderer static initialization
            for (size_t i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_MainCommandBuffers[i] = RZCommandBuffer::Create();
                m_MainCommandBuffers[i]->Init();
            }
        }

        void RZGridRenderer::InitDisposableResources()
        {
            // Render pass
            Graphics::AttachmentInfo textureTypes[2] = {
                {Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::BGRA8_UNORM, true},
                {Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH, true}};

            Graphics::RenderPassInfo renderPassInfo{};
            renderPassInfo.attachmentCount = 2;
            renderPassInfo.textureType     = textureTypes;
            renderPassInfo.name            = "Grid rendering";

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
            for (uint32_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
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

        void RZGridRenderer::Begin()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            // Get a swapchain image to render to
            Graphics::RZAPIRenderer::AcquireImage();

            // Begin recording the command buffers
            Graphics::RZAPIRenderer::Begin(m_MainCommandBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]);

            // Update the viewport
            Graphics::RZAPIRenderer::getCurrentCommandBuffer()->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

            // Begin the render pass
            m_RenderPass->BeginRenderPass(Graphics::RZAPIRenderer::getCurrentCommandBuffer(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), m_Framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, m_ScreenBufferWidth, m_ScreenBufferHeight);
        }

        void RZGridRenderer::BeginScene(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto& registry = scene->getRegistry();

            TransformComponent* m_CamTransform = nullptr;

            // Get the camera
            auto cameraView = registry.view<CameraComponent>();
            if (!cameraView.empty()) {
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

            // Update the Grid UBO
            m_GridUBOData.cameraPos = m_Camera->getPosition();
            m_GridUBO->SetData(sizeof(GridUBOData), &m_GridUBOData);
        }

        void RZGridRenderer::Submit(RZCommandBuffer* cmdBuf)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Submit work to GPU by binding the pipeline , buffers etc and recoding the draw command

            // Bind the pipeline
            m_Pipeline->Bind(cmdBuf);

            // Bind the descriptor set
            Graphics::RZAPIRenderer::BindDescriptorSets(m_Pipeline, cmdBuf, m_DescriptorSets);

            // Bind the appropriate buffers/mesh
            gridVBO->Bind(cmdBuf);
            gridIBO->Bind(cmdBuf);

            // Issues the Draw Commands
            Graphics::RZAPIRenderer::DrawIndexed(cmdBuf, 6);
        }

        void RZGridRenderer::EndScene(RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZGridRenderer::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // End the render pass and recording
            m_RenderPass->EndRenderPass(Graphics::RZAPIRenderer::getCurrentCommandBuffer());

            //Graphics::RZAPIRenderer::Submit(Graphics::RZAPIRenderer::getCurrentCommandBuffer());
        }

        void RZGridRenderer::Present()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //Graphics::RZAPIRenderer::SubmitWork();
            //Graphics::RZAPIRenderer::Present();
        }

        void RZGridRenderer::Resize(uint32_t width, uint32_t height)
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

        void RZGridRenderer::Destroy()
        {
            // Destroy the resources first
            m_DepthTexture->Release(true);

            //m_OverrideGlobalRHIShader->Destroy();

            m_ViewProjectionSystemUBO->Destroy();
            m_GridUBO->Destroy();

            gridVBO->Destroy();
            gridIBO->Destroy();

            for (auto set: m_DescriptorSets)
                set->Destroy();

            for (auto frameBuf: m_Framebuffers)
                frameBuf->Destroy();

            m_RenderPass->Destroy();

            m_Pipeline->Destroy();
        }

        void RZGridRenderer::OnEvent(RZEvent& event)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            throw std::logic_error("The method or operation is not implemented.");
        }
    }    // namespace Graphics
}    // namespace Razix