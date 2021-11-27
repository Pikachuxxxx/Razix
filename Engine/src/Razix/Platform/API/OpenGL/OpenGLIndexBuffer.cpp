#include "rzxpch.h"
#include "OpenGLIndexBuffer.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

namespace Razix {
    namespace Graphics {

        /* Converts the Buffer usage enum to OpenGL */
        static uint32_t BufferUsageToOpenGL(const BufferUsage usage)
        {
            switch (usage) {
                case BufferUsage::STATIC:
                    return GL_STATIC_DRAW;
                case BufferUsage::DYNAMIC:
                    return GL_DYNAMIC_DRAW;
                case BufferUsage::STREAM:
                    return GL_STREAM_DRAW;
            }
            return 0;
        }

        OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* data, uint32_t count, BufferUsage bufferUsage)
            : m_IBO(0)
        {
            m_Size = count * sizeof(uint32_t);
            m_Usage = bufferUsage;
            m_IndexCount = count;
            GLCall(glGenBuffers(1, &m_IBO));
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO));
            GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, BufferUsageToOpenGL(m_Usage)));
        }

        OpenGLIndexBuffer::~OpenGLIndexBuffer()
        {
            GLCall(glDeleteBuffers(1, &m_IBO));
        }

        void OpenGLIndexBuffer::Bind(RZCommandBuffer* commandBuffer /*= nullptr*/)
        {
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO));
        }

        void OpenGLIndexBuffer::Unbind()
        {
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        }
    }
}