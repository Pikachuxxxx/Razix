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
        while (true) {
            if (!rz_atomic32_exchange(&m_Flag, 1u, RZ_MEMORY_ORDER_ACQUIRE))
                return;
            // try_lock failed immediately we spink until lock is release and it's 0 and  
            while (rz_atomic32_load(&m_Flag, RZ_MEMORY_ORDER_RELAXED))
                RAZIX_BUSY_WAIT();
        } 
    }

    bool RZSpinLock::try_lock()
    {
        return !rz_atomic32_exchange(&m_Flag, 1u, RZ_MEMORY_ORDER_ACQUIRE);
    }

    void RZSpinLock::unlock()
    {
        rz_atomic32_store(&m_Flag, 0u, RZ_MEMORY_ORDER_RELEASE);
    }
}    // namespace Razix
