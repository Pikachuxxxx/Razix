#include "rzxpch.h"
#include "WindowsOS.h"

#include "Razix/Core/Application.h"

#include "Razix/Core/OS/Window.h"
#ifdef RAZIX_USE_GLFW_WINDOWS
#include "Razix/Platform/GLFW/GLFWWindow.h"
#include "Razix/Platform/GLFW/GLFWInput.h"
#else
#include "Razix/Platform/Windows/WindowsWindow.h"
#include "Razix/Platform/Windows/WindowsInput.h"
#endif

namespace Razix
{
	void WindowsOS::Init()
	{
		RAZIX_CORE_INFO("Creating Windows OS instance");
		RAZIX_CORE_INFO("System OS : {0}", STRINGIZE(RAZIX_BUILD_CONFIG));
	#ifdef RAZIX_USE_GLFW_WINDOWS
		GLFWWindow::Construct();
		// TODO: Initialize the OS specific Input Implementation here
	#else 
		WindowsWindow::Construct();
		Razix::RazixInput* Razix::RazixInput::sInstance = new WindowsInput();
	#endif

	}

	void WindowsOS::Run()
	{
		// TODO: Implement the Application life cycle here! in a more Robust way

		// Application auto Initialization by the Engine
		Razix::Application::Get().Run();
	}

}