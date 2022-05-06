// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLSwapchain.h"

#ifdef RAZIX_RENDER_API_OPENGL

    #include "Razix/Platform/API/OpenGL/OpenGLContext.h"

    #include <glfw/glfw3.h>

namespace Razix {
    namespace Graphics {

        OpenGLSwapchain::OpenGLSwapchain(uint32_t width, uint32_t height)
            : m_Width(width), m_Height(height) {}

        void OpenGLSwapchain::Flip()
        {
            glfwSwapBuffers(OpenGLContext::Get()->getGLFWWindow());
        }
    }    // namespace Graphics
}    // namespace Razix
#endif