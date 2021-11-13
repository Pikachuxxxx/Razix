#include "rzxpch.h"
#include "VKUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        VKUniformBuffer::VKUniformBuffer(uint32_t size, const void* data)
        {
            // Create the uniform buffer
            setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            setSize(size);
            Init(data);
        }

        void VKUniformBuffer::Init(const void* data)
        {
            VKBuffer::init(data);
        }

        void VKUniformBuffer::SetData(uint32_t size, const void* data)
        {
            VKBuffer::map();
            memcpy(m_Mapped, data, static_cast<size_t>(m_Size));
            VKBuffer::unMap();
        }

        void VKUniformBuffer::Destroy()
        {
            VKBuffer::destroy();
        }
    }
}