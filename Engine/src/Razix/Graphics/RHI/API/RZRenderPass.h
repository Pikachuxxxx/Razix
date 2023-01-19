#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"

namespace Razix {
    namespace Graphics {

        /* Forward decelerations to reduce header file include complexity */
        class RZCommandBuffer;
        class RZFramebuffer;

        /* Gives information about the type of texture that is attached to the framebuffer render pass */
        struct RAZIX_MEM_ALIGN RenderPassAttachmentInfo
        {
            RZTexture::Type   type;         /* The type of the attachment                                                                                               */
            RZTexture::Format format;       /* The format of the attachment                                                                                             */
            bool              clear = true; /* Whether or not to clear the particular attachment                                                                        */
            glm::vec4         clearColor;   /* Clear color with which the attachment is cleared, Note: x and y represent the depth clear values if clear is false       */
        };

        /* Info to create a render pass */
        struct RAZIX_MEM_ALIGN RenderPassInfo
        {
            std::string     name;            /* Name of the renderpass                               */
            RenderPassAttachmentInfo* attachmentInfos; /* Attachments and their info                           */
            uint32_t        attachmentCount; /* The number of attachments in the current render pass */
            uint8_t         _padding[3];     /* manual Padding for 16-byte alignment fill up         */
        };

        enum SubPassContents
        {
            INLINE,
            SECONDARY
        };

        /* Render Pass describes how the framebuffer is interpreted, provides multiple passes for transformations and attachments and helps with post-processing */
        class RAZIX_MEM_ALIGN RAZIX_API RZRenderPass : public RZRoot
        {
        public:
            RZRenderPass() = default;
            virtual ~RZRenderPass() {}

            /**
             * Creates a render pass with the underlying API to manipulate the framebuffer contents and do post processing
             * 
             * @param renderPassInfo Information on how to create the render poss
             * 
             * @returns pointer to the underlying API implementation of the razix render pass
             */
            static RZRenderPass* Create(const RenderPassInfo& renderPassInfo RZ_DEBUG_NAME_TAG_E_ARG);

            /**
             *  Begins the render pass
             * 
             * @param commandBuffer The command buffer onto which the render pass will be executed onto
             * @param clearColor The color with which the attachment will be cleared with
             * @param subPass Additional secondary passed along with the primary pass
             * @param width The width of the attachment
             * @param height The height of the attachment
             */
            virtual void BeginRenderPass(RZCommandBuffer* commandBuffer, glm::vec4 clearColor, RZFramebuffer* framebuffer, SubPassContents subpass, uint32_t width, uint32_t height) = 0;
            /* Ends the render pass */
            virtual void EndRenderPass(RZCommandBuffer* commandBuffer) = 0;
            // TODO: Implement this
            /* Adds another attachment to the render pass for additional pass read/writes */
            virtual void AddAttachment() = 0;

            virtual void Destroy() = 0;

            /* Get the total number of color attachments with the given render pass */
            inline uint32_t getColorAttachmentsCount() const { return m_ColorAttachmentsCount; }
            /* Gets the total number of attachments (color/depths/other) to the given render pass */
            inline uint32_t getAttachmentsCount() const { return m_AttachmentsCount; }
            /* Gets the attachments types info */
            RAZIX_INLINE RenderPassAttachmentInfo* getAttachmentTypes() { return m_AttachmentTypes; }

        protected:
            uint32_t        m_AttachmentsCount      = 0;       /* The total number of attachments bounded to the render pass                       */
            uint32_t        m_ColorAttachmentsCount = 0;       /* The total number of color attachments bounded to the render pass                 */
            RenderPassAttachmentInfo* m_AttachmentTypes       = nullptr; /* Types of attachments for the framebuffer that will be used by the render pass    */
        };
    }    // namespace Graphics
}    // namespace Razix
