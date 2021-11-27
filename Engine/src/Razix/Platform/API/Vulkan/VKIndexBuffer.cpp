#include "rzxpch.h"
#include "VKIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        VKIndexBuffer::VKIndexBuffer(uint32_t* data, uint32_t count, BufferUsage bufferUsage)
            : VKBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, count * sizeof(uint16_t), data)
        {
            m_Size = count * sizeof(uint32_t);
            m_Usage = bufferUsage;
            m_IndexCount = count;
        }

        VKIndexBuffer::~VKIndexBuffer()
        {
            if (m_IsBufferMapped) {
                VKBuffer::flush(m_Size);
                VKBuffer::unMap();
                m_IsBufferMapped = false;
            }
        }

        void VKIndexBuffer::Bind(RZCommandBuffer* commandBuffer /*= nullptr*/)
        {
            vkCmdBindIndexBuffer(static_cast<VKCommandBuffer*>(commandBuffer)->getVKCmdBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT32);
        }

        void VKIndexBuffer::Unbind() { }
    }
}