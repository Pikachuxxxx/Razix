// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLCommandBuffer.h"

#ifdef RAZIX_RENDER_API_OPENGL

    #include <glad/glad.h>

namespace Razix {
    namespace Graphics {

        // TODO: Add GLCall guards for all opengl function calls
        OpenGLCommandBuffer::OpenGLCommandBuffer() {}

        OpenGLCommandBuffer::~OpenGLCommandBuffer() {}

        void OpenGLCommandBuffer::Init() {}

        void OpenGLCommandBuffer::BeginRecording() {}

        void OpenGLCommandBuffer::EndRecording() {}

        void OpenGLCommandBuffer::Execute() {}

        void OpenGLCommandBuffer::Reset() {}

        void OpenGLCommandBuffer::UpdateViewport(uint32_t width, uint32_t height)
        {
            glViewport(0, 0, width, height);
        }
        /*

        void OpenGLCommandBuffer::Draw(uint32_t verticexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
        {
            glDrawArrays(GL_TRIANGLES, firstVertex, verticexCount);
        }

        void OpenGLCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
        {
            //TODO: How to send index data here?
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_BYTE, nullptr);
        }
        */
    }    // namespace Graphics
}    // namespace Razix
#endif