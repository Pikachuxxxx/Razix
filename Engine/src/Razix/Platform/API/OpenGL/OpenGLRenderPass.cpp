// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLRenderPass.h"

namespace Razix {
    namespace Graphics {

        OpenGLRenderPass::OpenGLRenderPass(const RenderPassInfo& renderPassInfo)
        {
            m_AttachmentsCount = renderPassInfo.attachmentCount;
            m_AttachmentTypes  = renderPassInfo.textureType;
        }

        OpenGLRenderPass::~OpenGLRenderPass()
        {
        }

        void OpenGLRenderPass::BeginRenderPass(RZCommandBuffer* commandBuffer, glm::vec4 clearColor, RZFramebuffer* framebuffer, SubPassContents subpass, uint32_t width, uint32_t height)
        {
        }

        void OpenGLRenderPass::EndRenderPass(RZCommandBuffer* commandBuffer)
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