#include "rzxpch.h"
#include "VKIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        VKIndexBuffer::VKIndexBuffer(uint16_t* data, uint32_t count, BufferUsage bufferUsage, const std::string& name)
            : VKBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, count * sizeof(uint32_t), data, name)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Size = count * sizeof(uint16_t);
            m_Usage = bufferUsage;
            m_IndexCount = count;
        }

        VKIndexBuffer::~VKIndexBuffer()
        {
            
        }

        void VKIndexBuffer::Bind(RZCommandBuffer* commandBuffer /*= nullptr*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            vkCmdBindIndexBuffer(static_cast<VKCommandBuffer*>(commandBuffer)->getBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT16);
        }

        void VKIndexBuffer::Unbind() { }

        void VKIndexBuffer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_IsBufferMapped) {
                VKBuffer::flush(m_Size);
                VKBuffer::unMap();
                m_IsBufferMapped = false;
            }
            VKBuffer::destroy();
        }

        void VKIndexBuffer::Resize(uint32_t size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            Destroy();
            VKBuffer::resize(size, data);
        }

        void VKIndexBuffer::Map(uint32_t size /*= 0*/, uint32_t offset /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::map(size == 0 ? VK_WHOLE_SIZE : size, offset);
        }

        void VKIndexBuffer::UnMap()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::unMap();
        }

        void* VKIndexBuffer::GetMappedBuffer()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return VKBuffer::getMappedRegion();
        }

        void VKIndexBuffer::Flush()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::flush();
        }
    }
}