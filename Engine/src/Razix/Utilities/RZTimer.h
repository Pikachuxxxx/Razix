#pragma once

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <windows.h>
typedef LARGE_INTEGER TimeStamp;
#else
    #include <chrono>
typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimeStamp;
#endif

namespace Razix {
    class RAZIX_API RZTimer : public RZRoot
    {
    public:
        RZTimer();
        ~RZTimer() = default;

        f32            GetTimeMS();
        static TimeStamp Now();

        static d32 Duration(TimeStamp start, TimeStamp end, d32 timeResolution = 1.0);
        static f32  Duration(TimeStamp start, TimeStamp end, f32 timeResolution);

        f32 GetElapsedMS()
        {
            return GetElapsed(1000.0f);
        }

        f32 GetElapsedS()
        {
            return GetElapsed(1.0f);
        }

        f32 GetElapsed(const f32 timeResolution) const
        {
            return Duration(m_Start, Now(), timeResolution);
        }

        d32 GetElapsed(const d32 timeResolution = 1.0) const
        {
            return Duration(m_Start, Now(), timeResolution);
        }

    private:
        TimeStamp m_Start;        //Start of timer
        TimeStamp m_Frequency;    //Ticks Per Second
        TimeStamp m_LastTime;     //Last time GetTimedMS was called
    };
}    // namespace Razix
