// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKFence.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Gfx {

        VKFence::VKFence(bool isSignalled /*= true*/)
            : m_IsSignaled(isSignalled)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags             = isSignalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

            VK_CHECK_RESULT(vkCreateFence(VKDevice::Get().getDevice(), &fenceCreateInfo, nullptr, &m_Fence));

            VK_TAG_OBJECT("Fence", VK_OBJECT_TYPE_FENCE, (uint64_t) m_Fence);
        }

        VKFence::~VKFence()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            vkDestroyFence(VKDevice::Get().getDevice(), m_Fence, nullptr);
        }

        bool VKFence::isSignaled()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            if (m_IsSignaled)
                return true;
            else
                return checkState();
        }

        bool VKFence::wait()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            // Waits until the fence is signaled
            VK_CHECK_RESULT(vkWaitForFences(VKDevice::Get().getDevice(), 1, &m_Fence, true, UINT64_MAX));
            m_IsSignaled = true;

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
    }    // namespace Gfx
}    // namespace Razix