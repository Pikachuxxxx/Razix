\    // clang-format off
#include "rzxpch.h"
// clang-format on
#include "GLFWWindow.h"

#include "Razix/Embedded/RazixLogo32.inl"
#include "Razix/Embedded/RazixLogo64.inl"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/RZKeyEvent.h"
#include "Razix/Events/RZMouseEvent.h"
#include "Razix/Utilities/LoadImage.h"

#ifdef RAZIX_RENDER_API_OPENGL
// clang-format off
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
// clang-format on
#endif

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

    namespace Razix
{
    static bool sGLFWInitialized = false;

    GLFWWindow::GLFWWindow(const WindowProperties& properties)
    {
        Init(properties);
    }

    GLFWWindow::~GLFWWindow()
    {
        //Shutdown();
    }

    void GLFWWindow::OnWindowUpdate()
    {
    }

    void GLFWWindow::ProcessInput()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

#if defined(RAZIX_RENDER_API_OPENGL) || defined(RAZIX_RENDER_API_VULKAN)
        if (Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::OPENGL || Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::VULKAN)
            glfwPollEvents();
#endif    // RAZIX_RENDER_API_OPENGL
    }

    void GLFWWindow::SetVSync(bool enabled)
    {
        if (Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::OPENGL) {
            if (enabled)
                glfwSwapInterval(1);
            else
                glfwSwapInterval(0);
        }
        m_Data.Vsync = enabled;
    }

    bool GLFWWindow::IsVSync() const
    {
        return m_Data.Vsync;
    }

    void GLFWWindow::SetWindowIcon()
    {
        u8* pixels = nullptr;

        // 64-bit logo
        std::vector<GLFWimage> images;
        GLFWimage              image64{};

        image64.height = RazixLogo64Height;
        image64.width  = RazixLogo64Width;
        image64.pixels = static_cast<unsigned char*>(&RazixLogo64Pixels[0]);
        images.push_back(image64);

        // 32-bit logo
        GLFWimage image32{};
        image32.height = RazixLogo32Height;
        image32.width  = RazixLogo32Width;
        image32.pixels = static_cast<unsigned char*>(RazixLogo32Pixels);
        images.push_back(image32);

        glfwSetWindowIcon(m_Window, int(images.size()), images.data());
    }

    void GLFWWindow::Construct()
    {
        ConstructionFunc = GLFWConstructionFunc;
    }

    void GLFWWindow::Destroy()
    {
        sGLFWInitialized = false;
        Shutdown();
    }

    RZWindow* GLFWWindow::GLFWConstructionFunc(const WindowProperties& properties)
    {
        return new GLFWWindow(properties);
    }

    void GLFWWindow::Init(const WindowProperties& properties)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

        m_Data.Title  = properties.Title;
        m_Data.Width  = properties.Width;
        m_Data.Height = properties.Height;
        //m_Data.API = properties.API; // use this with a switch statement to choose a proper rendering API

        RAZIX_CORE_INFO("Creating Window... \n \t\t\t\t Title : {0} (Width : {1}, Height : {2})", properties.Title, properties.Width, properties.Height);

        glfwSetErrorCallback([](int errorCode, cstr description) {
            RAZIX_CORE_ERROR("GLFW Error! code : {0} description : {1}", errorCode, description);
        });

        // TODO: Replace all this with WIN32 API
        if (!sGLFWInitialized) {
            int success = glfwInit();
            RAZIX_CORE_ASSERT(success, "Could not initialize GLFW");

            sGLFWInitialized = true;
        }

#ifdef RAZIX_RENDER_API_OPENGL
        if (Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::OPENGL) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    #endif
        }
#endif

#ifdef RAZIX_RENDER_API_VULKAN
        if (Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::VULKAN) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        }

#endif
        m_Window = glfwCreateWindow((int) properties.Width, (int) properties.Height, properties.Title.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(m_Window, &m_Data);

        //std::string icon = std::string(STRINGIZE(RAZIX_ROOT_DIR)) + std::string("/Razix/src/Razix/Embedded/RazixLogo.png");
        SetWindowIcon();

        // Setting up event callbacks function via the dispatcher
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            data.Width  = width;
            data.Height = height;

            RZWindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    RZKeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    RZKeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    RZKeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    RZMouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    RZMouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, d32 xOffset, d32 yOffset) {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            RZMouseScrolledEvent event((f32) xOffset, (f32) yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, d32 xPos, d32 yPos) {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            RZMouseMovedEvent event((f32) xPos, (f32) yPos);
            data.EventCallback(event);
        });
    }

    void GLFWWindow::Shutdown()
    {
        glfwDestroyWindow(m_Window);
    }
}    // namespace Razix