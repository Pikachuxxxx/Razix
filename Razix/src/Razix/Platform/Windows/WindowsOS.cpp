#include "rzxpch.h"
#include "WindowsOS.h"

#include "Razix/Core/Application.h"

#include "Razix/Core/OS/Window.h"
#ifdef RAZIX_USE_GLFW_WINDOWS
#include "Razix/Platform/OpenGL/GLFWWindow.h"
#include "Razix/Platform/OpenGL/GLFWInput.h"
#else
#include "Razix/Platform/Windows/WindowsWindow.h"
#include "Razix/Platform/OpenGL/WindowsInput.h"
#endif

namespace Razix
{
	void WindowsOS::Init()
	{
		RAZIX_CORE_INFO("System OS : {0}", STRINGIZE(RAZIX_BUILD_CONFIG));
	#ifdef RAZIX_USE_GLFW_WINDOWS
		GLFWWindow::Construct();
	#else 
		WindowsWindow::Construct();
		Razix::Input* Razix::Input::sInstance = new WindowsInput();
	#endif

	}

	void WindowsOS::Run()
	{
		// TODO: Implement the Application life cycle here! in a more Robust way

		// Application auto Initialization by the Engine
		Razix::Application::GetApplication().Run();
	}

}