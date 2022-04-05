#include "rzxpch.h"
#include "VKUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        VKUniformBuffer::VKUniformBuffer(uint32_t size, const void* data, const std::string& name)
        {
            // Create the uniform buffer
            setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            setSize(size);
            Init(data, name);
        }

        void VKUniformBuffer::Init(const void* data, const std::string& name)
        {
            VKBuffer::init(data, name);
        }

        void VKUniformBuffer::SetData(uint32_t size, const void* data)
        {
           /* VKBuffer::map();
            memcpy(m_Mapped, data, static_cast<size_t>(m_BufferSize));
            VKBuffer::unMap();*/
            VKBuffer::setData(size, data);
        }

        void VKUniformBuffer::Destroy()
        {
            VKBuffer::destroy();
        }
    }
}