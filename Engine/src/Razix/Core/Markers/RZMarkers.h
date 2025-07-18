#pragma once

// TODO: Move GFX markers to RHI, calle them rzRHI_GfxMarker along with Query markers (timestamp and pipeline stats)

#define RAZIX_MARK_BEGIN(name, labelColor)
#define RAZIX_MARK_ADD(name, labelColor)
#define RAZIX_MARK_END()

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