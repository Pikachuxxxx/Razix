#pragma once

#include "Razix/Core/Log.h"
#include "Razix/Core/Core.h"
#include "Razix/Events/event.h"

namespace Razix
{
    struct WindowProperties
    {
		WindowProperties(const std::string& title = "Razix", uint32_t width = 1280, uint32_t height = 720, int renderAPI = 0, bool fullscreen = false, bool vSync = true, bool borderless = false, const std::string& filepath = "")
			: Width(width)
			, Height(height)
			, Title(title)
			, Fullscreen(fullscreen)
			, VSync(vSync)
			, Borderless(borderless)
			, RenderAPI(renderAPI)
			, FilePath(filepath)
		{
		}

		uint32_t Width, Height;
		bool Fullscreen;
		bool VSync;
		bool Borderless;
		bool ShowConsole = true;
		std::string Title;
		int RenderAPI;
		std::string FilePath;
    };

    /// Window Interface for Desktop systems
    class RAZIX_API Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        static Window* Create(const WindowProperties& properties = WindowProperties());
        virtual ~Window() {}

        virtual void OnWindowUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        // Window Attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0;
	protected:
		static Window* (*ConstructionFunc)(const WindowProperties&);
		Window() = default;

    };

}