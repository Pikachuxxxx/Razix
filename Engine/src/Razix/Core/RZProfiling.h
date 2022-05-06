#pragma once

#ifndef RAZIX_DISTRIBUTION

    // Profilers cannot be switched at runtime and need to be decided before compile time itself
    #define RZ_PROFILER_TRACY   1
    #define RZ_PROFILER_OPTICK  0
    #define RZ_PROFILER_NVSIGHT 0

    #define RZ_ENABLE_CALL_STACK_CAPTURE 1
    #define RZ_CALL_STACK_DEPTH          15

    // Predefined colors for profiling engine systems and API parts
    #define RZ_PROFILE_COLOR_CORE         0x8B0000
    #define RZ_PROFILE_COLOR_APPLICATION  0xFFD700
    #define RZ_PROFILE_COLOR_GRAPHICS     0xFF8C00
    #define RZ_PROFILE_COLOR_RENDERERS    0xF08080
    #define RZ_PROFILE_COLOR_SCENE        0x00BFFF
    #define RZ_PROFILE_COLOR_SCRIPTING    0x8A2BE2
    #define RZ_PROFILE_COLOR_ASSET_SYSTEM 0xFF8C00

    // Graphics API will have separate colors for different parts such as for draw commands, pipeline binds etc.
    #define RZ_PROFILE_COLOR_GRAPHICS_API_DRAW_CALLS 0x8B0000

    // Tracy
    #if RZ_PROFILER_TRACY
        #define TRACY_ENABLE
        #define TRACY_CALLSTACK 1

        #include <Tracy.hpp>

        #if RZ_ENABLE_CALL_STACK_CAPTURE
            #define RAZIX_PROFILE_SCOPE(name)         ZoneScopedNS(name, RZ_CALL_STACK_DEPTH)
            #define RAZIX_PROFILE_SCOPEC(name, color) ZoneScopedNCS(name, color, RZ_CALL_STACK_DEPTH)
            #define RAZIX_PROFILE_FUNCTION()          ZoneScopedS(RZ_CALL_STACK_DEPTH)
            #define RAZIX_PROFILE_FUNCTIONC(color)    ZoneScopedCS(color, RZ_CALL_STACK_DEPTH)
            #define RAZIX_PROFILE_FRAMEMARKER(name)   FrameMark
        #else
            #define RAZIX_PROFILE_SCOPE(name)         ZoneScopedN(name)
            #define RAZIX_PROFILE_SCOPEC(name, color) ZoneScopedNC(name, color)
            #define RAZIX_PROFILE_FUNCTION()          ZoneScoped
            #define RAZIX_PROFILE_FUNCTIONC(color)    ZoneScopedC(color)
            #define RAZIX_PROFILE_FRAMEMARKER(name)   FrameMark
        #endif

        // GPU profiling needs API specific objects hence cannot be abstracted using macros but we can switch to required Profiler using defines, atleast for Tracy this is a mess
        #define RAZIX_PROFILE_GPU_CONTEXT(context)
        #define RAZIX_PROFILE_GPU_SCOPE(name)
        #define RAZIX_PROFILE_GPU_SCOPEC(name, color)
        //#define  RAZIX_PROFILE_GPU_FUNCTION()
        //#define  RAZIX_PROFILE_GPU_FUNCTION(color)
        #define RAZIX_PROFILE_GPU_FLIP(swapchain)

        #define RAZIX_PROFILE_LOCK(type, var, name) TracyLockableN(type, var, name)
        #define RAZIX_PROFILE_LOCKMARKER(var)       LockMark(var)
        #define RAZIX_PROFILE_SETTHREADNAME(name)   tracy::SetThreadName(name)
    #endif

    // OPTICK
    #if RZ_PROFILER_OPTICK

        #define USE_OPTICK 1
        #define OPTICK_ENABLE_GPU
        #include <optick.h>

        #define RAZIX_PROFILE_SCOPE(name)         OPTICK_EVENT(name)
        #define RAZIX_PROFILE_SCOPEC(name, color) OPTICK_EVENT(name)
        #define RAZIX_PROFILE_FUNCTION()          OPTICK_EVENT()
        #define RAZIX_PROFILE_FUNCTIONC(color)    OPTICK_EVENT()    // TODO: Add options to choose optick category or pass custom color just like we can in tracy
        #define RAZIX_PROFILE_FRAMEMARKER(name)   OPTICK_FRAME(name)

        #define RAZIX_PROFILE_GPU_CONTEXT(context)    OPTICK_GPU_CONTEXT(context)
        #define RAZIX_PROFILE_GPU_SCOPE(name)         OPTICK_GPU_EVENT(name)
        #define RAZIX_PROFILE_GPU_SCOPEC(name, color) OPTICK_GPU_EVENT(name)
        //#define RAZIX_PROFILE_GPU_FUNCTION()            OPTICK_GPU_EVENT(__FUNCTION__)
        //#define RAZIX_PROFILE_GPU_FUNCTION(color)       OPTICK_GPU_EVENT(__FUNCTION__)
        #define RAZIX_PROFILE_GPU_FLIP(swapchain) OPTICK_GPU_FLIP(swapchain)

        #define RAZIX_PROFILE_LOCK(type, var, name)
        #define RAZIX_PROFILE_LOCKMARKER(var)
        #define RAZIX_PROFILE_SETTHREADNAME(name) OPTICK_THREAD(name)
    #endif

#else

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
    #define RAZIX_PROFILE_FRAMEMARKER(name)

    #define RAZIX_PROFILE_GPU_CONTEXT(context)
    #define RAZIX_PROFILE_GPU_SCOPE(name)
    #define RAZIX_PROFILE_GPU_SCOPEC(name, color)
    //#define RAZIX_PROFILE_GPU_FUNCTION()
    //#define RAZIX_PROFILE_GPU_FUNCTION(color)
    #define RAZIX_PROFILE_GPU_FLIP(swapchain)

    #define RAZIX_PROFILE_LOCK(type, var, name)
    #define RAZIX_PROFILE_LOCKMARKER(var)
    #define RAZIX_PROFILE_SETTHREADNAME(name)

#endif