// clang-format off
#include "rzxpch.h"
// clang-format on
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
            m_Size  = 0;
            glGenVertexArrays(1, &m_VAO);
            glBindVertexArray(m_VAO);

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
            glBindVertexArray(m_VAO);
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
        }

        void OpenGLVertexBuffer::Unbind()
        {
            glBindVertexArray(0);
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        }

        void OpenGLVertexBuffer::SetData(uint32_t size, const void* dataoffset)
        {
            m_Size = size;
            GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
            //GLCall(glBufferData(GL_ARRAY_BUFFER, size, data));
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void OpenGLVertexBuffer::Resize(uint32_t size, const void* data)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void OpenGLVertexBuffer::AddBufferLayout(RZVertexBufferLayout& layout)
        {
            Bind(nullptr);

            auto&    elements = layout.getElements();
            uint32_t offset   = 0;
            for (uint32_t i = 0; i < elements.size(); ++i) {
                BufferLayoutElement& element = elements[i];
                glEnableVertexAttribArray(i);
                //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), nullptr);

                glVertexAttribPointer(i, element.getCount(), OpenGLUtilities::BufferFormatToGLType(element.format), element.normalized, layout.getStride(), (const void*) (size_t) element.offset);
                offset += element.offset;
            }
        }

        void OpenGLVertexBuffer::Map(uint32_t size /*= 0*/, uint32_t offset /*= 0*/)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void OpenGLVertexBuffer::UnMap()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void* OpenGLVertexBuffer::GetMappedBuffer()
        {
            return nullptr;
        }

        void OpenGLVertexBuffer::Flush()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }
    }    // namespace Graphics
}    // namespace Razix