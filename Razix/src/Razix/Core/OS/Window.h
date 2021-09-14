#pragma once

#include "Razix/Core/Log.h"
#include "Razix/Core/Core.h"
#include "Razix/Events/event.h"

namespace Razix
{
    /// <summary>
    /// A struct to set different window properties
    /// </summary>
	// TODO: Store some properties in the Application class
    struct WindowProperties
    {
		/// <summary>
		/// Sets a list of properties to the window
		/// </summary>
		/// <param name="title"> The title of the window </param>
		/// <param name="width"> The width of the window </param>
		/// <param name="height"> The Height of the window </param>
		/// <param name="renderAPI"> Current Render API being used </param>
		/// <param name="fullscreen"> Should the winodw open in fullscreen </param>
		/// <param name="vSync"> Whether to enable vsync or not </param>
		/// <param name="borderless"> Should the window be borderless or not </param>
		/// <param name="filepath"> The path of the application </param>
		WindowProperties(const std::string& title = "Razix Engine", uint32_t width = 1280, uint32_t height = 720, bool fullscreen = false, bool vSync = true, bool borderless = false)
			: Width(width)
			, Height(height)
			, Title(title)
			, Fullscreen(fullscreen)
			, VSync(vSync)
			, Borderless(borderless)
		{
		}

		/// Width and Height of the window
		uint32_t Width, Height;
		/// Fullscreen status of the window
		bool Fullscreen;
		/// A boolean to whether enable V-Sync or not
		bool VSync;
		///	Whether the window should be created with window or not
		bool Borderless;
		/// Whether or not to show the debug window console or not
		bool ShowConsole = true;
		///	The title of the window
		std::string Title;
    };

    /// Window Interface for Desktop systems
	// TODO: Add methods for all the window properties in the WindowProperties Constructor to be get/set in here
    class RAZIX_API Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        /// <summary>
        /// Creates a Razix window with the given window properties
        /// </summary>
        /// <param name="properties"> A struct with the custom window properties </param>
        /// <returns> Handle to the Razix window </returns>
        static Window* Create(const WindowProperties& properties = WindowProperties());
        virtual ~Window() {}

        /// <summary>
        /// Called on Every Window Update
        /// </summary>
        virtual void OnWindowUpdate() = 0;

		/// <summary>
		/// Process the Input polling
		/// </summary>
		virtual void ProcessInput() {};

        /// <summary>
        /// Gets the Width of the window
        /// </summary>
        virtual unsigned int GetWidth() const = 0;
        
		/// <summary>
		/// Gets the Height of the window
		/// </summary>
		virtual unsigned int GetHeight() const = 0;

        // Window Attributes
		
        /// <summary>
        /// Sets the window call back function
        /// </summary>
        /// <param name="callback"> Pointer to the callback function </param>
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        /// <summary>
        /// Enables V-Sync
        /// </summary>
        /// <param name="enabled"> Whether or not to enable v-sycn </param>
        virtual void SetVSync(bool enabled) = 0;

        /// <summary>
        /// Tells whether or not VSync was enabled or not
        /// </summary>
        virtual bool IsVSync() const = 0;

		/// <summary>
		/// Sets the window Icon
		/// </summary>
		virtual void SetWindowIcon() = 0;

        /// <summary>
        /// Gets the native window handle for the underlying OS
        /// </summary>
        /// <returns> A pointer to the underlying OS native handle </returns>
        virtual void* GetNativeWindow() const = 0;
	protected:
		/// <summary>
		/// Pointer to the appropriate Window Creation function based on OS
		/// </summary>
		static Window* (*ConstructionFunc)(const WindowProperties&);
		Window() = default;
    };
}