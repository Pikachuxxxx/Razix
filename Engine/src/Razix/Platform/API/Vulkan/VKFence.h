#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* A Vulkan fence is a object used to synchronize between the CPU and GPU */
        class VKFence
        {
        public:
            /**
             * Creates a Fence with the given signaled state
             * 
             * @param isSignalled Whether or not to create a fence in a signaled state or not
             */
            VKFence (bool isSignalled = true);
            ~VKFence ();

            /* Indicates whether the fence is in signaled or not */
            bool isSignaled ();
            /* Waits until the fence is signaled */
            bool wait ();
            /* Resets the fence into a non signaled state */
            void reset ();
            bool checkState ();
            void waitAndReset ();

            /* Gets the handle to the Vulkan fence */
            const VkFence& getVKFence () const { return m_Fence; }
            void           setSignaled (bool signaled) { m_IsSignaled = signaled; }

        private:
            VkFence m_Fence;      /* The handle to the Vulkan fence object                                        */
            bool    m_IsSignaled; /* Boolean that indicates the whether the fence is in signaled state or not     */
        };
    }    // namespace Graphics
}    // namespace Razix
#endif