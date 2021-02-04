#pragma once

#include "Razix/Window.h"

#include <GLFW/glfw3.h>

namespace Razix
{
    class WindowsWindow : public Window
    {
    public:    
        WindowsWindow(const WindowProperties& properties);
        virtual ~WindowsWindow();

        void OnWindowUpdate() override;

        inline unsigned int GetWidth() const override { return m_Data.Width; }
        inline unsigned int GetHeight() const override { return m_Data.Height; }

        // Window Attributes
        inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        inline void SetVSync(bool enabled) override;
        bool IsVSync() const override;
    private:
        virtual void Init(const WindowProperties& properties);
        virtual void Shutdown();
    private:
        GLFWwindow* m_Window;

        struct WindowData
        {
            std::string Title;
            unsigned int Width, Height;
            bool Vsync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };

}

