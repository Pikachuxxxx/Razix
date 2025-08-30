#pragma once

// TODO: https://nikitablack.github.io/post/how_to_use_vulkan_timestamp_queries/ use this to manually profile GPU timings

// Profilers cannot be switched at runtime and need to be decided before compile time itself
// Note: Using a profiler especially tracy increases the memory consumption monotonically, it's not a memory leak!
#define RZ_PROFILER_ENABLED 1
#define RZ_PROFILER_TRACY   1
#define RZ_PROFILER_OPTICK  0
#define RZ_PROFILER_NVSIGHT 0

#define RZ_ENABLE_CALL_STACK_CAPTURE 0
#define RZ_CALL_STACK_DEPTH          15

// Predefined colors for profiling engine systems and API parts
#define RZ_PROFILE_COLOR_CORE         0x8B0000
#define RZ_PROFILE_COLOR_APPLICATION  0xFFD700
#define RZ_PROFILE_COLOR_GRAPHICS     0xFF8C00
#define RZ_PROFILE_COLOR_RENDERERS    0xF08080
#define RZ_PROFILE_COLOR_SCENE        0x00BFFF
#define RZ_PROFILE_COLOR_SCRIPTING    0x8A2BE2
#define RZ_PROFILE_COLOR_ASSET_SYSTEM 0xFF8C00
#define RZ_PROFILE_COLOR_RHI          0x32CD32

// Graphics API will have separate colors for different parts such as for draw commands, pipeline binds etc.
#define RZ_PROFILE_COLOR_GRAPHICS_API_DRAW_CALLS 0x8B0000

// RHI operation specific profiling colors
#define RZ_PROFILE_COLOR_RHI_COMMAND_BUFFERS   0x228B22    // Forest Green
#define RZ_PROFILE_COLOR_RHI_RENDER_PASSES     0x4169E1    // Royal Blue
#define RZ_PROFILE_COLOR_RHI_PIPELINE_BINDS    0xFF4500    // Orange Red
#define RZ_PROFILE_COLOR_RHI_DRAW_CALLS        0xDC143C    // Crimson
#define RZ_PROFILE_COLOR_RHI_RESOURCE_BARRIERS 0x9932CC    // Dark Orchid
#define RZ_PROFILE_COLOR_RHI_SYNCHRONIZATION   0xFFD700    // Gold
#define RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS  0x00CED1    // Dark Turquoise

#if !defined(RAZIX_GOLD_MASTER) && RZ_PROFILER_ENABLED
    // Tracy
    #if RZ_PROFILER_TRACY
        #define TRACY_CALLSTACK 1

        #ifdef __cplusplus
            // C++ Mode - Use Tracy.hpp
            #include <tracy/Tracy.hpp>

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

            #define RAZIX_PROFILE_LOCK(type, var, name) TracyLockableN(type, var, name)
            #define RAZIX_PROFILE_LOCKMARKER(var)       LockMark(var)
            #define RAZIX_PROFILE_SETTHREADNAME(name)   tracy::SetThreadName(name)
        #else
            // C Mode - Use TracyC.h
            #include <tracy/TracyC.h>

            #if RZ_ENABLE_CALL_STACK_CAPTURE
                #define RAZIX_PROFILE_SCOPE_BEGIN(name)         TracyCZoneNS(ctx, name, 1, RZ_CALL_STACK_DEPTH)
                #define RAZIX_PROFILE_SCOPE_END()               TracyCZoneEnd(ctx)
                #define RAZIX_PROFILE_SCOPEC_BEGIN(name, color) TracyCZoneNCS(ctx, name, 1, color, RZ_CALL_STACK_DEPTH)
                #define RAZIX_PROFILE_SCOPEC_END()              TracyCZoneEnd(ctx)
                #define RAZIX_PROFILE_FUNCTION_BEGIN()          TracyCZoneS(ctx, 1, RZ_CALL_STACK_DEPTH)
                #define RAZIX_PROFILE_FUNCTION_END()            TracyCZoneEnd(ctx)
                #define RAZIX_PROFILE_FUNCTIONC_BEGIN(color)    TracyCZoneCS(ctx, 1, color, RZ_CALL_STACK_DEPTH)
                #define RAZIX_PROFILE_FUNCTIONC_END()           TracyCZoneEnd(ctx)
            #else
                #define RAZIX_PROFILE_SCOPE_BEGIN(name)         TracyCZoneN(ctx, name, 1)
                #define RAZIX_PROFILE_SCOPE_END()               TracyCZoneEnd(ctx)
                #define RAZIX_PROFILE_SCOPEC_BEGIN(name, color) TracyCZoneNC(ctx, name, 1, color)
                #define RAZIX_PROFILE_SCOPEC_END()              TracyCZoneEnd(ctx)
                #define RAZIX_PROFILE_FUNCTION_BEGIN()          TracyCZone(ctx, 1)
                #define RAZIX_PROFILE_FUNCTION_END()            TracyCZoneEnd(ctx)
                #define RAZIX_PROFILE_FUNCTIONC_BEGIN(color)    TracyCZoneC(ctx, 1, color)
                #define RAZIX_PROFILE_FUNCTIONC_END()           TracyCZoneEnd(ctx)
            #endif

            // Existing single-line macros for convenience:
            #if RZ_ENABLE_CALL_STACK_CAPTURE
                #define RAZIX_PROFILE_SCOPE(name)                    \
                    TracyCZoneNS(ctx, name, 1, RZ_CALL_STACK_DEPTH); \
                    TracyCZoneCtx ctx
                #define RAZIX_PROFILE_SCOPEC(name, color)                    \
                    TracyCZoneNCS(ctx, name, 1, color, RZ_CALL_STACK_DEPTH); \
                    TracyCZoneCtx ctx
                #define RAZIX_PROFILE_FUNCTION()              \
                    TracyCZoneS(ctx, 1, RZ_CALL_STACK_DEPTH); \
                    TracyCZoneCtx ctx
                #define RAZIX_PROFILE_FUNCTIONC(color)                \
                    TracyCZoneCS(ctx, 1, color, RZ_CALL_STACK_DEPTH); \
                    TracyCZoneCtx ctx
            #else
                #define RAZIX_PROFILE_SCOPE(name) \
                    TracyCZoneN(ctx, name, 1);    \
                    TracyCZoneCtx ctx
                #define RAZIX_PROFILE_SCOPEC(name, color) \
                    TracyCZoneNC(ctx, name, 1, color);    \
                    TracyCZoneCtx ctx
                #define RAZIX_PROFILE_FUNCTION() \
                    TracyCZone(ctx, 1);          \
                    TracyCZoneCtx ctx
                #define RAZIX_PROFILE_FUNCTIONC(color) \
                    TracyCZoneC(ctx, 1, color);        \
                    TracyCZoneCtx ctx
            #endif

            #define RAZIX_PROFILE_FRAMEMARKER(name) TracyCFrameMark

            #define RAZIX_PROFILE_LOCK(type, var, name) type var
            #define RAZIX_PROFILE_LOCKMARKER(var)
            #define RAZIX_PROFILE_SETTHREADNAME(name) TracyCSetThreadName(name)
        #endif

        // GPU profiling needs API specific objects hence cannot be abstracted using macros but we can switch to required Profiler using defines, atleast for Tracy this is a mess
        #define RAZIX_PROFILE_GPU_CONTEXT(context)
        #define RAZIX_PROFILE_GPU_SCOPE(name)
        #define RAZIX_PROFILE_GPU_SCOPEC(name, color)
        //#define  RAZIX_PROFILE_GPU_FUNCTION()
        //#define  RAZIX_PROFILE_GPU_FUNCTION(color)
        #define RAZIX_PROFILE_GPU_FLIP(swapchain)

    // OPTICK
    #elif RZ_PROFILER_OPTICK

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
        #define RAZIX_PROFILE_GPU_FLIP(swapchain)     OPTICK_GPU_FLIP(swapchain)

        #define RAZIX_PROFILE_LOCK(type, var, name)
        #define RAZIX_PROFILE_LOCKMARKER(var)
        #define RAZIX_PROFILE_SETTHREADNAME(name) OPTICK_THREAD(name)
    #else
        #define RAZIX_PROFILE_SCOPE(name)
        #define RAZIX_PROFILE_SCOPEC(name, color)
        #define RAZIX_PROFILE_FUNCTION()
        #define RAZIX_PROFILE_FUNCTIONC(color)
        #define RAZIX_PROFILE_FRAMEMARKER(name)

        #define RAZIX_PROFILE_SCOPE_BEGIN(name)         TracyCZoneN(ctx, name, 1)
        #define RAZIX_PROFILE_SCOPE_END()               TracyCZoneEnd(ctx)
        #define RAZIX_PROFILE_SCOPEC_BEGIN(name, color) TracyCZoneNC(ctx, name, 1, color)
        #define RAZIX_PROFILE_SCOPEC_END()              TracyCZoneEnd(ctx)
        #define RAZIX_PROFILE_FUNCTION_BEGIN()          TracyCZone(ctx, 1)
        #define RAZIX_PROFILE_FUNCTION_END()            TracyCZoneEnd(ctx)
        #define RAZIX_PROFILE_FUNCTIONC_BEGIN(color)    TracyCZoneC(ctx, 1, color)
        #define RAZIX_PROFILE_FUNCTIONC_END()           TracyCZoneEnd(ctx)

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
#else
    #define RAZIX_PROFILE_SCOPE(name)
    #define RAZIX_PROFILE_SCOPEC(name, color)
    #define RAZIX_PROFILE_FUNCTION()
    #define RAZIX_PROFILE_FUNCTIONC(color)
    #define RAZIX_PROFILE_FRAMEMARKER(name)

    #define RAZIX_PROFILE_SCOPE_BEGIN(name)
    #define RAZIX_PROFILE_SCOPE_END()
    #define RAZIX_PROFILE_SCOPEC_BEGIN(name, color)
    #define RAZIX_PROFILE_SCOPEC_END()
    #define RAZIX_PROFILE_FUNCTION_BEGIN()
    #define RAZIX_PROFILE_FUNCTION_END()
    #define RAZIX_PROFILE_FUNCTIONC_BEGIN(color)
    #define RAZIX_PROFILE_FUNCTIONC_END()

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