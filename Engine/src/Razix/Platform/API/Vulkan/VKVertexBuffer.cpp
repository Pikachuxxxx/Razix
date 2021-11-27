#include "rzxpch.h"
#include "VKVertexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Graphics {

        VKVertexBuffer::VKVertexBuffer(const BufferUsage& usage) : VKBuffer()
        {
            m_Usage = usage;
            m_Size = 0;

            VKBuffer::setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        }

        VKVertexBuffer::~VKVertexBuffer()
        {
            if (m_IsBufferMapped) {
                VKBuffer::flush(m_Size);
                VKBuffer::unMap();
                m_IsBufferMapped = false;
            }
        }

        void VKVertexBuffer::Bind(const RZCommandBuffer* cmdBuffer)
        {
            VkDeviceSize offsets[1] = { 0 };
            if (cmdBuffer)
                vkCmdBindVertexBuffers(static_cast<VKCommandBuffer*>(commandBuffer)->GetHandle(), 0, 1, &m_Buffer, offsets);
        }

        void VKVertexBuffer::Unbind()
        {

        }

        void VKVertexBuffer::SetData(uint32_t size, const void* data)
        {

        }

        void VKVertexBuffer::SetSubData(uint32_t size, const void* data, uint32_t offset)
        {

        }

    }
}
