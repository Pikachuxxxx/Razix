#ifndef _RZ_THREAD_CORE_H
#define _RZ_THREAD_CORE_H

#include "Razix/Core/RZCore.h"

#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/spinlock.h"
#include "Razix/Core/std/sync.h"
#include "Razix/Core/std/thread.h"

namespace Razix {

    template<class T>
    class alignas(RAZIX_CACHE_LINE_SIZE) RZScopedLock
    {
    public:
        // Mutexes and locking primitves cannot be copied/moved
        // Doing so may cause false impression and dangling zombies which will mess things up
        RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZScopedLock);

        explicit inline RZScopedLock(T* cs)
            : m_pLock(cs)
        {
            cs->lock();
        }

        explicit inline RZScopedLock(T& cs)
            : m_pLock(&cs)
        {
            cs.lock();
        }

        inline void unlock()
        {
            if (m_pLock) {
                m_pLock->unlock();
                m_pLock = NULL;
            }
        }
        inline ~RZScopedLock()
        {
            unlock();
        }

    private:
        T* m_pLock;
        char        _pad[RAZIX_CACHE_LINE_SIZE - sizeof(T)];
        // Pad and light to CACHE_LINE_SIZE to avoid false sharing b/w threads contention
        static_assert(sizeof(RZAtomicU32) <= RAZIX_CACHE_LINE_SIZE,
            "RZScopedLock::T cannot be larger than cache line size");

    };

    typedef RZScopedLock<RZSpinLock> RZScopedSpinLock;
    typedef RZScopedLock<RZCriticalSection> RZScopedCriticalSection;

}    // namespace Razix

#endif    // _RZ_THREAD_CORE_H
