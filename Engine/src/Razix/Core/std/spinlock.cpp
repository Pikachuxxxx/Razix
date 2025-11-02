// clang-format off
#include "rzxpch.h"
// clang-format on

#include "spinlock.h"

namespace Razix {
    void RZSpinLock::lock()
    {
        // acquire lock then value is 1 and return 0 and we exit, to execute the code
        // when it's 1 other threads wait, and once we are we restore it to 0, this is when
        // other who are waiting can acquire the lock and continue their work
        while (rz_atomic32_exchange(&m_Flag, true, RZ_MEMORY_ORDER_ACQUIRE));

        // TODO: do this once we have a proper cross-platform YIELD
        /*
        while (true) {
        if (!rz_atomic32_exchange(&m_Flag, 1u, RZ_MEMORY_ORDER_ACQUIRE))
            return;
        while (rz_atomic32_load(&m_Flag, RZ_MEMORY_ORDER_RELAXED))
            _mm_pause(); // use a RAZIX_CPU_YIELD macro
        } 
    */
    }

    bool RZSpinLock::try_lock()
    {
        return !rz_atomic32_exchange(&m_Flag, true, RZ_MEMORY_ORDER_ACQUIRE);
    }

    void RZSpinLock::unlock()
    {
        rz_atomic32_store(&m_Flag, false, RZ_MEMORY_ORDER_RELEASE);
    }

    //-----------------------------------------------------------------

    RZSpinLockScoped::RZSpinLockScoped(RZSpinLock& lock)
        : m_Lock(lock)
    {
        m_Lock.lock();
    }

    RZSpinLockScoped::~RZSpinLockScoped()
    {
        m_Lock.unlock();
    }
}    // namespace Razix
