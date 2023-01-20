#pragma once

#include "Razix/Graphics/RHI/API/RZSemaphore.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {
        class VKSemaphore : public RZSemaphore
        {
        public:
            VKSemaphore(RZ_DEBUG_NAME_TAG_S_ARG);
            ~VKSemaphore() {}

            void Destroy() override;

            void* getHandle(uint32_t idx) override;

        private:
            VkSemaphore m_Handles[RAZIX_MAX_SWAP_IMAGES_COUNT];
        };
    }    // namespace Graphics
}    // namespace Razix
