#include "rzxpch.h"
#include "OS.h"

namespace Razix
{
	RZOS* RZOS::s_Instance = nullptr;
	
	void RZOS::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}

