// clang-format off
#include "rzxpch.h"
// clang-format on

#include "OpenGLFrameBuffer.h"

#include "Razix/Platform/API/OpenGL/OpenGLRenderPass.h"

#include <glad/glad.h>

namespace Razix {
    namespace Graphics {

        OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferInfo& frameBufInfo)
            : isScreen(frameBufInfo.screenFBO)
        {
            // Generate the FBO and add attachments as specified by the renderpass Info + bind the RTs as provided
            if (frameBufInfo.screenFBO)
                return;

            glGenFramebuffers(1, &m_FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

            // Get and bind the attachments (multiple color or depth)
            for (u32 i = 0; i < frameBufInfo.attachmentCount; i++) {

                // Only bind color attachments
                if (frameBufInfo.renderPass->getAttachmentTypes()[i].type != TextureType::Texture_2D)
                    return;

                auto& attachment = frameBufInfo.attachments[i];

                attachment->Bind(i);

	            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, *(u32*)(attachment->GetAPIHandlePtr()), 0);
            }

            glGenRenderbuffers(1, &m_RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameBufInfo.width, frameBufInfo.height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Add depth attachment by default
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                RAZIX_CORE_ERROR("[OPENGL] Framebuffer is not complete!");

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }

        OpenGLFrameBuffer::~OpenGLFrameBuffer()
        {
        }

        void OpenGLFrameBuffer::Bind()
        {
            if (!isScreen)
                glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
            else
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void OpenGLFrameBuffer::Unbind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void OpenGLFrameBuffer::Destroy()
        {
            glDeleteBuffers(1, &m_FBO);
        }

    }    // namespace Graphics
}    // namespace Razix
