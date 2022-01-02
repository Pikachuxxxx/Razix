#include "rzxpch.h"
#include "VKFramebuffer.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKFramebuffer::VKFramebuffer(const FramebufferInfo& frameBufInfo)
        {
            m_Width             = frameBufInfo.width;
            m_Height            = frameBufInfo.height;
            m_AttachmentCount   = frameBufInfo.attachmentCount;

            std::vector<VkImageView> attachments;
            for (uint32_t i = 0; i < m_AttachmentCount; i++) {

                switch (frameBufInfo.attachmentTypes[i]) {
                    case RZTexture::Type::COLOR:
                        attachments.push_back(static_cast<VKTexture2D*>(frameBufInfo.attachments[i])->getImageView());
                    case RZTexture::Type::DEPTH:
                        attachments.push_back(static_cast<VKTexture2D*>(frameBufInfo.attachments[i])->getImageView());
                    default:
                        RAZIX_UNIMPLEMENTED_METHOD_MARK
                        break;
                }
            }
            
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.pNext = nullptr;
            framebufferCreateInfo.flags = 0;
            framebufferCreateInfo.attachmentCount = m_AttachmentCount;
            framebufferCreateInfo.pAttachments = attachments.data();
            framebufferCreateInfo.width = m_Width;
            framebufferCreateInfo.height = m_Height;
            framebufferCreateInfo.layers = 1;

            if (VK_CHECK_RESULT(vkCreateFramebuffer(VKDevice::Get().getDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffer)))
                RAZIX_CORE_TRACE("[Vulkan] Successfully created framebuffer!");
            else
                RAZIX_CORE_ERROR("[Vulkan] cannot create framebuffer!");
        }

        VKFramebuffer::~VKFramebuffer()
        {
            vkDestroyFramebuffer(VKDevice::Get().getDevice(), m_Framebuffer, VK_NULL_HANDLE);
        }
    }
}