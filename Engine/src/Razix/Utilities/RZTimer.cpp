// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTimer.h"

namespace Razix {
    RZTimer::RZTimer()
        : m_Start(Now()), m_Frequency()
    {
        m_LastTime = m_Start;
    }

    f32 RZTimer::GetTimeMS()
    {
        f32 time = Duration(m_LastTime, Now(), 1000.0f);
        m_LastTime = Now();
        return time;
    }
}    // namespace Razix