#include "Razix/Core/std/sync.h"

#include "Razix/Core/Log/RZLog.h"    // For RAZIX_CORE_ASSERT

#ifdef RAZIX_PLATFORM_UNIX

    #include <errno.h>
    #include <pthread.h>
    #include <time.h>

RZCriticalSection rz_critical_section_create(void)
{
    return rz_critical_section_create_ex(RAZIX_CS_DEF_SPIN_CNT);
}

RZCriticalSection rz_critical_section_create_ex(u32 spinCount)
{
    RZCriticalSection cs;
    cs.m_SpinCount = spinCount;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    pthread_mutex_t* nativeMutex = (pthread_mutex_t*) cs.m_Internal.buffer;
    pthread_mutex_init(nativeMutex, &attr);
    pthread_mutexattr_destroy(&attr);

    return cs;
}

void rz_critical_section_destroy(RZCriticalSection* cs)
{
    i32 err = pthread_mutex_destroy((pthread_mutex_t*) cs->m_Internal.buffer);
    RAZIX_UNUSED(err);
    RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to destroy critical section! | ERROR_CODE: {}", err);
}

void rz_critical_section_lock(RZCriticalSection* cs)
{
    pthread_mutex_t* nativeMutex = (pthread_mutex_t*) cs->m_Internal.buffer;

    for (uint32_t i = 0; i < cs->m_SpinCount; ++i) {
        if (!pthread_mutex_trylock(nativeMutex))
            return;
        // spin on it and just try to lock before falling back to OS primitive
        RAZIX_BUSY_WAIT();
    }

    pthread_mutex_lock(nativeMutex);
}

bool rz_critical_section_try_lock(RZCriticalSection* cs)
{
    return !pthread_mutex_trylock((pthread_mutex_t*) cs->m_Internal.buffer);
}

void rz_critical_section_unlock(RZCriticalSection* cs)
{
    pthread_mutex_unlock((pthread_mutex_t*) cs->m_Internal.buffer);
}

//-----------------------------------------------------------------------------

RZConditionalVar rz_conditional_var_init(void)
{
    RZConditionalVar cv;

    pthread_condattr_t attr;
    i32                err = pthread_condattr_init(&attr);
    RAZIX_UNUSED(err);
    RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to initialize conditional variable attributes! | ERROR_CODE: {}", err);

    err = pthread_cond_init((pthread_cond_t*) cv.m_Internal.buffer, &attr);
    RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to initialize conditional variable! | ERROR_CODE: {} ", err);

    err = pthread_condattr_destroy(&attr);
    RAZIX_CORE_ASSERT(err == 0, "[PosixThread] [Init Successful] Failed to destroy conditional variable attributes! | ERROR_CODE: {}", err);

    return cv;
}

void rz_conditional_var_destroy(RZConditionalVar* cv)
{
    i32 err = pthread_cond_destroy((pthread_cond_t*) cv->m_Internal.buffer);
    RAZIX_UNUSED(err);
    RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to destroy conditional variable! | ERROR_CODE: {}", err);
}

void rz_conditional_var_signal(RZConditionalVar* cv)
{
    /**
     * [Source]: https://man7.org/linux//man-pages/man3/pthread_cond_init.3.html
     * restarts one of the threads that are waiting
     * on the condition variable cond.  If no threads are waiting on
     * cond, nothing happens.  If several threads are waiting on cond,
     * exactly one is restarted, but it is not specified which.
     */
    i32 err = pthread_cond_signal((pthread_cond_t*) cv->m_Internal.buffer);
    RAZIX_UNUSED(err);
    RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to signal conditional variable! | ERROR_CODE: {} ", err);
}

void rz_conditional_var_broadcast(RZConditionalVar* cv)
{
    i32 err = pthread_cond_broadcast((pthread_cond_t*) cv->m_Internal.buffer);
    RAZIX_UNUSED(err);
    RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to broadcast conditional variable! | ERROR_CODE: {} ", err);
}

void rz_conditional_var_wait(RZConditionalVar* cv, RZCriticalSection* cs)
{
    // TODO: Check if the calling thread has the lock to the mutex being passed, track is locked, and parent info if possible or atleast the flag.
    i32 err = pthread_cond_wait((pthread_cond_t*) cv->m_Internal.buffer, (pthread_mutex_t*) cs->m_Internal.buffer);
    RAZIX_UNUSED(err);
    RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to wait on conditional variable. The mutex must be locked by the calling thread on entrance to pthread_cond_wait(). | ERROR_CODE: {} ", err);
}

void rz_conditional_var_wait_until(RZConditionalVar* cv, RZCriticalSection* cs, u32 timeout_ms)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000ULL;

    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }

    // TODO: Check if the calling thread has the lock to the mutex being passed, track is locked, and parent info if possible or atleast the flag.
    i32 err = pthread_cond_timedwait((pthread_cond_t*) cv->m_Internal.buffer, (pthread_mutex_t*) cs->m_Internal.buffer, &ts);
    if (err != 0) {
        RAZIX_CORE_ASSERT(err == ETIMEDOUT, "[PosixThread] Failed to wait on conditional variable. The mutex must be locked by the calling thread on entrance to pthread_cond_wait(). | ERROR_CODE: {} ", err);
    }
}

#endif    // RAZIX_PLATFORM_UNIX
