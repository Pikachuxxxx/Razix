// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKFramebuffer.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKRenderPass.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Gfx {

        VKFramebuffer::VKFramebuffer(const FramebufferInfo& frameBufInfo RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Width           = frameBufInfo.width;
            m_Height          = frameBufInfo.height;
            m_AttachmentCount = frameBufInfo.attachmentCount;

            std::vector<VkImageView> attachments;
            RenderPassAttachmentInfo*          attachmentTypes = frameBufInfo.renderPass->getAttachmentTypes();
            for (u32 i = 0; i < m_AttachmentCount; i++) {
                switch (attachmentTypes[i].type) {
                    case TextureType::Texture_2D:
                        attachments.push_back(static_cast<VKTexture*>(frameBufInfo.attachments[i])->getImageView());
                        break;
                    case TextureType::Texture_Depth:
                        attachments.push_back(static_cast<VKTexture*>(frameBufInfo.attachments[i])->getImageView());
                        break;
                    default:
                        RAZIX_UNIMPLEMENTED_METHOD_MARK
                        break;
                }
            }

            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.pNext                   = nullptr;
            framebufferCreateInfo.flags                   = 0;
            framebufferCreateInfo.attachmentCount         = m_AttachmentCount;
            framebufferCreateInfo.pAttachments            = attachments.data();
            framebufferCreateInfo.width                   = m_Width;
            framebufferCreateInfo.height                  = m_Height;
            framebufferCreateInfo.layers                  = 1;
            framebufferCreateInfo.renderPass              = static_cast<VKRenderPass*>(frameBufInfo.renderPass)->getVKRenderPass();

            if (VK_CHECK_RESULT(vkCreateFramebuffer(VKDevice::Get().getDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffer)))
                RAZIX_CORE_ERROR("[Vulkan] cannot create framebuffer!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Successfully created framebuffer!");

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_FRAMEBUFFER, (uint64_t) m_Framebuffer);
        }

        VKFramebuffer::~VKFramebuffer() {}

        void VKFramebuffer::Destroy()
        {
            vkDestroyFramebuffer(VKDevice::Get().getDevice(), m_Framebuffer, VK_NULL_HANDLE);
        }
    }    // namespace Graphics
}    // namespace Razix