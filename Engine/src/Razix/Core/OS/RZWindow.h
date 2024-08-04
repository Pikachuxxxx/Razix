#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZLog.h"
#include "Razix/Events/RZEvent.h"

namespace Razix {
    /* A struct to set different window properties */
    // TODO: Store some properties in the Application class
    struct WindowProperties
    {
        /**
		 * Sets a list of properties to the window
		 * 
		 * @param title		The title of the window </param>
		 * @param width		The width of the window </param>
		 * @param height		The Height of the window </param>
		 * @param renderAPI	Current Render API being used </param>
		 * @param fullscreen	Should the window open in fullscreen </param>
		 * @param vSync		Whether to enable vsync or not </param>
		 * @param borderless Should the window be borderless or not </param>
		 * @param filepath	The path of the application </param>
		 */
        WindowProperties(const std::string& title = "Razix Engine", u32 width = 1280, u32 height = 720, bool fullscreen = false, bool vSync = true, bool borderless = false)
            : Width(width), Height(height), Title(title), Fullscreen(fullscreen), VSync(vSync), Borderless(borderless)
        {
        }

        /* Width and Height of the window */
        u32 Width, Height;
        /* The title of the window */
        std::string Title;
        /* Fullscreen status of the window */
        bool Fullscreen;
        /* A boolean to whether enable V-Sync or not */
        bool VSync;
        /*Whether the window should be created with window or not */
        bool Borderless;
        /* Whether or not to show the debug window console or not */
        bool ShowConsole = true;
    };

    /* Window Interface for Desktop systems */
    // TODO: Add methods for all the window properties in the WindowProperties Constructor to be get/set in here
    class RAZIX_API RZWindow : public RZRoot
    {
    public:
        using EventCallbackFn = std::function<void(RZEvent&)>;

        /**
         * Creates a Razix window with the given window properties
         *
         * @param properties A struct with the custom window properties
         * @returns Handle to the Razix window
         */
        static RZWindow* Create(const WindowProperties& properties = WindowProperties());
        static RZWindow* Create(void* nativeHandle, const WindowProperties& properties = WindowProperties());
        virtual ~RZWindow() {}

        /* Called on Every Window Update */
        virtual void OnWindowUpdate() = 0;

        /* Process the Input polling */
        virtual void ProcessInput(){};

        virtual void Destroy() = 0;

        /* Gets the Width of the window */
        virtual unsigned int getWidth() const = 0;

        /* Gets the Height of the window */
        virtual unsigned int getHeight() const = 0;

        virtual void setTitle(const char* title) = 0;

        // Window Attributes

        /**
		 * Set the event callback function for Window events
		 * 
		 * @param callback Pointer to the callback function 
		 */
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        /* Enables V - Sync */
        virtual void SetVSync(bool enabled) = 0;

        /* Tells whether or not VSync was enabled or not */
        virtual bool IsVSync() const = 0;

        /* Sets the window Icon */
        virtual void SetWindowIcon() = 0;

        /**
         * Gets the native window handle for the underlying OS
         * @returns A pointer to the underlying OS native handle
         */
        virtual void* GetNativeWindow() const = 0;

    protected:
        /* Pointer to the appropriate Window Creation function based on OS */
        static RZWindow* (*ConstructionFunc)(const WindowProperties&);
        static RZWindow* (*ConstructionFuncNative)(void*, const WindowProperties&);
        RZWindow() = default;
    };
}    // namespace Razix