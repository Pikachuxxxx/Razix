#pragma once

#ifndef RAZIX_GOLD_MASTER

    #define RAZIX_MARK_BEGIN(name, labelColor) BeginMarker(name, labelColor);

    #define RAZIX_MARK_ADD(name, labelColor) InsertMarker(name, labelColor);

    #define RAZIX_MARK_END() EndMarker();

#else

    #define RAZIX_MARK_BEGIN(name, labelColor)

    #define RAZIX_MARK_ADD(name, labelColor)

    #define RAZIX_MARK_END()

#endif

RAZIX_API void BeginMarker(const std::string& name, float4 color);
RAZIX_API void InsertMarker(const std::string& name, float4 color);
RAZIX_API void EndMarker();

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