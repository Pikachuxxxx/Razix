#include "Razix/Core/std/sync.h"

#include "Razix/Core/Log/RZLog.h"    // For RAZIX_CORE_ASSERT

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <windows.h>

rz_critical_section rz_critical_section_create(void)
{
    return rz_critical_section_create_ex(RAZIX_CS_DEF_SPIN_CNT);
}

rz_critical_section rz_critical_section_create_ex(u32 spintCount)
{
    rz_critical_section cs;
    cs.m_SpinCount = spintCount;
    bool ret       = InitializeCriticalSectionAndSpinCount((CRITICAL_SECTION*) &cs.m_Internal.buffer, cs.m_SpinCount);
    RAZIX_CORE_ASSERT(ret, "Failed to initialize Critical Section with spintcount!");
    RAZIX_UNUSED(ret);

    return cs;
}

void rz_critical_section_destroy(rz_critical_section* cs)
{
    DeleteCriticalSection((CRITICAL_SECTION*) &cs->m_Internal.buffer);
}

void rz_critical_section_lock(rz_critical_section* cs)
{
    EnterCriticalSection((CRITICAL_SECTION*) &cs->m_Internal.buffer);
}

bool rz_critical_section_try_lock(rz_critical_section* cs)
{
    return TryEnterCriticalSection((CRITICAL_SECTION*) &cs->m_Internal.buffer) != 0;
}

void rz_critical_section_unlock(rz_critical_section* cs)
{
    LeaveCriticalSection((CRITICAL_SECTION*) &cs->m_Internal.buffer);
}

//-----------------------------------------------------------------------------

rz_cond_var rz_conditional_var_create(void)
{
    rz_cond_var cv;
    InitializeConditionVariable((CONDITION_VARIABLE*) &cv.m_Internal.buffer);
    return cv;
}

void rz_conditional_var_destroy(rz_cond_var* cv)
{
    // No-op on Windows
}

void rz_conditional_var_signal(rz_cond_var* cv)
{
    WakeConditionVariable((CONDITION_VARIABLE*) &cv->m_Internal.buffer);
}

void rz_conditional_var_broadcast(rz_cond_var* cv)
{
    WakeAllConditionVariable((CONDITION_VARIABLE*) &cv->m_Internal.buffer);
}

void rz_conditional_var_wait(rz_cond_var* cv, rz_critical_section* cs)
{
    SleepConditionVariableCS((CONDITION_VARIABLE*) &cv->m_Internal.buffer, (CRITICAL_SECTION*) &cs->m_Internal.buffer, INFINITE);
}

void rz_conditional_var_wait_until(rz_cond_var* cv, rz_critical_section* cs, u32 timeout_ms)
{
    SleepConditionVariableCS((CONDITION_VARIABLE*) &cv->m_Internal.buffer, (CRITICAL_SECTION*) &cs->m_Internal.buffer, (DWORD) timeout_ms);
}
#endif