// clang-format off
#include "rzxpch.h"
// clang-format on

#include "Razix/Core/std/sync.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <windows.h>

namespace Razix {

    void RZCriticalSection::init(u32 spinCount)
    {
        m_SpinCount = spinCount;
        bool ret    = InitializeCriticalSectionAndSpinCount(&m_CS, m_SpinCount);
        RAZIX_CORE_ASSERT(ret, "Failed to initialize Critical Section with spintcount!");
    }

    void RZCriticalSection::destroy()
    {
        DeleteCriticalSection(&m_CS);
    }

    void RZCriticalSection::lock()
    {
        EnterCriticalSection(&m_CS);
    }

    bool RZCriticalSection::try_lock()
    {
        return TryEnterCriticalSection(&m_CS) != 0;
    }

    void RZCriticalSection::unlock()
    {
        LeaveCriticalSection(&m_CS);
    }

    //---------------------------------------------------------------------------
    void RZConditionalVar::init()
    {
        InitializeConditionVariable(&m_CV);
    }

    void RZConditionalVar::destroy()
    {
        // No-op on Windows
    }

    void RZConditionalVar::signal()
    {
        WakeConditionVariable(&m_CV);
    }

    void RZConditionalVar::broadcast()
    {
        WakeAllConditionVariable(&m_CV);
    }

    void RZConditionalVar::wait(RZCriticalSection* cs)
    {
        SleepConditionVariableCS(&m_CV, &cs->m_CS, INFINITE);
    }

    void RZConditionalVar::wait(RZCriticalSection* cs, u32 timeout_ms)
    {
        SleepConditionVariableCS(&m_CV, &cs->m_CS, (DWORD) timeout_ms);
    }
}    // namespace Razix
#endif