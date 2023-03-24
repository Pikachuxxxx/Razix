#pragma once

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        class VKCommandBuffer : public RZCommandBuffer
        {
        public:
            VKCommandBuffer();
            VKCommandBuffer(VkCommandBuffer vulkanHandle);
            ~VKCommandBuffer();

            void Init(RZ_DEBUG_NAME_TAG_S_ARG) override;
            //void Init(VkCommandPool cmdPool = VK_NULL_HANDLE NAME_TAG);
            void BeginRecording() override;
            void EndRecording() override;
            void Execute() override;
            void Reset() override;
            void UpdateViewport(u32 width, u32 height) override;

            void* getAPIBuffer() override { return (void*) &m_CommandBuffer; }

            /*
            void Draw(u32 verticexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) override;
            void DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, int32_t vertexOffset, u32 firstInstance) override;
            */
            VkCommandBuffer getBuffer() const { return m_CommandBuffer; }

        private:
            VkCommandBuffer    m_CommandBuffer; /* Handle to the Vulkan command buffer that will be executed by the GPu     */
            VkCommandPool      m_CommandPool;   /* The command pool from which the command buffer will be allocated from    */
        };
    }    // namespace Graphics
}    // namespace Razix
#endif
