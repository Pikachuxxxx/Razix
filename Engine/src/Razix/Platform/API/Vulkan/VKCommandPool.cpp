// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKCommandPool.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "VKDevice.h"
    #include "VKUtilities.h"

namespace Razix {
    namespace Gfx {

        VKCommandPool::VKCommandPool(int queueIndex, VkCommandPoolCreateFlags flags)
        {
            VkCommandPoolCreateInfo cmdPoolCI{};
            cmdPoolCI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmdPoolCI.queueFamilyIndex = queueIndex;
            cmdPoolCI.flags            = flags;

            VK_CHECK_RESULT(vkCreateCommandPool(VKDevice::Get().getDevice(), &cmdPoolCI, nullptr, &m_CmdPool));
        }

        //-------------------------------------------------------------------------------------------
        RAZIX_CLEANUP_RESOURCE_IMPL(VKCommandPool)
        {
            vkDestroyCommandPool(VKDevice::Get().getDevice(), m_CmdPool, nullptr);
        }
        //-------------------------------------------------------------------------------------------

        void VKCommandPool::Reset()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            VK_CHECK_RESULT(vkResetCommandPool(VKDevice::Get().getDevice(), m_CmdPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
        }
    }    // namespace Gfx
}    // namespace Razix
#endif