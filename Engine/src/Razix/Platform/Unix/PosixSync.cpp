#include "Razix/Core/std/sync.h"

#ifdef RAZIX_PLATFORM_UNIX

    #include <pthread.h>

//---------------------------------------------------------------------------

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
        pthread_mutex_destroy(&m_CS);
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

}    // namespace Razix

#endif    // RAZIX_PLATFORM_UNIX
