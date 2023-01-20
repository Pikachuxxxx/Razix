#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"

#include <glm/glm.hpp>

namespace Razix {
    namespace Graphics {

        /* Forward Declarations */
        class RZRenderPass;

        /* The necessary information necessary to create the framebuffer to hold the frame info and render pass that will be done on the framebuffer swap images that will be renderer */
        // TODO: Replace the pointers with  rzstl::SharedRef(s)
        struct FramebufferInfo
        {
            uint32_t                width;            /* The initial width with which the framebuffer will be constructed with                */
            uint32_t                height;           /* The initial height with which the framebuffer will be constructed with               */
            uint32_t                layer = 0;        /* Multiple layers of images can exist in the framebuffer and it denotes that number    */
            uint32_t                attachmentCount;  /* Total number of attachments to the framebuffer                                       */
            uint32_t                msaaLevel;        /* Multi-Sample Anti-Aliasing samples per-pixel count                                   */
            bool                    screenFBO = true; /* Is this a final screen FBO or a render target pass                                   */
            RZTexture**             attachments;      /* Pointer to all the attachments of the framebuffer                                    */
            // TODO: convert this to RenderTexture so that screenFBO boolean can be used to validate if swapchain image is passed instead; it should automatically get the swapchain image if we use screenFBO and depthTexture is created by itself
            Graphics::RZRenderPass* renderPass;       /* The render passes that will performed on the contents of the framebuffer             */
            // TODO: resolve this from the render pass that is provided
            //RZTexture::Type*        attachmentTypes;        /* The types of the attachments                                                         */
        };

        /* Framebuffer is what the swapchain images and command are executed onto to be used by the GPU to read and present from */
        class RAZIX_API RZFramebuffer : public RZRoot
        {
        public:
            RZFramebuffer() = default;
            virtual ~RZFramebuffer() {}

            /**
             * Creates a framebuffer pointer with the underlying API implementation
             * 
             * @param frameBufInfo Framebuffer information used to customize and create the framebuffer
             * 
             * @returns The pointer to the API implementation of the Framebuffer class
             */
            static RZFramebuffer* Create(const FramebufferInfo& frameBufInfo RZ_DEBUG_NAME_TAG_E_ARG);

            virtual void Bind() {}
            virtual void Unbind() {}

            virtual void Destroy() = 0;

            /* Gets the width of the framebuffer */
            inline const uint32_t& getWidht() const { return m_Width; }
            /* Gets the Height of the framebuffer */
            inline const uint32_t& getHeight() const { return m_Height; }

            /* Sets the color with which the framebuffer will be cleared with */
            void setClearColor(const glm::vec4& color) { m_ClearColor = color; }

        protected:
            uint32_t  m_Width;           /* The Width of the framebuffer                                     */
            uint32_t  m_Height;          /* The Height of the framebuffer                                    */
            uint32_t  m_AttachmentCount; /* Number of framebuffer texture attachments                        */
            glm::vec4 m_ClearColor;      /* The clear color with which the framebuffer will be cleared with  */
        };
    }    // namespace Graphics
}    // namespace Razix