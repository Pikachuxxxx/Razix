#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* Creates a Vulkan command buffer */
        class VKCommandPool
        {
        public:
            /**
             * Constructor for vulkan command buffer
             * 
             * @param queueIndex The Queue family index for which the pool will generate buffers for
             * @param flags Configure the pool
             */
            VKCommandPool(int queueIndex, VkCommandPoolCreateFlags flags);
            /* Destructor for the command pool */
            ~VKCommandPool() {}

            /* Resets the pool and the buffers it allocated */
            void reset();
            /* Destroys the command pool */
            void destroy();

            /* Returns the underlying Vulkan command pool handle */
            const VkCommandPool& getVKPool() const { return m_CmdPool; }

        private:
            VkCommandPool m_CmdPool; /* Handle to vulkan command pool */
        };
    }    // namespace Graphics
}    // namespace Razix
#endif
