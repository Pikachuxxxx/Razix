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
        const std::string Key   = name;  \
        auto              start = std::chrono::high_resolution_clock::now();

    #define RAZIX_TIME_STAMP_END()                                                                           \
        auto                                   stop             = std::chrono::high_resolution_clock::now(); \
        std::chrono::duration<f32, std::milli> ms_f32           = (stop - start);                            \
        Razix::RZEngine::Get().GetStatistics().PassTimings[Key] = ms_f32.count();

#else
    #define RAZIX_TIME_STAMP_BEGIN(name)
    #define RAZIX_TIME_STAMP_END()
#endif