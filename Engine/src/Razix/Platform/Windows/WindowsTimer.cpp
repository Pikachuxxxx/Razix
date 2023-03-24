// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Utilities/RZTimer.h"

namespace Razix {
    static d32 GetQueryPerformancePeriod()
    {
        LARGE_INTEGER result;
        QueryPerformanceFrequency(&result);
        return 1.0 / static_cast<d32>(result.QuadPart);
    }
    const d32 freq = GetQueryPerformancePeriod();

    TimeStamp RZTimer::Now()
    {
        TimeStamp temp;
        QueryPerformanceCounter(&temp);
        return temp;
    }

    d32 RZTimer::Duration(TimeStamp start, TimeStamp end, d32 timeResolution)
    {
        return (end.QuadPart - start.QuadPart) * timeResolution * freq;
    }

    f32 RZTimer::Duration(TimeStamp start, TimeStamp end, f32 timeResolution)
    {
        return static_cast<f32>((end.QuadPart - start.QuadPart) * timeResolution * freq);
    }
}    // namespace Razix