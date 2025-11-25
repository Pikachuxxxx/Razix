#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Core/Utils/RZTime.h"

namespace Razix {

    class RAZIX_API RZTimestep
    {
    public:
        RZTimestep(f32 initialTime = 0.0f)
            : m_LastTime(rz_time_now()), m_TimestepMs(0.0f), m_FrameCount(0), m_FPS(0.0f), m_TotalTimeSec(0.0f), m_AvgFPS(0.0f) {}

        inline void Update(rz_time_stamp currentTime)
        {
            m_TimestepMs = static_cast<f32>(rz_get_elapsed_ms(m_LastTime, currentTime));
            m_LastTime   = currentTime;
            m_TotalTimeSec += (m_TimestepMs / 1000.0f);

            m_FrameCount++;

            if (m_TimestepMs > 0.0f) {
                m_FPS = 1000.0f / m_TimestepMs;
            }

            m_AvgFPS = m_TotalTimeSec > 0.0f ? (m_FrameCount) / m_TotalTimeSec : 0.0f;
        }

        inline f32 GetTimestepMs() const { return m_TimestepMs; }
        inline f32 GetElapsedMs() const { return (m_TotalTimeSec * 1000.0f); }
        inline f32 GetTimestepSeconds() const { return m_TimestepMs * 1000.0f; }
        inline f32 GetElapsedSeconds() const { return m_TotalTimeSec; }

        inline f32 GetCurrentFPS() const { return m_FPS; }
        inline f32 GetAverageFPS() const { return m_AvgFPS; }
        inline u32 GetFrameCount() const { return m_FrameCount; }

    private:
        rz_time_stamp m_LastTime     = {};
        f32           m_TimestepMs   = 0.0f;
        f32           m_TotalTimeSec = 0.0f;
        f32           m_FPS          = 0.0f;
        f32           m_AvgFPS       = 0.0f;
        u32           m_FrameCount   = 0;
    };
}    // namespace Razix
