#pragma once 

#ifndef RAZIX_DISTRIBUTION

// Profilers cannot be switched at runtime and need to be decided before compile time itself
#define RZ_PROFILER_TRACY                           1
#define RZ_PROFILER_OPTICK                          0
#define RZ_PROFILER_NVSIGHT                         0

// Predefined colors for profiling engine systems and API parts
// Graphics API will have separate colors for different parts such as for draw commands, pipeline binds etc.

#define RZ_PROFILE_COLOR_CORE                       0x8B0000
#define RZ_PROFILE_COLOR_APPLICATION                0xFFD700
#define RZ_PROFILE_COLOR_GRAPHICS                   0xFF8C00
#define RZ_PROFILE_COLOR_RENDERERS                  0xF08080
#define RZ_PROFILE_COLOR_SCENE                      0x00BFFF
#define RZ_PROFILE_COLOR_SCRIPTING                  0x8A2BE2
#define RZ_PROFILE_COLOR_ASSET_SYSTEM               0xFF8C00

#define RZ_PROFILE_COLOR_GRAPHICS_API_DRAW_CALLS    0x8B0000

#if RZ_PROFILER_TRACY
    #define TRACY_ENABLE
    #define TRACY_CALLSTACK                         1

    #include <Tracy.hpp>

    #define RAZIX_PROFILE_SCOPE(name)               ZoneScopedN(name)   
    #define RAZIX_PROFILE_SCOPEC(name, color)       ZoneScopedNC(name, color)
    #define RAZIX_PROFILE_FUNCTION()                ZoneScoped
    #define RAZIX_PROFILE_FUNCTIONC(color)          ZoneScopedC(color)
    #define RAZIX_PROFILE_FRAMEMARKER()             FrameMark
                                                    
    #define RAZIX_PROFILE_LOCK(type, var, name)     TracyLockableN(type, var, name)
    #define RAZIX_PROFILE_LOCKMARKER(var)           LockMark(var)
    #define RAZIX_PROFILE_SETTHREADNAME(name)       tracy::SetThreadName(name)
#endif

#else cn 

    #define RZ_PROFILE_COLOR_CORE         
    #define RZ_PROFILE_COLOR_GRAPHICS     
    #define RZ_PROFILE_COLOR_RENDERERS    
    #define RZ_PROFILE_COLOR_SCENE        
    #define RZ_PROFILE_COLOR_SCRIPTING    
    #define RZ_PROFILE_COLOR_ASSET_SYSTEM 

    #define RAZIX_PROFILE_SCOPE(name)          
    #define RAZIX_PROFILE_SCOPE(name, color)   
    #define RAZIX_PROFILE_FUNCTION()           
    #define RAZIX_PROFILE_FUNCTION(color)      
    #define RAZIX_PROFILE_FRAMEMARKER        

    #define RAZIX_PROFILE_LOCK(type, var, name)
    #define RAZIX_PROFILE_LOCKMARKER(var)      
    #define RAZIX_PROFILE_SETTHREADNAME(name)  

#endif