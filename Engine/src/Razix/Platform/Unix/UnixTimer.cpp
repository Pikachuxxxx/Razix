// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Utilities/RZTimer.h"

namespace Razix {
   
    TimeStamp RZTimer::Now()
    {
        // Get current time using the high_resolution_clock
        return std::chrono::high_resolution_clock::now();
    }

    d32 RZTimer::Duration(TimeStamp start, TimeStamp end, d32 timeResolution)
    {
        // Get duration between two time points and convert to seconds
        auto duration = std::chrono::duration_cast<std::chrono::duration<d32>>(end - start);
        return duration.count() / timeResolution;
    }

    f32 RZTimer::Duration(TimeStamp start, TimeStamp end, f32 timeResolution)
    {
        // Get duration between two time points and convert to seconds (for float resolution)
        auto duration = std::chrono::duration_cast<std::chrono::duration<f32>>(end - start);
        return duration.count() / timeResolution;
    }
} // namespace Razix
