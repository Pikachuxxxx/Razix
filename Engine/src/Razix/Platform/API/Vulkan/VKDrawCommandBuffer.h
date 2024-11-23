#pragma once

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Platform/API/Vulkan/VKCommandPool.h"

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {

        class VKDrawCommandBuffer final : public RZDrawCommandBuffer
        {
        public:
            VKDrawCommandBuffer(VkCommandPool pool);
            VKDrawCommandBuffer(VkCommandBuffer vulkanHandle);
            ~VKDrawCommandBuffer() {}
                
            RAZIX_CLEANUP_RESOURCE

            void Init(RZ_DEBUG_NAME_TAG_S_ARG) override;
            void BeginRecording() override;
            void EndRecording() override;
            void Execute() override;
            void Reset() override;

            void* getAPIBuffer() override { return (void*) &m_CommandBuffer; }

            VkCommandBuffer getBuffer() const { return m_CommandBuffer; }

        private:
            VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE; /* Handle to the Vulkan command buffer that will be executed by the GPu     */
            VkCommandPool   m_CommandPool   = VK_NULL_HANDLE; /* The command pool from which the command buffer will be allocated from    */
        };
    }    // namespace Graphics
}    // namespace Razix
#endif
