// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLVertexBuffer.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

#include <glad/glad.h>

namespace Razix {
    namespace Graphics {

        /* Converts the Buffer usage enum to OpenGL */
        static u32 BufferUsageToOpenGL(const BufferUsage usage)
        {
            switch (usage) {
                case BufferUsage::Static:
                    return GL_STATIC_DRAW;
                case BufferUsage::Dynamic:
                    return GL_DYNAMIC_DRAW;
                case BufferUsage::Stream:
                    return GL_STREAM_DRAW;
            }
            return 0;
        }

        OpenGLVertexBuffer::OpenGLVertexBuffer(u32 size, const void* data, BufferUsage usage)
        {
            m_Usage = usage;
            m_Size  = 0;
            glGenVertexArrays(1, &m_VAO);
            glBindVertexArray(m_VAO);

            GL_CALL(glGenBuffers(1, &m_VBO));
            GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
            GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, BufferUsageToOpenGL(m_Usage)));
        }

        OpenGLVertexBuffer::~OpenGLVertexBuffer()
        {
            GL_CALL(glDeleteBuffers(1, &m_VBO));
        }

        void OpenGLVertexBuffer::Bind(RZCommandBuffer* cmdBuffer)
        {
            // Bind the VAO here later
            glBindVertexArray(m_VAO);
            GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
        }

        void OpenGLVertexBuffer::Unbind()
        {
            glBindVertexArray(0);
            GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        }

        void OpenGLVertexBuffer::SetData(u32 size, const void* dataoffset)
        {
            m_Size = size;
            GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
            GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, dataoffset, GL_STATIC_DRAW));
            //RAZIX_UNIMPLEMENTED_METHOD
        }

        void OpenGLVertexBuffer::Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void OpenGLVertexBuffer::AddBufferLayout(RZVertexBufferLayout& layout)
        {
            Bind(nullptr);

            auto& elements = layout.getElements();

            u32 offset = 0;
            for (u32 i = 0; i < elements.size(); ++i) {
                BufferLayoutElement& element = elements[i];
                glEnableVertexAttribArray(i);
                //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_f32), nullptr);

                glVertexAttribPointer(i, element.getCount(), OpenGLUtilities::BufferFormatToGLType(element.format), element.normalized, layout.getStride(), (const void*) (sz) element.offset);
                offset += element.offset;
            }
        }

        void OpenGLVertexBuffer::Map(u32 size /*= 0*/, u32 offset /*= 0*/)
        {
            Bind(nullptr);

            m_Mapped = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
            RAZIX_CORE_ASSERT(m_Mapped != nullptr, "[OPENGL] cannot map buffer")
        }

        void OpenGLVertexBuffer::UnMap()
        {
            Bind(nullptr);

            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void* OpenGLVertexBuffer::GetMappedBuffer()
        {
            return m_Mapped;
        }

        void OpenGLVertexBuffer::Flush()
        {
        }
    }    // namespace Graphics
}    // namespace Razix