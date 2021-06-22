#include "rzxpch.h"
#include "WindowsOS.h"

#include "Razix/Core/Application.h"

#include "Razix/Core/OS/Window.h"
#ifdef RAZIX_USE_GLFW_WINDOWS
#include "Razix/Platform/OpenGL/GLFWWindow.h"
#else
#include "Razix/Platform/Windows/WindowsWindow.h"
#endif

namespace Razix
{
	void WindowsOS::Init()
	{
		RAZIX_CORE_TRACE("System OS : Windows");
	#ifdef RAZIX_USE_GLFW_WINDOWS
		GLFWWindow::Construct();
	#else 
		WindowsWindow::Construct();
	#endif

	}

	void WindowsOS::Run()
	{
		// TODO: Implement the Application life cycle here! in a more Robust way

		// Application auto Initialization by the Engine
		/*auto app = */Razix::Application::GetApplication().Run();
		//app.Run();
	}

}