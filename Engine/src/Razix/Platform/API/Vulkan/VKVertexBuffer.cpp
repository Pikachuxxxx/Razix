// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKVertexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKDrawCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        VKVertexBuffer::VKVertexBuffer(u32 size, const void* data, BufferUsage usage RZ_DEBUG_NAME_TAG_E_ARG)
            : VKBuffer(usage, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, data RZ_DEBUG_E_ARG_NAME)
        {
        }

        void VKVertexBuffer::Bind(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VkDeviceSize offsets[1]        = {0};
            auto         cmdBufferResource = RZResourceManager::Get().getDrawCommandBuffer(cmdBuffer);
            vkCmdBindVertexBuffers(static_cast<VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer(), 0, 1, &m_Buffer, offsets);
        }

        void VKVertexBuffer::SetData(u32 size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::setData(size, data);
        }

        void VKVertexBuffer::Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            Destroy();
            VKBuffer::resize(size, data RZ_DEBUG_E_ARG_NAME);
        }

        void VKVertexBuffer::Destroy()
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

        void VKVertexBuffer::Map(u32 size, u32 offset)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_IsBufferMapped = true;
            VKBuffer::map(size == 0 ? VK_WHOLE_SIZE : size, offset);
        }

        void VKVertexBuffer::UnMap()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_IsBufferMapped = false;
            VKBuffer::unMap();
        }

        void* VKVertexBuffer::GetMappedBuffer()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return VKBuffer::getMappedRegion();
        }

        void VKVertexBuffer::Flush()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::flush(m_BufferSize);
        }

        void VKVertexBuffer::Invalidate()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::invalidate(m_BufferSize);
        }
    }    // namespace Graphics
}    // namespace Razix
