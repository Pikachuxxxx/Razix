#pragma once

// EASTL is removed we write our onw containers as needed

// Thanks to my colleague and friend CybernetHacker14 (Ujjwal Raut) https://github.com/CybernetHacker14 for this switch mechanism b/w EASTL and RZSTL/std STL
// From https://github.com/CybernetHacker14/Sentinel/blob/e4e6325e3a3e3e0b98f446917506798c86c4f26e/Engine/Source/Sentinel/Common/Core/STL.h

// Currently since RZSTL is WIP we will fallback to EASTL, this can be done even when RZSTL cannot implement somethings and if some vendors needs it
#define USE_EASTL 0

// Functions Calling Conventions for Razix Engine depending on the OS and compiler configuration
// On Windows we use __cdecl by default however __stdcall might be necessary for interop API with Razix Engine and C#
#define RAZIX_CALLEE
#define RAZIX_CALLER
#undef RAZIX_CALLEE
#ifdef RAZIX_CALLEE
    #define RAZIX_CALLING_CONVENTION __stdcall
#endif
#ifdef RAZIX_CALLER
    #define RAZIX_CALLING_CONVENTION __cdecl
#endif

