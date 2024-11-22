#pragma once

#include "Razix/Gfx/RHI/API/RZCommandPool.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {

        /* Creates a Vulkan command buffer */
        class VKCommandPool final : public RZCommandPool
        {
        public:
            VKCommandPool() {}
            /* Destructor for the command pool */
            ~VKCommandPool() {}
            /**
             * Constructor for vulkan command buffer
             * 
             * @param queueIndex The Queue family index for which the pool will generate buffers for
             * @param flags Configure the pool
             */
            VKCommandPool(int queueIndex, VkCommandPoolCreateFlags flags);

            RAZIX_CLEANUP_RESOURCE

            /* Resets the pool and the buffers it allocated */
            void  Reset() override;
            void* getAPIHandle() override { return &m_CmdPool; }

            /* Returns the underlying Vulkan command pool handle */
            const VkCommandPool& getVKPool() const { return m_CmdPool; }

        private:
            VkCommandPool m_CmdPool = VK_NULL_HANDLE; /* Handle to vulkan command pool */
        };
    }    // namespace Graphics
}    // namespace Razix
#endif
