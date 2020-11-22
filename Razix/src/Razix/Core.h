#pragma once

#ifdef RZX_PLATFORM_WINDOWS
	#ifdef RZX_BUILD_DLL
		#define RAZIX_API __declspec(dllexport)
	#else
		#define RAZIX_API __declspec(dllimport)
	#endif
#elif RZX_PLATFORM_MACOS
	#error Does not support MacOS yet!
#endif 

#define BIT(x) (1 << x) 