#ifndef _RZ_SPINLOCK_H_
#define _RZ_SPINLOCK_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/std/atomics.h"

RAZIX_ALIGN_AS(RAZIX_CACHE_LINE_SIZE) typedef struct RZSpinLock{
    RZAtomicU32 flag;
    char        _pad[RAZIX_CACHE_LINE_SIZE - sizeof(RZAtomicU32)];
    // Pad and light to CACHE_LINE_SIZE to avoid false sharing b/w threads contention
} RZSpinLock;

void rz_spinlock_lock(RZSpinLock* spinlock);
bool rz_spinlock_try_lock(RZSpinLock* spinlock);
void rz_spinlock_unlock(RZSpinLock* spinlock);

#endif    // _RZ_SPINLOCK_H_
