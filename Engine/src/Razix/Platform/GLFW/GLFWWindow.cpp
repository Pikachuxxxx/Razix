// clang-format off
#include "rzxpch.h"
// clang-format on
#include "GLFWWindow.h"

#include "Razix/Embedded/RazixLogo32.inl"
#include "Razix/Embedded/RazixLogo64.inl"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/RZKeyEvent.h"
#include "Razix/Events/RZMouseEvent.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Core/Utils/RZLoadImage.h"

#if defined RAZIX_RENDER_API_OPENGL || RAZIX_RENDER_API_VULKAN
// clang-format off
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
// clang-format on
#endif

namespace Razix {
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

#if defined(RAZIX_RENDER_API_VULKAN) || defined(RAZIX_RENDER_API_DIRECTX12)
        glfwPollEvents();
#endif    // RAZIX_RENDER_API_OPENGL
    }

    void GLFWWindow::SetVSync(bool enabled)
    {
        m_Data.Vsync = enabled;
    }

    bool GLFWWindow::IsVSync() const
    {
        return m_Data.Vsync;
    }

    void GLFWWindow::SetWindowIcon()
    {
        // 64-bit logo
        RZDynamicArray<GLFWimage> images;
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

    void GLFWWindow::setTitle(const char* title)
    {
        glfwSetWindowTitle(m_Window, title);
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

        RAZIX_CORE_INFO("Creating Window... \n \t\t\t\t Title : {0} (Width : {1}, Height : {2})", properties.Title, properties.Width, properties.Height);

        glfwSetErrorCallback([](int errorCode, cstr description) {
            RAZIX_CORE_ERROR("GLFW Error! code : {0} description : {1}", errorCode, description);
        });

        if (!sGLFWInitialized) {
            int success = glfwInit();
            RAZIX_CORE_ASSERT(success, "Could not initialize GLFW");
            RAZIX_UNUSED(success);

            sGLFWInitialized = true;
        }

#if defined RAZIX_RENDER_API_VULKAN || defined RAZIX_RENDER_API_DIRECTX12
        if (rzGfxCtx_GetRenderAPI() == RZ_RENDER_API_VULKAN || rzGfxCtx_GetRenderAPI() == RZ_RENDER_API_D3D12) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        }
#endif

#ifdef __APPLE__
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
        // FIXME: Disable resizing in Apple until I sort out crashes and resizing withung correct swapchain extents and use proper DPI scaling!
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#endif

        int           monitorCount;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
        if (monitors && monitorCount > 0) {
// Use the primary monitor for now, since we don't want to crash at startup on the monitor the game launches on
#define PRIMARY_MONITOR_IDX 0
            GLFWmonitor* primary = monitors[PRIMARY_MONITOR_IDX];
            int          workWidth, workHeight;
            glfwGetMonitorWorkarea(primary, nullptr, nullptr, &workWidth, &workHeight);

            m_Data.Width  = std::min(properties.Width, static_cast<u32>(workWidth));
            m_Data.Height = std::min(properties.Height, static_cast<u32>(workHeight));
        }

        m_Window = glfwCreateWindow((int) m_Data.Width, (int) m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

        //        int fbWidth, fbHeight;
        //        glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
        // update DPI
        //        float xscale, yscale;
        //        glfwGetWindowContentScale(m_Window, &xscale, &yscale);
        //        m_Data.wScale = int(xscale);
        //        m_Data.hScale = int(yscale);

        glfwSetWindowUserPointer(m_Window, &m_Data);

        //RZString icon = RZString(STRINGIZE(RAZIX_ROOT_DIR)) + RZString("/Razix/src/Razix/Embedded/RazixLogo.png");
#ifndef __APPLE__
        SetWindowIcon();
#endif

        // The problem with using capture list here is the user_data is a global state, it's not per callback function and that will not properly capture the modified lambda with the necessary args and maintain their lifetime properly
        // Setting up event callbacks function via the dispatcher
        //        SetWindowSizeCallback(m_Window, [&](int width, int height) {
        //
        //            m_Data.Width  = width;
        //            m_Data.Height = height;
        //
        //            RZWindowResizeEvent event(width, height);
        //            m_Data.EventCallback(event);
        //        });

        // Handle high DPI-monitors (only Primary for now)
        //        glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow* window, float xscale, float yscale){
        //            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);
        //
        //            data.wScale = static_cast<u32>(xscale);
        //            data.hScale = static_cast<u32>(yscale);
        //
        //            data.Width *= data.wScale;
        //            data.Height *= data.hScale;
        //
        //            RZWindowResizeEvent event(data.Width, data.Height);
        //            data.EventCallback(event);
        //        });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            data.Width  = width;
            data.Height = height;

            if (!data.Width || !data.Height)
                return;

            RZWindowResizeEvent event(data.Width, data.Height);
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
