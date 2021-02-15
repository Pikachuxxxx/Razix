#include "rzxpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Razix
{

    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        RZX_CORE_ASSERT(windowHandle, "Window Handle is NULL!");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        RZX_CORE_ASSERT(status, "Cannot initialize GLAD!");

        // Log the Vendor, Renderer Device and the Version of the drivers
        const GLubyte* vendor   = glGetString(GL_VENDOR);       // Returns the vendor
        const GLubyte* renderer = glGetString(GL_RENDERER);     // Returns a hint to the model
        const GLubyte* version  = glGetString(GL_VERSION);      // Returns the version

        RZX_CORE_INFO("OpenGL Info : \n \t\t\t Vendor : {0} \n \t\t\t Renderer : {1} \n \t\t\t Version : {2} ", vendor, renderer, version);
        RZX_CORE_INFO("GLFW Version : {0}", glfwGetVersionString());
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

}
