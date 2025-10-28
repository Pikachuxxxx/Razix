#pragma once

#include "Razix/Core/RZCore.h"

namespace Razix {

    class RAZIX_API RZTimestep
    {
    public:
        RZTimestep(f32 initialTime = 0.0f)
            : m_Timestep(0.0f), m_LastTime(initialTime), m_Elapsed(0.0f), m_FrameCount(0), m_FPS(0.0f), m_TotalTime(0.0f), m_AvgFPS(0.0f) {}

        inline void Update(f32 currentTime)
        {
            m_Timestep = currentTime - m_LastTime;
            m_LastTime = currentTime;
            m_Elapsed += m_Timestep;
            m_TotalTime += m_Timestep;

            m_FrameCount++;

            if (m_Timestep > 0.0f) {
                m_FPS = 1.0f / m_Timestep;
            }

            if (m_TotalTime > 0.0f) {
                m_AvgFPS = static_cast<f32>(m_FrameCount) / m_TotalTime;
            }
        }

        inline f32 GetTimestepMs() const { return m_Timestep * 1000.0f; }
        inline f32 GetElapsedMs() const { return m_Elapsed * 1000.0f; }
        inline f32 GetTimestepSeconds() const { return m_Timestep; }
        inline f32 GetElapsedSeconds() const { return m_Elapsed; }

        inline f32 GetCurrentFPS() const { return m_FPS; }
        inline f32 GetAverageFPS() const { return m_AvgFPS; }
        inline u32 GetFrameCount() const { return m_FrameCount; }

    private:
        f32  m_Timestep  = 0.0f;
        f32  m_LastTime  = 0.0f;
        f32  m_Elapsed   = 0.0f;
        f32  m_TotalTime = 0.0f;
        f32  m_FPS       = 0.0f;
        f32  m_AvgFPS;
        u32  m_FrameCount = 0;
        bool _padding[4]  = {0};
    };
}    // namespace Razix
