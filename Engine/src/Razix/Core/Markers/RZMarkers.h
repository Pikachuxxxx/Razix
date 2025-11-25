#pragma once

// GPU Marker macros - API agnostic interface
#ifndef RAZIX_GOLD_MASTER
    #define RAZIX_MARK_BEGIN(cmdBuf, name, labelColor) Razix::Gfx::BeginMarker(cmdBuf, name, labelColor)
    #define RAZIX_MARK_ADD(cmdBuf, name, labelColor)   Razix::Gfx::InsertMarker(cmdBuf, name, labelColor)
    #define RAZIX_MARK_END(cmdBuf)                     Razix::Gfx::EndMarker(cmdBuf)
#else
    #define RAZIX_MARK_BEGIN(cmdBuf, name, labelColor)
    #define RAZIX_MARK_ADD(cmdBuf, name, labelColor)
    #define RAZIX_MARK_END(cmdBuf)
#endif

//------------------------------------------------------------------------
// CPU timestamp markers

#ifndef RAZIX_GOLD_MASTER

    #define RAZIX_TIME_STAMP_BEGIN(name) \
        const RZString& Key   = name;    \
        auto            start = rz_time_now();

    #define RAZIX_TIME_STAMP_END()                                               \
        auto stop                                               = rz_time_now(); \
        Razix::RZEngine::Get().GetStatistics().PassTimings[Key] = static_cast<f32>(rz_get_elapsed_ms(start, stop));

#else
    #define RAZIX_TIME_STAMP_BEGIN(name)
    #define RAZIX_TIME_STAMP_END()
#endif