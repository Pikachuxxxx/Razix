#ifndef _RZ_SPINLOCK_H_
#define _RZ_SPINLOCK_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/std/RZatomics.h"

namespace Razix {

    class alignas(RAZIX_CACHE_LINE_SIZE) RAZIX_API RZSpinLock
    {
    public:
        RZSpinLock() 
            : m_Flag(0) {}
        ~RZSpinLock() = default;
        // Mutexes and locking primitves cannot be copied/moved
        // Doing so may cause false impression and dangling zmobies which will mess things up
        RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZSpinLock);

        void lock();
        bool try_lock();
        void unlock();

    private:
        RZAtomicU32 m_Flag = false;
        char        _pad[RAZIX_CACHE_LINE_SIZE - sizeof(RZAtomicU32)];
        // Pad and light to CACHE_LINE_SIZE to avoid false sharing b/w threads contention
        static_assert(sizeof(RZAtomicU32) <= RAZIX_CACHE_LINE_SIZE,
            "RZAtomicU32 cannot be larger than cache line size");
    };

    class alignas(RAZIX_CACHE_LINE_SIZE) RAZIX_API RZSpinLockScoped
    {
    public:
        RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZSpinLockScoped);

        explicit RZSpinLockScoped(RZSpinLock& lock);
        ~RZSpinLockScoped();

    private:
        RZSpinLock& m_Lock;
    };

}    // namespace Razix

#endif // _RZ_SPINLOCK_H_
