// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLRenderPass.h"

#include <glad/glad.h>

namespace Razix {
    namespace Graphics {

        OpenGLRenderPass::OpenGLRenderPass(const RenderPassInfo& renderPassInfo)
        {
            m_AttachmentsCount = renderPassInfo.attachmentCount;
            m_AttachmentTypes  = renderPassInfo.attachmentInfos;
            shouldClear        = renderPassInfo.attachmentInfos[0].clear;
        }

        OpenGLRenderPass::~OpenGLRenderPass()
        {
        }

        void OpenGLRenderPass::BeginRenderPass(RZDrawCommandBufferHandle cmdBuffer, glm::vec4 clearColor, RZFramebuffer* framebuffer, SubPassContents subpass, u32 width, u32 height)
        {
            // Clear the necessary buffer COLOR, DEpth and stencil as needed
            glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            if (shouldClear)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        void OpenGLRenderPass::EndRenderPass(RZDrawCommandBufferHandle cmdBuffer)
        {
        }

        void OpenGLRenderPass::AddAttachment()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void OpenGLRenderPass::Destroy()
        {
        }

    }    // namespace Graphics
}    // namespace Razix