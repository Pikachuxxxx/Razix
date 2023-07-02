// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKFence.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKFence::VKFence(bool isSignalled /*= true*/)
            : m_IsSignaled(isSignalled)
        {
            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags             = isSignalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

            VK_CHECK_RESULT(vkCreateFence(VKDevice::Get().getDevice(), &fenceCreateInfo, nullptr, &m_Fence));

            VK_TAG_OBJECT("Fence", VK_OBJECT_TYPE_FENCE, (uint64_t) m_Fence);
        }

        VKFence::~VKFence()
        {
            vkDestroyFence(VKDevice::Get().getDevice(), m_Fence, nullptr);
        }

        bool VKFence::isSignaled()
        {
            if (m_IsSignaled)
                return true;
            else
                return checkState();
        }

        bool VKFence::wait()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            //RAZIX_CORE_ASSERT(!m_IsSignaled, "[Vulkan] Fence is Signaled!");

            // Waits until the fence is signaled
            const VkResult result = vkWaitForFences(VKDevice::Get().getDevice(), 1, &m_Fence, true, UINT64_MAX);

#if 0
                        // Waits until the fence is signaled
            VkResult result;    // = vkWaitForFences(VKDevice::Get().getDevice(), 1, &m_Fence, true, UINT32_MAX);

            result = vkGetFenceStatus(VKDevice::Get().getDevice(), m_Fence);

            if (result != VK_SUCCESS) {
                result = vkWaitForFences(VKDevice::Get().getDevice(), 1, &m_Fence, VK_TRUE, UINT64_MAX);
            }
#endif

            VK_CHECK_RESULT(result);
            if (result == VK_SUCCESS) {
                m_IsSignaled = true;
                return false;
            }
            return true;
        }

        void VKFence::reset()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            if (m_IsSignaled)
                VK_CHECK_RESULT(vkResetFences(VKDevice::Get().getDevice(), 1, &m_Fence));

            m_IsSignaled = false;
        }

        bool VKFence::checkState()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            RAZIX_CORE_ASSERT(!m_IsSignaled, "[Vulkan] Fence is signalled!");

            // Waits until the fence is signaled
            const VkResult result = vkWaitForFences(VKDevice::Get().getDevice(), 1, &m_Fence, true, UINT32_MAX);

            VK_CHECK_RESULT(result);
            if (result == VK_SUCCESS) {
                m_IsSignaled = true;
                return false;
            }
            return true;
        }

        void VKFence::waitAndReset()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            if (!isSignaled())
                wait();

            reset();
        }
    }    // namespace Graphics
}    // namespace Razix