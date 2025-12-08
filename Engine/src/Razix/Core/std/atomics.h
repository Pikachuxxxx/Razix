#ifndef _RZ_ATOMICS_H_
#define _RZ_ATOMICS_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

    // Memory ordering
    typedef enum RZMemoryOrder
    {
        RZ_MEMORY_ORDER_RELAXED,    // No synchronization
        RZ_MEMORY_ORDER_ACQUIRE,    // Load barrier, don't move operations after this barrier
        RZ_MEMORY_ORDER_RELEASE,    // Store barrier, don't move operations prior to this barrier
        RZ_MEMORY_ORDER_ACQ_REL,    // Load + Store barrier
        RZ_MEMORY_ORDER_SEQ_CST,    // Sequential consistency, ACQ_REL + more safety
    } RZMemoryOrder;

    // Atomics Types (only unsigned supported for now)
    typedef volatile u32       RZAtomicU32;
    typedef volatile u64       RZAtomicU64;
    typedef volatile uintptr_t RZAtomicUPtr;

    //----------------------------------------------------------------
    // API
    //----------------------------------------------------------------
    // Barriers
    RAZIX_API void rz_atomic_full_barrier(void);
    RAZIX_API void rz_atomic_acquire_barrier(void);
    RAZIX_API void rz_atomic_release_barrier(void);
    //----------------------------------------------------------------
    // 32-bit
    RAZIX_API uint32_t rz_atomic32_load(RZAtomicU32* atom, RZMemoryOrder order);
    RAZIX_API void     rz_atomic32_store(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order);
    RAZIX_API uint32_t rz_atomic32_add(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order);
    RAZIX_API uint32_t rz_atomic32_sub(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order);
    RAZIX_API uint32_t rz_atomic32_increment(RZAtomicU32* atom, RZMemoryOrder order);
    RAZIX_API uint32_t rz_atomic32_decrement(RZAtomicU32* atom, RZMemoryOrder order);
    RAZIX_API bool     rz_atomic32_cas(RZAtomicU32* atom, uint32_t expected, uint32_t desired, RZMemoryOrder order);
    RAZIX_API uint32_t rz_atomic32_exchange(RZAtomicU32* atom, uint32_t desired, RZMemoryOrder order);
    //----------------------------------------------------------------
    // 64-bit
    RAZIX_API uint64_t rz_atomic64_load(RZAtomicU64* atom, RZMemoryOrder order);
    RAZIX_API void     rz_atomic64_store(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order);
    RAZIX_API uint64_t rz_atomic64_add(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order);
    RAZIX_API uint64_t rz_atomic64_sub(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order);
    RAZIX_API uint64_t rz_atomic64_increment(RZAtomicU64* atom, RZMemoryOrder order);
    RAZIX_API uint64_t rz_atomic64_decrement(RZAtomicU64* atom, RZMemoryOrder order);
    RAZIX_API bool     rz_atomic64_cas(RZAtomicU64* atom, uint64_t expected, uint64_t desired, RZMemoryOrder order);
    RAZIX_API uint64_t rz_atomic64_exchange(RZAtomicU64* atom, uint64_t desired, RZMemoryOrder order);
    //----------------------------------------------------------------
    // ptr
    RAZIX_API void* rz_atomic_ptr_load(RZAtomicUPtr* atom, RZMemoryOrder order);
    RAZIX_API void  rz_atomic_ptr_store(RZAtomicUPtr* atom, void* value, RZMemoryOrder order);
    RAZIX_API void* rz_atomic_ptr_exchange(RZAtomicUPtr* atom, void* desired, RZMemoryOrder order);
    RAZIX_API bool  rz_atomic_ptr_cas(RZAtomicUPtr* atom, void* expected, void* desired, RZMemoryOrder order);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif    // _RZ_ATOMICS_H_
