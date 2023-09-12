// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLCommandBuffer.h"

#ifdef RAZIX_RENDER_API_OPENGL

    #include <glad/glad.h>

namespace Razix {
    namespace Graphics {

        // TODO: Add GLCall guards for all opengl function calls
        OpenGLCommandBuffer::OpenGLCommandBuffer()
        {
        }

        OpenGLCommandBuffer::~OpenGLCommandBuffer()
        {
        }

        void OpenGLCommandBuffer::Init(RZ_DEBUG_NAME_TAG_S_ARG)
        {
        }

        void OpenGLCommandBuffer::BeginRecording()
        {
        }

        void OpenGLCommandBuffer::EndRecording()
        {
        }

        void OpenGLCommandBuffer::Execute()
        {
        }

        void OpenGLCommandBuffer::Reset()
        {
        }
    }    // namespace Graphics
}    // namespace Razix
#endif