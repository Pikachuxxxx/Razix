#include "rzxpch.h"
#include "VKVertexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        VKVertexBuffer::VKVertexBuffer(uint32_t size, const void* data, BufferUsage usage) 
            : VKBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, data)
        {
            m_Usage = usage;
            m_Size = 0;

            VKBuffer::setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        }

        VKVertexBuffer::~VKVertexBuffer() { }

        void VKVertexBuffer::Bind(RZCommandBuffer* cmdBuffer)
        {
            VkDeviceSize offsets[1] = {0};
            if (cmdBuffer)
                vkCmdBindVertexBuffers(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), 0, 1, &m_Buffer, offsets);
        }

        void VKVertexBuffer::Unbind()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void VKVertexBuffer::SetSubData(uint32_t size, const void* data, uint32_t offset)
        {
            VKBuffer::map(size, offset);
            VKBuffer::setData(size, data);
            VKBuffer::unMap();
        }

        void VKVertexBuffer::AddBufferLayout(RZVertexBufferLayout& layout) { }

        void VKVertexBuffer::Destroy()
        {
            if (m_IsBufferMapped) {
                VKBuffer::flush(m_Size);
                VKBuffer::unMap();
                m_IsBufferMapped = false;
            }

            VKBuffer::destroy();
        }

    }
}
