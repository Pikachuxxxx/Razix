// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKSemaphore.h"

#include "razix/Platform/API/Vulkan/VKDevice.h"
#include "razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKSemaphore::VKSemaphore(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreInfo.pNext                 = nullptr;

            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; ++i) {
                VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().getDevice(), &semaphoreInfo, nullptr, &m_Handles[i]));

                // Tag em
                VK_TAG_OBJECT(RZ_DEBUG_S_ARG_NAME, VK_OBJECT_TYPE_SEMAPHORE, (uint64_t) m_Handles[i])
            }
        }

        void VKSemaphore::Destroy()
        {
            // Wait for completion before deleting them
            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; ++i)
                vkDestroySemaphore(VKDevice::Get().getDevice(), m_Handles[i], nullptr);
        }

        void* VKSemaphore::getAPIHandlePtr(u32 idx)
        {
            return &m_Handles[idx];
        }
    }    // namespace Graphics
}    // namespace Razix