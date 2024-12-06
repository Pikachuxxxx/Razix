// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLStorageBuffer.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

namespace Razix {
    namespace Graphics {

        OpenGLStorageBuffer::OpenGLStorageBuffer(u32 size, const std::string& name)
            : m_Size(size)
        {
            glGenBuffers(1, &m_SSBO);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_DRAW);    //sizeof(data) only works for statically sized C/C++ a arrays
            // now bind this buffer to the block index done using descriptors (not here)
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);    // unbind
        }

        void OpenGLStorageBuffer::Bind()
        {
            // Binding index to the buffer will be done by OpenGLDescriptorSet API
            GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_SSBO));
        }

        void* OpenGLStorageBuffer::GetData()
        {
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);    // missed previously call to memory barrier
            Bind();

            void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

            memcpy(m_Mapped, ptr, m_Size);

            glUnmapNamedBuffer(GL_SHADER_STORAGE_BUFFER);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            return m_Mapped;
        }

        void OpenGLStorageBuffer::SetData(u32 size, const void* data)
        {
            GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO));
            GL_CALL(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW));
            GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
        }

        void OpenGLStorageBuffer::Destroy()
        {
            GL_CALL(glDeleteBuffers(1, &m_SSBO));
        }

    }    // namespace Graphics
}    // namespace Razix
