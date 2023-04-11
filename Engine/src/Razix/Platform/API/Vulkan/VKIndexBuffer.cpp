// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        VKIndexBuffer::VKIndexBuffer(u32* data, u32 count, BufferUsage bufferUsage RZ_DEBUG_NAME_TAG_E_ARG)
            : VKBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, count * sizeof(u32), data RZ_DEBUG_E_ARG_NAME)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Size       = count * sizeof(u32);
            m_Usage      = bufferUsage;
            m_IndexCount = count;
        }

        VKIndexBuffer::~VKIndexBuffer()
        {
        }

        void VKIndexBuffer::Bind(RZCommandBuffer* commandBuffer /*= nullptr*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            vkCmdBindIndexBuffer(static_cast<VKCommandBuffer*>(commandBuffer)->getBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT32);
        }

        void VKIndexBuffer::Unbind() {}

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

        void VKIndexBuffer::Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            Destroy();
            VKBuffer::resize(size, data RZ_DEBUG_NAME_TAG_STR_E_ARG("Add a name here stupid"));
        }

        void VKIndexBuffer::Map(u32 size /*= 0*/, u32 offset /*= 0*/)
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
    }    // namespace Graphics
}    // namespace Razix