#ifndef _RZ_SPINLOCK_H_
#define _RZ_SPINLOCK_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/std/atomics.h"

RAZIX_ALIGN_AS(RAZIX_CACHE_LINE_SIZE)
typedef struct rz_spin_lock
{
    rz_atomic_u32 flag;
    char          _pad[RAZIX_CACHE_LINE_SIZE - sizeof(rz_atomic_u32)];
    // Pad and light to CACHE_LINE_SIZE to avoid false sharing b/w threads contention
} rz_spin_lock;

#ifdef __cplusplus
extern "C"
{
#endif

    RAZIX_API void rz_spinlock_lock(rz_spin_lock* spinlock);
    RAZIX_API bool rz_spinlock_try_lock(rz_spin_lock* spinlock);
    RAZIX_API void rz_spinlock_unlock(rz_spin_lock* spinlock);

#ifdef __cplusplus
}
#endif

#endif    // _RZ_SPINLOCK_H_
