#pragma once

#include "Razix/Core/OS/RZWindow.h"

// Forward declaring to reduce header file complexity
struct GLFWwindow;

namespace Razix {
    class GLFWWindow : public RZWindow
    {
    public:
        GLFWWindow(const WindowProperties& properties);
        ~GLFWWindow();

        void OnWindowUpdate() override;
        void ProcessInput() override;

        inline unsigned int getWidth() const override { return m_Data.Width; }
        inline unsigned int getHeight() const override { return m_Data.Height; }
        void                setTitle(const char* title) override;

        // Window Attributes
        inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        inline void SetVSync(bool enabled) override;
        bool        IsVSync() const override;
        void        SetWindowIcon() override;

        virtual void* GetNativeWindow() const override { return m_Window; }

        static void Construct();

        void Destroy() override;

    private:
        static RZWindow* GLFWConstructionFunc(const WindowProperties& properties);

        virtual void Init(const WindowProperties& properties);
        virtual void Shutdown();

        GLFWwindow* m_Window;

        struct WindowData
        {
            std::string Title;
            u32         Width;
            u32         Height;
            //            u32             wScale;
            //            u32             hScale;
            bool            Vsync;
            EventCallbackFn EventCallback;
        } m_Data;    // Call me old fashioned, Bitch!!!
    };
}    // namespace Razix
