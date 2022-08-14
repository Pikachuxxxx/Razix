// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGridRenderer.h"

#include "Razix/Core/RZApplication.h"

#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZFramebuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZRenderPass.h"
#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZSwapchain.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZGridRenderer::Init()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererName = "Grid Renderer";

            // Init the width and height of RT
            m_ScreenBufferWidth  = RZApplication::Get().getWindow()->getWidth();
            m_ScreenBufferHeight = RZApplication::Get().getWindow()->getHeight();

            // Create the mesh that will be rendered which is a plane
            m_Plane = MeshFactory::CreatePlane(2000.0f, 2000.0f);

            // Load the grid shader
            m_Shader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/grid.rzsf");

            // TODO: Use a set of 2 UBOs and DescSets (per frame in flight)

            // Create the uniform buffers
            // 1. Create the View Projection UBOs
            m_ViewProjectionSystemUBO = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionSystemUBOData), &m_ViewProjSystemUBOData, "System_ViewProjUBO");

            // 2. Create the Grid related UBO and data
            m_GridUBO = Graphics::RZUniformBuffer::Create(sizeof(GridUBOData), &m_GridUBOData, "Grid Data");

            // Now create the descriptor sets for this and assign the UBOs for it
            // get the descriptor infos to create the descriptor sets
            auto setInfos = m_Shader->getSetsCreateInfos();
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
        }

        void RZGridRenderer::Begin()
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
            m_ViewProjSystemUBOData.view       = m_Camera->getViewMatrix();
            m_ViewProjSystemUBOData.projection = m_Camera->getProjection();
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
            m_Plane->getVertexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
            m_Plane->getIndexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

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
            m_RenderPass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
        }

        void RZGridRenderer::Present()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
        }

        void RZGridRenderer::Resize(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy the resources first
            m_DepthTexture->Release(true);

            for (auto frameBuf: m_Framebuffers)
                frameBuf->Destroy();

            m_RenderPass->Destroy();

            m_Pipeline->Destroy();

            Graphics::RZAPIRenderer::OnResize(width, height);

            InitDisposableResources();
        }

        void RZGridRenderer::OnEvent(RZEvent& event)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZGridRenderer::InitDisposableResources()
        {
            // Render pass
            Graphics::AttachmentInfo textureTypes[2] = {
                {Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN},
                {Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH}};

            Graphics::RenderPassInfo renderPassInfo{};
            renderPassInfo.attachmentCount = 2;
            renderPassInfo.textureType     = textureTypes;
            renderPassInfo.name            = "Grid rendering";
            renderPassInfo.clear           = true;    // Since this the first in the render stack it needs a new clean SC image to render to

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
    }    // namespace Graphics
}    // namespace Razix