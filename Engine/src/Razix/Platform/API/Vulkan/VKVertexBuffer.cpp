// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKVertexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        VKVertexBuffer::VKVertexBuffer(u32 size, const void* data, BufferUsage usage RZ_DEBUG_NAME_TAG_E_ARG)
            : VKBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, data RZ_DEBUG_E_ARG_NAME)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Usage = usage;
            m_Size  = 0;

            VKBuffer::setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        }

        VKVertexBuffer::~VKVertexBuffer() {}

        void VKVertexBuffer::Bind(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VkDeviceSize offsets[1] = {0};
            if (cmdBuffer)
                vkCmdBindVertexBuffers(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), 0, 1, &m_Buffer, offsets);
        }

        void VKVertexBuffer::Unbind()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
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

        void VKVertexBuffer::AddBufferLayout(RZVertexBufferLayout& layout) {}

        void VKVertexBuffer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_IsBufferMapped) {
                VKBuffer::flush(m_Size);
                VKBuffer::unMap();
                m_IsBufferMapped = false;
            }

            VKBuffer::destroy();
        }

        void VKVertexBuffer::Map(u32 size, u32 offset)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::map(size == 0 ? VK_WHOLE_SIZE : size, offset);
        }

        void VKVertexBuffer::UnMap()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

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

            VKBuffer::flush();
        }
    }    // namespace Graphics
}    // namespace Razix
