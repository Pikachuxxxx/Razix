#pragma once

// DLL Export and Import settings for Windows OS
#ifdef RZX_PLATFORM_WINDOWS
    #ifdef RZX_BUILD_DLL
        #define RAZIX_API __declspec(dllexport)
    #else
        #define RAZIX_API __declspec(dllimport)
    #endif

#elif RZX_PLATFORM_MACOS
    #error Does not support MacOS yet!
#endif 

// Platform specific Debug functions 
#ifdef RZX_PLATFORM_WINDOWS
    #define RZX_DEBUG_BREAK() __debugbreak()
#elif
    #define RZX_DEBUG_BREAK() 
#endif

// Asserts
#if RZX_ENABLE_ASSERTS // Only available in Debug Builds (predefined by the build system only in Debug mode)
    #define RZX_CORE_ASSERT(x, ...) {if(!(x)) { RZX_CORE_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); RZX_DEBUG_BREAK(); }}
    #define RZX_ASSERT(x, ...) {if(!(x)) { RZX_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); RZX_DEBUG_BREAK(); }}
#else 
    #define RZX_CORE_ASSERT(x, ...)
    #define RZX_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x) 