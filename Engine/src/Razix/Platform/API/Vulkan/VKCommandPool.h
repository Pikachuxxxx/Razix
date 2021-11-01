#pragma once

#ifdef RAZIX_RENDER_API_VULKAN 

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {
        
        class VKCommandPool
        {
        public:
            VKCommandPool(int queueIndex, VkCommandPoolCreateFlags flags);
            ~VKCommandPool();

            void reset();

            const VkCommandPool& getVKPool() const { return m_Handle; }

        private:
            VkCommandPool m_Handle;
        };
    }
}
#endif
