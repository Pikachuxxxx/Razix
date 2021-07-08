#include "rzxpch.h"
#include "GLFWWindow.h"

#include "Razix/Embedded/RazixLogo32.inl"
#include "Razix/Embedded/RazixLogo64.inl"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/KeyEvent.h"
#include "Razix/Events/MouseEvent.h"
#include "Razix/Utilities/LoadImage.h"

#include <GLFW/glfw3.h>
//TODO: Remove this, the context handle using Refs and Init
#ifdef RAZIX_RENDER_API_OPENGL
#include "Platform/OpenGL/OpenGLContext.h"
#endif // RAZIX_RENDER_API_OPENGL

namespace Razix
{
    static bool sGLFWInitialized = false;

    GLFWWindow::GLFWWindow(const WindowProperties& properties)
    {
        Init(properties);
    }

    GLFWWindow::~GLFWWindow()
    {
        Shutdown();
    }

    void GLFWWindow::OnWindowUpdate()
    {
#ifdef RAZIX_RENDER_API_OPENGL
        m_Context->SwapBuffers();
#endif // RAZIX_RENDER_API_OPENGL
    }

    void GLFWWindow::ProcessInput()
    {
#ifdef RAZIX_RENDER_API_OPENGL
		glfwPollEvents();
#endif // RAZIX_RENDER_API_OPENGL
    }

    void GLFWWindow::SetVSync(bool enabled)
    {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.Vsync = enabled;
    }

    bool GLFWWindow::IsVSync() const
    {
        return m_Data.Vsync;
    }

    void GLFWWindow::SetWindowIcon()
    {
        uint8_t* pixels = nullptr;

        // 64-bit logo
        std::vector<GLFWimage> images;
        GLFWimage image64{};
        image64.height = RazixLogo64Height;
        image64.width = RazixLogo64Width;
        image64.pixels = static_cast<unsigned char*>(&RazixLogo64Pixels[0]);
        images.push_back(image64);

        // 32-bit logo
		GLFWimage image32{};
        image32.height = RazixLogo32Height;
        image32.width = RazixLogo32Width;
        image32.pixels = static_cast<unsigned char*>(RazixLogo32Pixels);
        images.push_back(image32);

        glfwSetWindowIcon(m_Window, int(images.size()), images.data());

    }

    void GLFWWindow::Construct()
    {
        ConstructionFunc = GLFWConstructionFunc;
    }

    Window* GLFWWindow::GLFWConstructionFunc(const WindowProperties& properties)
    {
        return new GLFWWindow(properties);
    }

    void GLFWWindow::Init(const WindowProperties& properties)
    {
        m_Data.Title = properties.Title;
        m_Data.Width = properties.Width;
        m_Data.Height = properties.Height;
        //m_Data.API = properties.API; // use this with a switch statement to choose a proper rendering API

        RAZIX_CORE_INFO("Creating Window... \n \t\t\t\t Title : {0} (Width : {1}, Height : {2})", properties.Title, properties.Width, properties.Height);

        glfwSetErrorCallback([](int errorCode, const char* description)
        {
            RAZIX_CORE_ERROR("GLFW Error! code : {0} description : {1}", errorCode, description);
        });

        // TODO: Replace all this with WIN32 API
        if (!sGLFWInitialized)
        {
            int success = glfwInit();
            RAZIX_CORE_ASSERT(success, "Could not initialize GLFW");

            sGLFWInitialized = true;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
        #endif

        m_Window = glfwCreateWindow((int)properties.Width, (int)properties.Height, properties.Title.c_str(), nullptr, nullptr);

#ifdef RAZIX_RENDER_API_OPENGL
        m_Context = new OpenGLContext(m_Window);
        m_Context->Init();
#endif

        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        std::string icon = std::string(STRINGIZE(RAZIX_ROOT_DIR)) + std::string("/Razix/src/Razix/Embedded/RazixLogo.png");
        SetWindowIcon();

        // Setting up event callbacks function via the dispatcher
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(key, 0);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(key);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                KeyPressedEvent event(key, 1);
                data.EventCallback(event);
                break;
            }
            }
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);

        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }

    void GLFWWindow::Shutdown()
    {
        glfwDestroyWindow(m_Window);
    }
}