// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKDrawCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        VKIndexBuffer::VKIndexBuffer(u32* data, u32 count, BufferUsage bufferUsage RZ_DEBUG_NAME_TAG_E_ARG)
            : VKBuffer(bufferUsage, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, count * sizeof(u32), (const void*) data RZ_DEBUG_E_ARG_NAME)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_IndexCount = count;
        }

        void VKIndexBuffer::Bind(RZDrawCommandBufferHandle cmdBuffer /*= nullptr*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto cmdBufferResource = RZResourceManager::Get().getDrawCommandBuffer(cmdBuffer);
            vkCmdBindIndexBuffer(static_cast<VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT32);
        }

        void VKIndexBuffer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_IsBufferMapped) {
                VKBuffer::invalidate(m_BufferSize);
                VKBuffer::flush(m_BufferSize);
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

            m_IsBufferMapped = true;
            VKBuffer::map(size == 0 ? VK_WHOLE_SIZE : size, offset);
        }

        void VKIndexBuffer::UnMap()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_IsBufferMapped = false;
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

            VKBuffer::flush(m_BufferSize);
        }

        void VKIndexBuffer::Invalidate()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::invalidate(m_BufferSize);
        }

    }    // namespace Graphics
}    // namespace Razix