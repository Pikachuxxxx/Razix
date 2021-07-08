#pragma once

#include "Razix/Core/OS/Window.h"
#include "Razix/Renderer/GraphicsContext.h"

// Forward declaring to reduce header file complexity
struct GLFWwindow;

namespace Razix
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowProperties& properties);
		~GLFWWindow();

		void OnWindowUpdate() override;
		void ProcessInput() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window Attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		inline void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		void SetWindowIcon() override;

		virtual void* GetNativeWindow() const override { return m_Window; }

		static void Construct();

	private:
		static Window* GLFWConstructionFunc(const WindowProperties& properties);

		virtual void Init(const WindowProperties& properties);
		virtual void Shutdown();

		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title{};
			unsigned int Width{}, Height{};
			bool Vsync;

			EventCallbackFn EventCallback;
		} m_Data; // Call me old fashioned, Bitch!!!
	};
}

