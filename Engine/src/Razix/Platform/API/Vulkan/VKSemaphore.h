#pragma once

#include "Razix/Gfx/RHI/API/RZSemaphore.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {
        class VKSemaphore : public RZSemaphore
        {
        public:
            VKSemaphore(RZ_DEBUG_NAME_TAG_S_ARG);
            ~VKSemaphore() {}

            void Destroy() override;

            void* getAPIHandlePtr(u32 idx) override;

        private:
            VkSemaphore m_Handles[RAZIX_MAX_SWAP_IMAGES_COUNT];
        };
    }    // namespace Graphics
}    // namespace Razix
