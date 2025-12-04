// clang-format off
#include "rzxpch.h"
// clang-format on

#include "Razix/Core/std/sync.h"

#ifdef RAZIX_PLATFORM_UNIX

    #include <pthread.h>
    #include <errno.h>
    #include <time.h>

namespace Razix {

    void RZCriticalSection::init(u32 spinCount)
    {
        m_SpinCount = spinCount;

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);

        pthread_mutex_init(&m_CS, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    void RZCriticalSection::destroy()
    {
        i32 err = pthread_mutex_destroy(&m_CS);
        RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to destroy critical section! | ERROR_CODE: {}", err);
    }

    void RZCriticalSection::lock()
    {
        for (uint32_t i = 0; i < m_SpinCount; ++i) {
            if (!pthread_mutex_trylock(&m_CS))
                return;
            // spin on it and just try to lock before falling back to OS primitive
            RAZIX_BUSY_WAIT();
        }

        pthread_mutex_lock(&m_CS);
    }

    bool RZCriticalSection::try_lock()
    {
        return !pthread_mutex_trylock(&m_CS);
    }

    void RZCriticalSection::unlock()
    {
        pthread_mutex_unlock(&m_CS);
    }

    //-----------------------------------------------------------------------------

    void RZConditionalVar::init()
    {
        pthread_condattr_t attr;
        i32                err = pthread_condattr_init(&attr);
        RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to initialize conditional variable attributes! | ERROR_CODE: {}", err);

        err = pthread_cond_init(&m_CV, &attr);
        RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to initialize conditional variable! | ERROR_CODE: {} ", err);

        err = pthread_condattr_destroy(&attr);
        RAZIX_CORE_ASSERT(err == 0, "[PosixThread] [Init Successful] Failed to destroy conditional variable attributes! | ERROR_CODE: {}", err);
    }

    void RZConditionalVar::destroy()
    {
        i32 err = pthread_cond_destroy(&m_CV);
        RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to destroy conditional variable! | ERROR_CODE: {}", err);
    }
    
    void RZConditionalVar::signal()
    {
        /**
         * [Source]: https://man7.org/linux//man-pages/man3/pthread_cond_init.3.html
         * restarts one of the threads that are waiting
         * on the condition variable cond.  If no threads are waiting on
         * cond, nothing happens.  If several threads are waiting on cond,
         * exactly one is restarted, but it is not specified which.
         */
        i32 err = pthread_cond_signal(&m_CV);
        RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to signal conditional variable! | ERROR_CODE: {} ", err);
    }
    
    void RZConditionalVar::broadcast()
    {
        i32 err = pthread_cond_broadcast(&m_CV);
        RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to broadcast conditional variable! | ERROR_CODE: {} ", err);
    }

    void RZConditionalVar::wait(RZCriticalSection* cs)
    {
        // TODO: Check if the calling thread has the lock to the mutex being passed, track is locked, and parent info if possible or atleast the flag.
        i32 err = pthread_cond_wait(&m_CV, &cs->m_CS);
        RAZIX_CORE_ASSERT(err == 0, "[PosixThread] Failed to wait on conditional variable. The mutex must be locked by the calling thread on entrance to pthread_cond_wait(). | ERROR_CODE: {} ", err);    
    }

    void RZConditionalVar::wait(RZCriticalSection* cs, u32 timeout_ms)
    {

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        ts.tv_sec  += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000ULL;

        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }

        // TODO: Check if the calling thread has the lock to the mutex being passed, track is locked, and parent info if possible or atleast the flag.
        i32 err = pthread_cond_timedwait(&m_CV, &cs->m_CS, &ts);
        if (err != 0) {
            RAZIX_CORE_ASSERT(err == ETIMEDOUT, "[PosixThread] Failed to wait on conditional variable. The mutex must be locked by the calling thread on entrance to pthread_cond_wait(). | ERROR_CODE: {} ", err);
        }
    }

}    // namespace Razix

#endif    // RAZIX_PLATFORM_UNIX
