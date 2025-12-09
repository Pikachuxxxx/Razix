#include "Razix/Core/RZCore.h"
#include "Razix/Core/std/atomics.h"

#ifdef RAZIX_PLATFORM_WINDOWS

    #include <intrin.h>
    #include <windows.h>

void rz_atomic_full_barrier(void)
{
    _ReadWriteBarrier();
}
void rz_atomic_acquire_barrier(void)
{
    _ReadBarrier();
}
void rz_atomic_release_barrier(void)
{
    _WriteBarrier();
}

// 32-bit
uint32_t rz_atomic32_load(rz_atomic_u32* atom, rz_memory_order order)
{
    u32 value = *atom;
    if (order == RZ_MEMORY_ORDER_ACQUIRE ||
        order == RZ_MEMORY_ORDER_ACQ_REL ||
        order == RZ_MEMORY_ORDER_SEQ_CST) {
        rz_atomic_acquire_barrier();
    }
    return value;
}
void rz_atomic32_store(rz_atomic_u32* atom, uint32_t value, rz_memory_order order)
{
    if (order == RZ_MEMORY_ORDER_RELEASE ||
        order == RZ_MEMORY_ORDER_ACQ_REL ||
        order == RZ_MEMORY_ORDER_SEQ_CST) {
        rz_atomic_release_barrier();
    }

    if (order == RZ_MEMORY_ORDER_SEQ_CST) {
        InterlockedExchange((volatile long*) atom, (long) value);
    } else {
        *atom = value;    // volatile-write
    }
}
uint32_t rz_atomic32_add(rz_atomic_u32* atom, uint32_t value, rz_memory_order order)
{
    long old = InterlockedExchangeAdd((volatile long*) atom, (long) value);    // returns old value

    if (order == RZ_MEMORY_ORDER_ACQUIRE ||
        order == RZ_MEMORY_ORDER_ACQ_REL) {
        rz_atomic_acquire_barrier();
    }
    return (u32) old;
}

uint32_t rz_atomic32_sub(rz_atomic_u32* atom, uint32_t value, rz_memory_order order)
{
    return rz_atomic32_add(atom, -(int32_t) value, order);
}

uint32_t rz_atomic32_increment(rz_atomic_u32* atom, rz_memory_order order)
{
    return InterlockedIncrement((volatile long*) atom);
}

uint32_t rz_atomic32_decrement(rz_atomic_u32* atom, rz_memory_order order)
{
    return InterlockedDecrement((volatile long*) atom);
}

bool rz_atomic32_cas(rz_atomic_u32* atom, uint32_t expected, uint32_t desired, rz_memory_order order)
{
    long old = InterlockedCompareExchange((volatile long*) atom, (long) desired, (long) expected);
    return old == (long) expected;
}
uint32_t rz_atomic32_exchange(rz_atomic_u32* atom, uint32_t desired, rz_memory_order order)
{
    long old = InterlockedExchange((volatile long*) atom, (long) desired);
    return (uint32_t) old;
}

uint64_t rz_atomic64_load(rz_atomic_u64* atom, rz_memory_order order)
{
    uint64_t value = *atom;
    if (order == RZ_MEMORY_ORDER_ACQUIRE ||
        order == RZ_MEMORY_ORDER_ACQ_REL ||
        order == RZ_MEMORY_ORDER_SEQ_CST) {
        rz_atomic_acquire_barrier();
    }
    return value;
}

void rz_atomic64_store(rz_atomic_u64* atom, uint64_t value, rz_memory_order order)
{
    if (order == RZ_MEMORY_ORDER_RELEASE ||
        order == RZ_MEMORY_ORDER_ACQ_REL ||
        order == RZ_MEMORY_ORDER_SEQ_CST) {
        rz_atomic_release_barrier();
    }

    if (order == RZ_MEMORY_ORDER_SEQ_CST) {
        InterlockedExchange64((volatile long long*) atom, (long long) value);
    } else {
        *atom = value;
    }
}

uint64_t rz_atomic64_add(rz_atomic_u64* atom, uint64_t value, rz_memory_order order)
{
    long long old = InterlockedExchangeAdd64((volatile long long*) atom, (long long) value);
    if (order == RZ_MEMORY_ORDER_ACQUIRE ||
        order == RZ_MEMORY_ORDER_ACQ_REL) {
        rz_atomic_acquire_barrier();
    }
    return (uint64_t) old;
}

uint64_t rz_atomic64_sub(rz_atomic_u64* atom, uint64_t value, rz_memory_order order)
{
    return rz_atomic64_add(atom, -(int64_t) value, order);
}

uint64_t rz_atomic64_increment(rz_atomic_u64* atom, rz_memory_order order)
{
    long long old = InterlockedIncrement64((volatile long long*) atom);
    return (uint64_t) old;
}

uint64_t rz_atomic64_decrement(rz_atomic_u64* atom, rz_memory_order order)
{
    long long old = InterlockedDecrement64((volatile long long*) atom);
    return (uint64_t) old;
}

bool rz_atomic64_cas(rz_atomic_u64* atom, uint64_t expected, uint64_t desired,
    rz_memory_order order)
{
    long long old = InterlockedCompareExchange64((volatile long long*) atom, (long long) desired, (long long) expected);
    return old == (long long) expected;
}

uint64_t rz_atomic64_exchange(rz_atomic_u64* atom, uint64_t desired, rz_memory_order order)
{
    long long old = InterlockedExchange64((volatile long long*) atom, (long long) desired);
    return (uint64_t) old;
}

void* rz_atomic_ptr_load(rz_atomic_uptr* atom, rz_memory_order order)
{
    return (void*) rz_atomic64_load((rz_atomic_u64*) atom, order);
}

void rz_atomic_ptr_store(rz_atomic_uptr* atom, void* value, rz_memory_order order)
{
    rz_atomic64_store((rz_atomic_u64*) atom, (uint64_t) value, order);
}

void* rz_atomic_ptr_exchange(rz_atomic_uptr* atom, void* desired, rz_memory_order order)
{
    return (void*) rz_atomic64_exchange((rz_atomic_u64*) atom, (uint64_t) desired, order);
}

bool rz_atomic_ptr_cas(rz_atomic_uptr* atom, void* expected, void* desired, rz_memory_order order)
{
    return rz_atomic64_cas((rz_atomic_u64*) atom, (uint64_t) expected, (uint64_t) desired, order);
}
#endif    // RAZIX_PLATFORM_WINDOWS
