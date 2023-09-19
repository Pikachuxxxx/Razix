// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLIndexBuffer.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

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

        OpenGLIndexBuffer::OpenGLIndexBuffer(u32* data, u32 count, BufferUsage bufferUsage)
            : m_IBO(0)
        {
            m_Size       = count * sizeof(u16);
            m_Usage      = bufferUsage;
            m_IndexCount = count;
            GL_CALL(glGenBuffers(1, &m_IBO));
            GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO));
            GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), data, BufferUsageToOpenGL(m_Usage)));
        }

        OpenGLIndexBuffer::~OpenGLIndexBuffer()
        {
            GL_CALL(glDeleteBuffers(1, &m_IBO));
        }

        void OpenGLIndexBuffer::Bind(RZCommandBuffer* commandBuffer /*= nullptr*/)
        {
            GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO));
        }

        void OpenGLIndexBuffer::Unbind()
        {
            GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        }

        void OpenGLIndexBuffer::Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void OpenGLIndexBuffer::Map(u32 size /*= 0*/, u32 offset /*= 0*/)
        {
            Bind(nullptr);

            m_Mapped = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
            RAZIX_CORE_ASSERT(m_Mapped != nullptr, "[OPENGL] cannot map buffer")
        }

        void OpenGLIndexBuffer::UnMap()
        {
            Bind(nullptr);

            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }

        void* OpenGLIndexBuffer::GetMappedBuffer()
        {
            return m_Mapped;
        }

        void OpenGLIndexBuffer::Flush()
        {
        }

    }    // namespace Graphics
}    // namespace Razix