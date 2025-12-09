#ifndef _RZ_ATOMICS_H_
#define _RZ_ATOMICS_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

    // Memory ordering
    typedef enum rz_memory_order
    {
        RZ_MEMORY_ORDER_RELAXED,    // No synchronization
        RZ_MEMORY_ORDER_ACQUIRE,    // Load barrier, don't move operations after this barrier
        RZ_MEMORY_ORDER_RELEASE,    // Store barrier, don't move operations prior to this barrier
        RZ_MEMORY_ORDER_ACQ_REL,    // Load + Store barrier
        RZ_MEMORY_ORDER_SEQ_CST,    // Sequential consistency, ACQ_REL + more safety
    } rz_memory_order;

    // Atomics Types (only unsigned supported for now)
    typedef volatile u32       rz_atomic_u32;
    typedef volatile u64       rz_atomic_u64;
    typedef volatile uintptr_t rz_atomic_uptr;

    //----------------------------------------------------------------
    // API
    //----------------------------------------------------------------
    // Barriers
    RAZIX_API void rz_atomic_full_barrier(void);
    RAZIX_API void rz_atomic_acquire_barrier(void);
    RAZIX_API void rz_atomic_release_barrier(void);
    //----------------------------------------------------------------
    // 32-bit
    RAZIX_API uint32_t rz_atomic32_load(rz_atomic_u32* atom, rz_memory_order order);
    RAZIX_API void     rz_atomic32_store(rz_atomic_u32* atom, uint32_t value, rz_memory_order order);
    RAZIX_API uint32_t rz_atomic32_add(rz_atomic_u32* atom, uint32_t value, rz_memory_order order);
    RAZIX_API uint32_t rz_atomic32_sub(rz_atomic_u32* atom, uint32_t value, rz_memory_order order);
    RAZIX_API uint32_t rz_atomic32_increment(rz_atomic_u32* atom, rz_memory_order order);
    RAZIX_API uint32_t rz_atomic32_decrement(rz_atomic_u32* atom, rz_memory_order order);
    RAZIX_API bool     rz_atomic32_cas(rz_atomic_u32* atom, uint32_t expected, uint32_t desired, rz_memory_order order);
    RAZIX_API uint32_t rz_atomic32_exchange(rz_atomic_u32* atom, uint32_t desired, rz_memory_order order);
    //----------------------------------------------------------------
    // 64-bit
    RAZIX_API uint64_t rz_atomic64_load(rz_atomic_u64* atom, rz_memory_order order);
    RAZIX_API void     rz_atomic64_store(rz_atomic_u64* atom, uint64_t value, rz_memory_order order);
    RAZIX_API uint64_t rz_atomic64_add(rz_atomic_u64* atom, uint64_t value, rz_memory_order order);
    RAZIX_API uint64_t rz_atomic64_sub(rz_atomic_u64* atom, uint64_t value, rz_memory_order order);
    RAZIX_API uint64_t rz_atomic64_increment(rz_atomic_u64* atom, rz_memory_order order);
    RAZIX_API uint64_t rz_atomic64_decrement(rz_atomic_u64* atom, rz_memory_order order);
    RAZIX_API bool     rz_atomic64_cas(rz_atomic_u64* atom, uint64_t expected, uint64_t desired, rz_memory_order order);
    RAZIX_API uint64_t rz_atomic64_exchange(rz_atomic_u64* atom, uint64_t desired, rz_memory_order order);
    //----------------------------------------------------------------
    // ptr
    RAZIX_API void* rz_atomic_ptr_load(rz_atomic_uptr* atom, rz_memory_order order);
    RAZIX_API void  rz_atomic_ptr_store(rz_atomic_uptr* atom, void* value, rz_memory_order order);
    RAZIX_API void* rz_atomic_ptr_exchange(rz_atomic_uptr* atom, void* desired, rz_memory_order order);
    RAZIX_API bool  rz_atomic_ptr_cas(rz_atomic_uptr* atom, void* expected, void* desired, rz_memory_order order);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif    // _RZ_ATOMICS_H_
