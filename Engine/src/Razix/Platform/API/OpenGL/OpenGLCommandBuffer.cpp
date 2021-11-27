#include "rzxpch.h"
#include "OpenGLCommandBuffer.h"

#ifdef RAZIX_RENDER_API_OPENGL

#include <glad/glad.h>

namespace Razix {
    namespace Graphics {

        OpenGLCommandBuffer::OpenGLCommandBuffer() { }

        OpenGLCommandBuffer::~OpenGLCommandBuffer() { }

        void OpenGLCommandBuffer::Init() { }

        void OpenGLCommandBuffer::BeginRecording() { }

        void OpenGLCommandBuffer::EndRecording() { }

        void OpenGLCommandBuffer::Execute() { }

        void OpenGLCommandBuffer::Reset() { }

        void OpenGLCommandBuffer::UpdateViewport(uint32_t width, uint32_t height)
        {
            glViewport(0, 0, width, height);
        }
    }
}
#endif