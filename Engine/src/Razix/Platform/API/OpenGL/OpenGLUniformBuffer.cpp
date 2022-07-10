#include "rzxpch.h"
#include "OpenGLUniformBuffer.h"


namespace Razix {
    namespace Graphics {

        OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, const void* data, const std::string& name)
        {
            
        }

        OpenGLUniformBuffer::~OpenGLUniformBuffer()
        {

        }

        void OpenGLUniformBuffer::Init(const void* data, const std::string& name)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void OpenGLUniformBuffer::SetData(uint32_t size, const void* data)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void OpenGLUniformBuffer::Destroy()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }
}