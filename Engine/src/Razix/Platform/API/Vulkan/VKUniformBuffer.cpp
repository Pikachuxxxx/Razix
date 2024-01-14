// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        VKUniformBuffer::VKUniformBuffer(const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Desc = desc;

            // Create the uniform buffer
            setUsageFlags(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            setSize(desc.size);
            VKBuffer::init(desc.data RZ_DEBUG_E_ARG_NAME);
        }

        void VKUniformBuffer::SetData(u32 size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::setData(size, data);
        }

        void VKUniformBuffer::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VKBuffer::destroy();
        }

        void VKUniformBuffer::DestroyResource()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix