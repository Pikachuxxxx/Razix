#include "spinlock.h"

void rz_spinlock_lock(RZSpinLock* spinlock)
{
    // acquire lock then value is 1 and return 0 and we exit, to execute the code
    // when it's 1 other threads wait, and once we are we restore it to 0, this is when
    // other who are waiting can acquire the lock and continue their work
    while (true) {
        if (!rz_atomic32_exchange(&spinlock->flag, 1u, RZ_MEMORY_ORDER_ACQUIRE))
            return;
        // try_lock failed immediately we spink until lock is release and it's 0 and
        while (rz_atomic32_load(&spinlock->flag, RZ_MEMORY_ORDER_RELAXED))
            RAZIX_BUSY_WAIT();
    }
}

bool rz_spinlock_try_lock(RZSpinLock* spinlock)
{
    return !rz_atomic32_exchange(&spinlock->flag, 1u, RZ_MEMORY_ORDER_ACQUIRE);
}

void rz_spinlock_unlock(RZSpinLock* spinlock)
{
    rz_atomic32_store(&spinlock->flag, 0u, RZ_MEMORY_ORDER_RELEASE);
}
