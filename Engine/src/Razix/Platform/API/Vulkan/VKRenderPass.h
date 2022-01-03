#pragma once

#include "Razix/Graphics/API/RZRenderPass.h"

#ifdef RAZIX_RENDER_API_VULKAN

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* Vulkan implementation of the render pass */
        class VKRenderPass : public RZRenderPass
        {
        public:
            /* Creates a Vulkan render pass object with the given information */
            VKRenderPass(const RenderPassInfo& renderPassInfo);
            ~VKRenderPass();

            /* Begins the Vulkan render pass */
            void BeginRenderPass(RZCommandBuffer* commandBuffer, glm::vec4 clearColor, RZFramebuffer* framebuffer, SubPassContents subpass, uint32_t width, uint32_t height) override;
            /* Ends the Vulkan render pass on the command buffer */
            void EndRenderPass(RZCommandBuffer* commandBuffer) override;
            /* Adds any attachments to the render pass */
            void AddAttachment() override;

            bool init(const RenderPassInfo& renderpassInfo);
            VkAttachmentDescription getAttachmentDescription(AttachmentInfo info, bool clear = true);
        private:
            VkRenderPass    m_RenderPass;   /* The handle to the vulkan render pass object                              */
            VkClearValue*   m_ClearValue;   /* The clear value with which the color attachments will be cleared with    */
            bool            m_DepthOnly;    /* Should we create the render pass with only a depth attachment            */
            bool            m_ClearDepth;   /* Whether or not to clear the depth attachment                             */
        };
    }
}
#endif