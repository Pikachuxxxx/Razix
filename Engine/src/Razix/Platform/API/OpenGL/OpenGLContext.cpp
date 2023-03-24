// clang-format off
#include "rzxpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace Razix {
    namespace Graphics {
        OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
            : m_Window(windowHandle)
        {
            RAZIX_CORE_ASSERT(windowHandle, "Window Handle is NULL!");
        }

        void OpenGLContext::Init()
        {
            glfwMakeContextCurrent(m_Window);
            int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
            RAZIX_CORE_ASSERT(status, "Cannot initialize GLAD!");

            // Log the Vendor, Renderer Device and the Version of the drivers
            const GLubyte* vendor   = glGetString(GL_VENDOR);      // Returns the vendor
            const GLubyte* renderer = glGetString(GL_RENDERER);    // Returns a hint to the model
            const GLubyte* version  = glGetString(GL_VERSION);     // Returns the version

            RAZIX_CORE_INFO("OpenGL Info : \n \t\t\t\t Vendor : {0} \n \t\t\t\t Renderer : {1} \n \t\t\t\t Version : {2} ", vendor, renderer, version);
            RAZIX_CORE_INFO("GLFW Version : {0}", glfwGetVersionString());

            m_Swapchain = new OpenGLSwapchain(0, 0);
        }

        void OpenGLContext::Destroy()
        {
            glfwTerminate();
        }

        void OpenGLContext::ClearWithColor(f32 r, f32 g, f32 b)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(r, g, b, 1.0f);
        }

    }    // namespace Graphics
}    // namespace Razix
