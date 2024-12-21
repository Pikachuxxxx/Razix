#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Gfx/RHI/API/RZSampler.h"

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {

        class VKSampler final : public RZSampler
        {
        public:
            VKSampler(const RZSamplerDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            inline VkSampler getSampler() const { return m_ImageSampler; }

        private:
            VkSampler m_ImageSampler = VK_NULL_HANDLE;
            // TODO: add more props like LODs, wrap/filter mode etc.

        private:
            void createSamplerHandle();
        };
    }    // namespace Gfx
}    // namespace Razix
#endif
