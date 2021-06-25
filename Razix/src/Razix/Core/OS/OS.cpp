#include "rzxpch.h"
#include "OS.h"

namespace Razix
{
	OS* OS::s_Instance = nullptr;
	
	void OS::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}

