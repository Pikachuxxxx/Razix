// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        VKUniformBuffer::VKUniformBuffer(const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
            : VKBuffer(desc.usage, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, desc.size, desc.data RZ_DEBUG_E_ARG_NAME)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Desc = desc;
        }

        void VKUniformBuffer::SetData(u32 size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::setData(size, data);
        }

        void VKUniformBuffer::DestroyResource()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::destroy();
        }

        void VKUniformBuffer::Flush()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::flush(m_BufferSize);
        }

        void VKUniformBuffer::Invalidate()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::invalidate(m_BufferSize);
        }

    }    // namespace Graphics
}    // namespace Razix