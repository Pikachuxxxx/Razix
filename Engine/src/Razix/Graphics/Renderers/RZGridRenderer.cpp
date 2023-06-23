// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGridRenderer.h"
#if 0

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
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Scene/Components/RZComponents.h"

#include <RZMemoryFunctions.h>

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
            m_ViewProjectionSystemUBO = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData RZ_DEBUG_NAME_TAG_STR_E_ARG("System_ViewProjUBO"));

            // 2. Create the Grid related UBO and data
            m_GridUBO = Graphics::RZUniformBuffer::Create(sizeof(GridUBOData), &m_GridUBOData RZ_DEBUG_NAME_TAG_STR_E_ARG("Grid Data"));

            // Now create the descriptor sets for this and assign the UBOs for it
            // get the descriptor infos to create the descriptor sets
            // Well don't use any binding tables for this simple Gird Renderer shader here this is fine!!!! fuckin keep it simple!!
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
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Grid Renderer Set"));
                m_DescriptorSets.push_back(descSet);
            }

            InitDisposableResources();

            f32 vertices[8 * 4] = {-2000.5f, 0.0f, -2000.5f, 0.0f, 0.0f, 2000.5f, 0.0f, -2000.5f, 1.0f, 0.0f, 2000.5f, 0.0f, 2000.5f, 1.0f, 1.0f, -2000.5f, 0.0f, 2000.5f, 0.0f, 1.0f};

            u16 indices[6] = {0, 1, 2, 2, 3, 0};

            // This buffer layout will be somehow combined with the vertex buffers and passed to the pipeline for the Input Assembly stage
            RZVertexBufferLayout bufferLayout;
            bufferLayout.push<glm::vec3>("Position");
            bufferLayout.push<glm::vec2>("TexCoord");

            gridVBO = Graphics::RZVertexBuffer::Create(sizeof(f32) * 8 * 4, vertices, Graphics::BufferUsage::STATIC RZ_DEBUG_NAME_TAG_STR_E_ARG("Grid VBO"));
            gridVBO->AddBufferLayout(bufferLayout);
            gridIBO = Graphics::RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Grid IBO") indices, 6);

            // TODO: This is also to be moved to the renderer static initialization
            for (sz i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_MainCommandBuffers[i] = RZCommandBuffer::Create();
                m_MainCommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Grid Renderer Main Command Buffers"));
            }
        }

        void RZGridRenderer::InitDisposableResources()
        {
            // Render pass
            Graphics::RenderPassAttachmentInfo textureTypes[2] = {
                {Graphics::RZTextureProperties::Type::COLOR, Graphics::RZTextureProperties::Format::BGRA8_UNORM, true},
                {Graphics::RZTextureProperties::Type::DEPTH, Graphics::RZTextureProperties::Format::DEPTH, true}};

            Graphics::RenderPassInfo renderPassInfo{};
            renderPassInfo.attachmentCount = 2;
            renderPassInfo.attachmentInfos = textureTypes;
            renderPassInfo.name            = "Grid rendering";

            m_RenderPass = Graphics::RZRenderPass::Create(renderPassInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Grid Pass"));

            // Create the graphics pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.renderpass          = m_RenderPass;
            pipelineInfo.shader              = m_OverrideGlobalRHIShader;
            pipelineInfo.transparencyEnabled = true;

            m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Grid Pipeline"));

            // Framebuffer (we need on per frame ==> 3 in total)
            // Create the framebuffer
            Graphics::RZTextureProperties::Type attachmentTypes[2];
            attachmentTypes[0] = Graphics::RZTextureProperties::Type::COLOR;
            attachmentTypes[1] = Graphics::RZTextureProperties::Type::DEPTH;

            auto swapImgCount = Graphics::RZRHI::getSwapchain()->GetSwapchainImageCount();
            m_DepthTexture    = Graphics::RZDepthTexture::Create(m_ScreenBufferWidth, m_ScreenBufferHeight);

            m_Framebuffers.clear();
            for (u32 i = 0; i < Graphics::RZRHI::getSwapchain()->GetSwapchainImageCount(); i++) {
                Graphics::RZTexture* attachments[2];
                attachments[0] = Graphics::RZRHI::getSwapchain()->GetImage(i);
                attachments[1] = m_DepthTexture;

                Graphics::FramebufferInfo frameBufInfo{};
                frameBufInfo.width           = m_ScreenBufferWidth;
                frameBufInfo.height          = m_ScreenBufferHeight;
                frameBufInfo.attachmentCount = 2;
                frameBufInfo.renderPass      = m_RenderPass;
                frameBufInfo.attachments     = attachments;

                m_Framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Grid Renderer FB")));
            }
        }

        void RZGridRenderer::Begin()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            auto cmdBuf = m_MainCommandBuffers[Graphics::RZRHI::getSwapchain()->getCurrentImageIndex()];
            // Get a swapchain image to render to
            Graphics::RZRHI::AcquireImage();

            // Begin recording the command buffers
            Graphics::RZRHI::Begin(cmdBuf);

            RAZIX_MARK_BEGIN(*(VkCommandBuffer*) cmdBuf->getAPIBuffer(), "Grid Pass", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

            // Update the viewport
            Graphics::RZRHI::getCurrentCommandBuffer()->UpdateViewport(m_ScreenBufferWidth, m_ScreenBufferHeight);

            // Begin the render pass
            m_RenderPass->BeginRenderPass(Graphics::RZRHI::getCurrentCommandBuffer(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), m_Framebuffers[Graphics::RZRHI::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, m_ScreenBufferWidth, m_ScreenBufferHeight);
        }

        void RZGridRenderer::BeginScene(Razix::RZScene* scene)
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
            Graphics::RZRHI::BindDescriptorSets(m_Pipeline, cmdBuf, m_DescriptorSets);

            // Bind the appropriate buffers/mesh
            gridVBO->Bind(cmdBuf);
            gridIBO->Bind(cmdBuf);

            // Issues the Draw Commands
            Graphics::RZRHI::DrawIndexed(cmdBuf, 6);
        }

        void RZGridRenderer::EndScene(Razix::RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void RZGridRenderer::End()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // End the render pass
            m_RenderPass->EndRenderPass(Graphics::RZRHI::getCurrentCommandBuffer());

            //Graphics::RZAPIRenderer::Submit(Graphics::RZAPIRenderer::getCurrentCommandBuffer());

            RAZIX_MARK_END(*(VkCommandBuffer*) Graphics::RZRHI::getCurrentCommandBuffer()->getAPIBuffer());
        }

        void RZGridRenderer::Present()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32 now = m_RendererTimer.GetElapsedS();
            m_PassTimer.Update(now);
            RZEngine::Get().GetStatistics().GridPass = abs(RZEngine::Get().GetStatistics().DeltaTime - m_PassTimer.GetTimestepMs());

            //Graphics::RZAPIRenderer::SubmitWork();
            //Graphics::RZAPIRenderer::Present();
        }

        void RZGridRenderer::Resize(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_ScreenBufferHeight == height && m_ScreenBufferWidth == width)
                return;

            m_ScreenBufferWidth  = width;
            m_ScreenBufferHeight = height;

            Graphics::RZRHI::OnResize(width, height);

            // Destroy the resources first
            m_DepthTexture->Release(true);

            for (auto frameBuf: m_Framebuffers)
                frameBuf->Destroy();

            m_RenderPass->Destroy();

            m_Pipeline->Destroy();

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
#endif