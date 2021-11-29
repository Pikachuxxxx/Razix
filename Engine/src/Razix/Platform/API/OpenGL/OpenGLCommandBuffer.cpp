#include "rzxpch.h"
#include "OpenGLCommandBuffer.h"

#ifdef RAZIX_RENDER_API_OPENGL

#include <glad/glad.h>

namespace Razix {
    namespace Graphics {

        OpenGLCommandBuffer::OpenGLCommandBuffer() { }

        OpenGLCommandBuffer::~OpenGLCommandBuffer() { }

        void OpenGLCommandBuffer::Init() { 
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), nullptr);
        }

        void OpenGLCommandBuffer::BeginRecording() { }

        void OpenGLCommandBuffer::EndRecording() { }

        void OpenGLCommandBuffer::Execute() 
        {
            // TODO: Remove thiss!!!!!!!!
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        void OpenGLCommandBuffer::Reset() { }

        void OpenGLCommandBuffer::UpdateViewport(uint32_t width, uint32_t height)
        {
            glViewport(0, 0, width, height);
        }
    }
}
#endif