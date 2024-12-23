#pragma once

#include "Razix/Core/RZCore.h"

namespace Razix {
    /// <summary>
    /// Manages the delta time of the frame
    /// </summary>
    class RAZIX_API RZTimestep
    {
    public:
        /// <summary>
        /// Initializes the delta time calculation function
        /// </summary>
        /// <param name="initialTime"> The time at which the runtime loop has started since the beggining of the initialization </param>
        RZTimestep(f32 initialTime = 0.0f)
            : m_Timestep(0.0f), m_LastTime(initialTime), m_Elapsed(0.0f) {}

        /// <summary>
        /// Updates the delta time
        /// </summary>
        /// <param name="currentTime"> current time </param>
        inline void Update(f32 currentTime)
        {
            m_Timestep = currentTime - m_LastTime;
            m_LastTime = currentTime;
            m_Elapsed += m_Timestep;
        }

        /// <summary>
        /// Gets the delta time in milliseconds
        /// </summary>
        inline f32 GetTimestepMs() const { return m_Timestep * 1000.0f; }

        /// <summary>
        /// Gets the elapsed time since last frame in milliseconds
        /// </summary>
        inline f32 GetElapsedMs() const { return m_Elapsed * 1000.0f; }

        /// <summary>
        /// Gets the deltatime in seconds
        /// </summary>
        inline f32 GetTimestepSeconds() const { return m_Timestep; }

        /// <summary>
        /// Gets the elapsed time since last frame in seconds
        /// </summary>
        inline f32 GetElapsedSeconds() const { return m_Elapsed; }

    private:
        /// Time between the last 2 frames, deltatime
        f32 m_Timestep;
        /// Time taken from start when the last frame was rendered
        f32 m_LastTime;
        ///	Time elapsed since the beginning of the first frame
        f32  m_Elapsed;
        bool _padding[4];
    };
}    // namespace Razix
