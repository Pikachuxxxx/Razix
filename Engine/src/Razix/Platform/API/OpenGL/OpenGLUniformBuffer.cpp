// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLUniformBuffer.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

namespace Razix {
    namespace Graphics {

        OpenGLUniformBuffer::OpenGLUniformBuffer(const RZBufferDesc& desc)
        {
            m_Desc = desc;

            GL_CALL(glGenBuffers(1, &m_UBO));
            GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UBO));
            GL_CALL(glBufferData(GL_UNIFORM_BUFFER, desc.size, desc.data, GL_DYNAMIC_DRAW));
            GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
            // now bind this buffer to the block index done using descriptors (not here)
            //GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_UBO, 0, size);
        }

        void OpenGLUniformBuffer::Bind()
        {
            GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UBO));
        }

        void OpenGLUniformBuffer::SetData(u32 size, const void* data)
        {
            GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UBO));
            GL_CALL(glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW));
            GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
        }

        void OpenGLUniformBuffer::DestroyResource()
        {
            GL_CALL(glDeleteBuffers(1, &m_UBO));
        }
    }    // namespace Graphics
}    // namespace Razix