#ifndef _RZ_THREAD_CORE_H
#define _RZ_THREAD_CORE_H

#include "Razix/Core/RZCore.h"

#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/spinlock.h"
#include "Razix/Core/std/sync.h"
#include "Razix/Core/std/thread.h"

namespace Razix {

    template<typename T>
    struct LockTraits
    {
        static void lock(T* t) { t->lock(); }
        static void unlock(T* t) { t->unlock(); }
    };

    template<>
    struct LockTraits<RZSpinLock>
    {
        static void lock(RZSpinLock* t) { rz_spinlock_lock(t); }
        static void unlock(RZSpinLock* t) { rz_spinlock_unlock(t); }
    };

    template<>
    struct LockTraits<RZCriticalSection>
    {
        static void lock(RZCriticalSection* t) { rz_critical_section_lock(t); }
        static void unlock(RZCriticalSection* t) { rz_critical_section_unlock(t); }
    };

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
            if (m_pLock) LockTraits<T>::lock(m_pLock);
        }

        explicit inline RZScopedLock(T& cs)
            : m_pLock(&cs)
        {
            if (m_pLock) LockTraits<T>::lock(m_pLock);
        }

        inline void unlock()
        {
            if (m_pLock) {
                LockTraits<T>::unlock(m_pLock);
                m_pLock = nullptr;
            }
        }

        inline ~RZScopedLock()
        {
            unlock();
        }

    private:
        T* m_pLock;
    };

    typedef RZScopedLock<RZSpinLock>        RZScopedSpinLock;
    typedef RZScopedLock<RZCriticalSection> RZScopedCriticalSection;

    static_assert(sizeof(rz_atomic_u32) <= RAZIX_CACHE_LINE_SIZE, "RZSpinLock::rz_atomic_u32 cannot be larger than cache line size");

    static_assert(sizeof(RZCriticalSection) == 2 * RAZIX_CACHE_LINE_SIZE, "RZCriticalSection must be less than 2 cache lines");
    static_assert(alignof(RZCriticalSection) == RAZIX_CACHE_LINE_SIZE, "RZCriticalSection must be cache-line aligned");
    // print size of RZCriticalSection::m_Internal at compile time

    static_assert(sizeof(RZConditionalVar) == RAZIX_CACHE_LINE_SIZE, "RZConditionalVar must be 1 cache lines");
    static_assert(alignof(RZConditionalVar) == RAZIX_CACHE_LINE_SIZE, "RZConditionalVar must be 1 cache-line aligned");

}    // namespace Razix

#endif    // _RZ_THREAD_CORE_H
