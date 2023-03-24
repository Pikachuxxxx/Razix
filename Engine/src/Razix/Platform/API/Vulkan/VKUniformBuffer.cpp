// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        VKUniformBuffer::VKUniformBuffer(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Create the uniform buffer
            setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            setSize(size);
            Init(data RZ_DEBUG_E_ARG_NAME);
        }

        void VKUniformBuffer::Init(const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::init(data RZ_DEBUG_E_ARG_NAME);
        }

        void VKUniformBuffer::SetData(u32 size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            /* VKBuffer::map();
            memcpy(m_Mapped, data, static_cast<sz>(m_BufferSize));
            VKBuffer::unMap();*/
            VKBuffer::setData(size, data);
        }

        void VKUniformBuffer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::destroy();
        }
    }    // namespace Graphics
}    // namespace Razix