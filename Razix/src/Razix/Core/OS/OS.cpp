#include "rzxpch.h"
#include "OS.h"

#ifdef RAZIX_PLATFORM_WINDOWS
#include "Razix/Platform/Windows/WindowsOS.h"
#endif // RAZIX_PLATFORM_WINDOWS


namespace Razix
{
	OS* OS::s_Instance = nullptr;
	
	void OS::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}

