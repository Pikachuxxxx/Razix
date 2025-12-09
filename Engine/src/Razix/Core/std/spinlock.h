#ifndef _RZ_SPINLOCK_H_
#define _RZ_SPINLOCK_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/std/atomics.h"

RAZIX_ALIGN_AS(RAZIX_CACHE_LINE_SIZE)
typedef struct RZSpinLock
{
    rz_atomic_u32 flag;
    char          _pad[RAZIX_CACHE_LINE_SIZE - sizeof(rz_atomic_u32)];
    // Pad and light to CACHE_LINE_SIZE to avoid false sharing b/w threads contention
} RZSpinLock;

#ifdef __cplusplus
extern "C"
{
#endif

    RAZIX_API void rz_spinlock_lock(RZSpinLock* spinlock);
    RAZIX_API bool rz_spinlock_try_lock(RZSpinLock* spinlock);
    RAZIX_API void rz_spinlock_unlock(RZSpinLock* spinlock);

#ifdef __cplusplus
}
#endif

#endif    // _RZ_SPINLOCK_H_
