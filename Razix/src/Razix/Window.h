#pragma once

#include "rzxpch.h"

#include "Razix/Core.h"
#include "Razix/Log.h"
#include "razix/events/event.h"

namespace Razix
{
    struct WindowProperties
    {
        std::string Title;
        unsigned int Width;
        unsigned int Height;

        WindowProperties(const std::string& title = "Razix Engine",
                         unsigned int width = 1000,
                         unsigned int height = 800) 
                : Title(title), Width(width), Height(height)
        {
        }
    };

    // Window Interface for Desktop systems
    class RAZIX_API Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() {}

        virtual void OnWindowUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        // Window Attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        static Window* Create(const WindowProperties& properties = WindowProperties());

        virtual void* GetNativeWindow() const = 0;
    };

}