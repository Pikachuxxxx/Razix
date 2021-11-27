#include "rzxpch.h"
#include "OpenGLVertexBuffer.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

#include <glad/glad.h>

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

        OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, const void* data, BufferUsage usage)
        {
            m_Usage = usage;
            m_Size = 0;
            GLCall(glGenBuffers(1, &m_VBO));
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
            GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, BufferUsageToOpenGL(m_Usage)));
        }

        OpenGLVertexBuffer::~OpenGLVertexBuffer()
        {
            GLCall(glDeleteBuffers(1, &m_VBO));
        }

        void OpenGLVertexBuffer::Bind(RZCommandBuffer* cmdBuffer)
        {
            // Bind the VAO here later
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
        }

        void OpenGLVertexBuffer::Unbind()
        {
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        }

        void OpenGLVertexBuffer::SetSubData(uint32_t size, const void* data, uint32_t offset)
        {
            m_Size = size;
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
            GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
        }

    }
}