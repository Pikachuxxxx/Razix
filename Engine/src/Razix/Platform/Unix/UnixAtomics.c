#include "Razix/Core/RZCore.h"
#include "Razix/Core/std/atomics.h"

#ifdef RAZIX_PLATFORM_UNIX

static inline int rz_map_memorder(rz_memory_order order)
{
    switch (order) {
        case RZ_MEMORY_ORDER_RELAXED: return __ATOMIC_RELAXED;
        case RZ_MEMORY_ORDER_ACQUIRE: return __ATOMIC_ACQUIRE;
        case RZ_MEMORY_ORDER_RELEASE: return __ATOMIC_RELEASE;
        case RZ_MEMORY_ORDER_ACQ_REL: return __ATOMIC_ACQ_REL;
        case RZ_MEMORY_ORDER_SEQ_CST: return __ATOMIC_SEQ_CST;
        default: return __ATOMIC_SEQ_CST;
    }
}

void rz_atomic_full_barrier(void)
{
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
}
void rz_atomic_acquire_barrier(void)
{
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
void rz_atomic_release_barrier(void)
{
    __atomic_thread_fence(__ATOMIC_RELEASE);
}

// 32-bit
uint32_t rz_atomic32_load(rz_atomic_u32* atom, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_load_n((uint32_t*) atom, mo);
}

void rz_atomic32_store(rz_atomic_u32* atom, uint32_t value, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    __atomic_store_n((uint32_t*) atom, value, mo);
}

uint32_t rz_atomic32_add(rz_atomic_u32* atom, uint32_t value, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_add_fetch((uint32_t*) atom, value, mo);
}

uint32_t rz_atomic32_sub(rz_atomic_u32* atom, uint32_t value, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_sub_fetch((uint32_t*) atom, value, mo);
}

uint32_t rz_atomic32_increment(rz_atomic_u32* atom, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_add_fetch((uint32_t*) atom, 1, mo);
}

uint32_t rz_atomic32_decrement(rz_atomic_u32* atom, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_sub_fetch((uint32_t*) atom, 1, mo);
}

bool rz_atomic32_cas(rz_atomic_u32* atom, uint32_t expected, uint32_t desired, rz_memory_order order)
{
    int succ, fail;
    switch (order) {
        case RZ_MEMORY_ORDER_RELAXED:
            succ = __ATOMIC_RELAXED;
            fail = __ATOMIC_RELAXED;
            break;
        case RZ_MEMORY_ORDER_ACQUIRE:
            succ = __ATOMIC_ACQUIRE;
            fail = __ATOMIC_RELAXED;
            break;
        case RZ_MEMORY_ORDER_RELEASE:
            succ = __ATOMIC_RELEASE;
            fail = __ATOMIC_RELAXED;
            break;
        case RZ_MEMORY_ORDER_ACQ_REL:
            succ = __ATOMIC_ACQ_REL;
            fail = __ATOMIC_ACQUIRE;
            break;
        case RZ_MEMORY_ORDER_SEQ_CST:
            succ = __ATOMIC_SEQ_CST;
            fail = __ATOMIC_SEQ_CST;
            break;
        default:
            succ = __ATOMIC_SEQ_CST;
            fail = __ATOMIC_SEQ_CST;
            break;
    }

    uint32_t exp = expected;
    return __atomic_compare_exchange_n((uint32_t*) atom, &exp, desired, false, succ, fail);
}

uint32_t rz_atomic32_exchange(rz_atomic_u32* atom, uint32_t desired, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_exchange_n((uint32_t*) atom, desired, mo);
}

// 64-bit
uint64_t rz_atomic64_load(rz_atomic_u64* atom, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_load_n((uint64_t*) atom, mo);
}

void rz_atomic64_store(rz_atomic_u64* atom, uint64_t value, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    __atomic_store_n((uint64_t*) atom, value, mo);
}

uint64_t rz_atomic64_add(rz_atomic_u64* atom, uint64_t value, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_add_fetch((uint64_t*) atom, value, mo);
}

uint64_t rz_atomic64_sub(rz_atomic_u64* atom, uint64_t value, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_sub_fetch((uint64_t*) atom, value, mo);
}

uint64_t rz_atomic64_increment(rz_atomic_u64* atom, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_add_fetch((uint64_t*) atom, 1, mo);
}

uint64_t rz_atomic64_decrement(rz_atomic_u64* atom, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_sub_fetch((uint64_t*) atom, 1, mo);
}

bool rz_atomic64_cas(rz_atomic_u64* atom, uint64_t expected, uint64_t desired,
    rz_memory_order order)
{
    int succ, fail;
    switch (order) {
        case RZ_MEMORY_ORDER_RELAXED:
            succ = __ATOMIC_RELAXED;
            fail = __ATOMIC_RELAXED;
            break;
        case RZ_MEMORY_ORDER_ACQUIRE:
            succ = __ATOMIC_ACQUIRE;
            fail = __ATOMIC_RELAXED;
            break;
        case RZ_MEMORY_ORDER_RELEASE:
            succ = __ATOMIC_RELEASE;
            fail = __ATOMIC_RELAXED;
            break;
        case RZ_MEMORY_ORDER_ACQ_REL:
            succ = __ATOMIC_ACQ_REL;
            fail = __ATOMIC_ACQUIRE;
            break;
        case RZ_MEMORY_ORDER_SEQ_CST:
            succ = __ATOMIC_SEQ_CST;
            fail = __ATOMIC_SEQ_CST;
            break;
        default:
            succ = __ATOMIC_SEQ_CST;
            fail = __ATOMIC_SEQ_CST;
            break;
    }

    uint64_t exp = expected;
    return __atomic_compare_exchange_n((uint64_t*) atom, &exp, desired, false, succ, fail);
}

uint64_t rz_atomic64_exchange(rz_atomic_u64* atom, uint64_t desired, rz_memory_order order)
{
    int mo = rz_map_memorder(order);
    return __atomic_exchange_n((uint64_t*) atom, desired, mo);
}

// pointer atomics
void* rz_atomic_ptr_load(rz_atomic_uptr* atom, rz_memory_order order)
{
    uint64_t v = rz_atomic64_load((rz_atomic_u64*) atom, order);
    return (void*) (uintptr_t) v;
}

void rz_atomic_ptr_store(rz_atomic_uptr* atom, void* value, rz_memory_order order)
{
    rz_atomic64_store((rz_atomic_u64*) atom, (uint64_t) (uintptr_t) value, order);
}

void* rz_atomic_ptr_exchange(rz_atomic_uptr* atom, void* desired, rz_memory_order order)
{
    uint64_t old = rz_atomic64_exchange((rz_atomic_u64*) atom, (uint64_t) (uintptr_t) desired, order);
    return (void*) (uintptr_t) old;
}

bool rz_atomic_ptr_cas(rz_atomic_uptr* atom, void* expected, void* desired, rz_memory_order order)
{
    return rz_atomic64_cas((rz_atomic_u64*) atom, (uint64_t) (uintptr_t) expected, (uint64_t) (uintptr_t) desired, order);
}
#endif    // RAZIX_PLATFORM_UNIX
