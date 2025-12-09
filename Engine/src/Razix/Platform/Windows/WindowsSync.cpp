// clang-format off
#include "rzxpch.h"
// clang-format on

#include "Razix/Core/std/sync.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <windows.h>

namespace Razix {

    void rz_critical_section::init(u32 spinCount)
    {
        m_SpinCount = spinCount;
        bool ret    = InitializeCriticalSectionAndSpinCount(&m_CS, m_SpinCount);
        RAZIX_CORE_ASSERT(ret, "Failed to initialize Critical Section with spintcount!");
    }

    void rz_critical_section::destroy()
    {
        DeleteCriticalSection(&m_CS);
    }

    void rz_critical_section::lock()
    {
        EnterCriticalSection(&m_CS);
    }

    bool rz_critical_section::try_lock()
    {
        return TryEnterCriticalSection(&m_CS) != 0;
    }

    void rz_critical_section::unlock()
    {
        LeaveCriticalSection(&m_CS);
    }

    //---------------------------------------------------------------------------
    void rz_cond_var::init()
    {
        InitializeConditionVariable(&m_CV);
    }

    void rz_cond_var::destroy()
    {
        // No-op on Windows
    }

    void rz_cond_var::signal()
    {
        WakeConditionVariable(&m_CV);
    }

    void rz_cond_var::broadcast()
    {
        WakeAllConditionVariable(&m_CV);
    }

    void rz_cond_var::wait(rz_critical_section* cs)
    {
        SleepConditionVariableCS(&m_CV, &cs->m_CS, INFINITE);
    }

    void rz_cond_var::wait(rz_critical_section* cs, u32 timeout_ms)
    {
        SleepConditionVariableCS(&m_CV, &cs->m_CS, (DWORD) timeout_ms);
    }
}    // namespace Razix
#endif