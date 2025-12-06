#include "atomics.h"
#include "Razix/Core/RZCore.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <intrin.h>
    #include <windows.h>
#endif

void rz_atomic_full_barrier(void)
{
#ifdef RAZIX_PLATFORM_WINDOWS
    _ReadWriteBarrier();
#endif
}
void rz_atomic_acquire_barrier(void)
{
#ifdef RAZIX_PLATFORM_WINDOWS
    _ReadBarrier();
#endif
}
void rz_atomic_release_barrier(void)
{
#ifdef RAZIX_PLATFORM_WINDOWS
    _WriteBarrier();
#endif
}

#ifdef RAZIX_PLATFORM_WINDOWS

// --------------------
// Windows branch (unchanged)
// --------------------

// 32-bit
uint32_t rz_atomic32_load(RZAtomicU32* atom, RZMemoryOrder order)
{
    u32 value = *atom;
    if (order == RZ_MEMORY_ORDER_ACQUIRE ||
        order == RZ_MEMORY_ORDER_ACQ_REL ||
        order == RZ_MEMORY_ORDER_SEQ_CST) {
        rz_atomic_acquire_barrier();
    }
    return value;
}
void rz_atomic32_store(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order)
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
uint32_t rz_atomic32_add(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order)
{
    long old = InterlockedExchangeAdd((volatile long*) atom, (long) value);    // returns old value

    if (order == RZ_MEMORY_ORDER_ACQUIRE ||
        order == RZ_MEMORY_ORDER_ACQ_REL) {
        rz_atomic_acquire_barrier();
    }
    return (u32) old;
}

uint32_t rz_atomic32_sub(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order)
{
    return rz_atomic32_add(atom, -(int32_t) value, order);
}

uint32_t rz_atomic32_increment(RZAtomicU32* atom, RZMemoryOrder order)
{
    return InterlockedIncrement((volatile long*) atom);
}

uint32_t rz_atomic32_decrement(RZAtomicU32* atom, RZMemoryOrder order)
{
    return InterlockedDecrement((volatile long*) atom);
}

bool rz_atomic32_cas(RZAtomicU32* atom, uint32_t expected, uint32_t desired, RZMemoryOrder order)
{
    long old = InterlockedCompareExchange((volatile long*) atom, (long) desired, (long) expected);
    return old == (long) expected;
}
uint32_t rz_atomic32_exchange(RZAtomicU32* atom, uint32_t desired, RZMemoryOrder order)
{
    long old = InterlockedExchange((volatile long*) atom, (long) desired);
    return (uint32_t) old;
}

uint64_t rz_atomic64_load(RZAtomicU64* atom, RZMemoryOrder order)
{
    uint64_t value = *atom;
    if (order == RZ_MEMORY_ORDER_ACQUIRE ||
        order == RZ_MEMORY_ORDER_ACQ_REL ||
        order == RZ_MEMORY_ORDER_SEQ_CST) {
        rz_atomic_acquire_barrier();
    }
    return value;
}

void rz_atomic64_store(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order)
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

uint64_t rz_atomic64_add(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order)
{
    long long old = InterlockedExchangeAdd64((volatile long long*) atom, (long long) value);
    if (order == RZ_MEMORY_ORDER_ACQUIRE ||
        order == RZ_MEMORY_ORDER_ACQ_REL) {
        rz_atomic_acquire_barrier();
    }
    return (uint64_t) old;
}

uint64_t rz_atomic64_sub(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order)
{
    return rz_atomic64_add(atom, -(int64_t) value, order);
}

uint64_t rz_atomic64_increment(RZAtomicU64* atom, RZMemoryOrder order)
{
    long long old = InterlockedIncrement64((volatile long long*) atom);
    return (uint64_t) old;
}

uint64_t rz_atomic64_decrement(RZAtomicU64* atom, RZMemoryOrder order)
{
    long long old = InterlockedDecrement64((volatile long long*) atom);
    return (uint64_t) old;
}

bool rz_atomic64_cas(RZAtomicU64* atom, uint64_t expected, uint64_t desired,
    RZMemoryOrder order)
{
    long long old = InterlockedCompareExchange64((volatile long long*) atom, (long long) desired, (long long) expected);
    return old == (long long) expected;
}

uint64_t rz_atomic64_exchange(RZAtomicU64* atom, uint64_t desired, RZMemoryOrder order)
{
    long long old = InterlockedExchange64((volatile long long*) atom, (long long) desired);
    return (uint64_t) old;
}

void* rz_atomic_ptr_load(RZAtomicUPtr* atom, RZMemoryOrder order)
{
    return (void*) rz_atomic64_load((RZAtomicU64*) atom, order);
}

void rz_atomic_ptr_store(RZAtomicUPtr* atom, void* value, RZMemoryOrder order)
{
    rz_atomic64_store((RZAtomicU64*) atom, (uint64_t) value, order);
}

void* rz_atomic_ptr_exchange(RZAtomicUPtr* atom, void* desired, RZMemoryOrder order)
{
    return (void*) rz_atomic64_exchange((RZAtomicU64*) atom, (uint64_t) desired, order);
}

bool rz_atomic_ptr_cas(RZAtomicUPtr* atom, void* expected, void* desired, RZMemoryOrder order)
{
    return rz_atomic64_cas((RZAtomicU64*) atom, (uint64_t) expected, (uint64_t) desired, order);
}

#else    // GCC/Clang implementation using __atomic intrinsics

static inline int rz_map_memorder(RZMemoryOrder order)
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

// 32-bit
uint32_t rz_atomic32_load(RZAtomicU32* atom, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_load_n((uint32_t*) atom, mo);
}

void rz_atomic32_store(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    __atomic_store_n((uint32_t*) atom, value, mo);
}

uint32_t rz_atomic32_add(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    // __atomic_add_fetch returns the new value (post-add). This matches your request.
    return __atomic_add_fetch((uint32_t*) atom, value, mo);
}

uint32_t rz_atomic32_sub(RZAtomicU32* atom, uint32_t value, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_sub_fetch((uint32_t*) atom, value, mo);
}

uint32_t rz_atomic32_increment(RZAtomicU32* atom, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_add_fetch((uint32_t*) atom, 1, mo);
}

uint32_t rz_atomic32_decrement(RZAtomicU32* atom, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_sub_fetch((uint32_t*) atom, 1, mo);
}

bool rz_atomic32_cas(RZAtomicU32* atom, uint32_t expected, uint32_t desired, RZMemoryOrder order)
{
    // For CAS we need two orders: success and failure. Failure must be no stronger than success and cannot be RELEASE.
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

uint32_t rz_atomic32_exchange(RZAtomicU32* atom, uint32_t desired, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_exchange_n((uint32_t*) atom, desired, mo);
}

// 64-bit
uint64_t rz_atomic64_load(RZAtomicU64* atom, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_load_n((uint64_t*) atom, mo);
}

void rz_atomic64_store(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    __atomic_store_n((uint64_t*) atom, value, mo);
}

uint64_t rz_atomic64_add(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_add_fetch((uint64_t*) atom, value, mo);
}

uint64_t rz_atomic64_sub(RZAtomicU64* atom, uint64_t value, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_sub_fetch((uint64_t*) atom, value, mo);
}

uint64_t rz_atomic64_increment(RZAtomicU64* atom, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_add_fetch((uint64_t*) atom, 1, mo);
}

uint64_t rz_atomic64_decrement(RZAtomicU64* atom, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_sub_fetch((uint64_t*) atom, 1, mo);
}

bool rz_atomic64_cas(RZAtomicU64* atom, uint64_t expected, uint64_t desired,
    RZMemoryOrder order)
{
    // For CAS we need two orders: success and failure. Failure must be no stronger than success and cannot be RELEASE.
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

uint64_t rz_atomic64_exchange(RZAtomicU64* atom, uint64_t desired, RZMemoryOrder order)
{
    int mo = rz_map_memorder(order);
    return __atomic_exchange_n((uint64_t*) atom, desired, mo);
}

// pointer atomics (assume uintptr_t sized same as 64-bit atomic functions)
void* rz_atomic_ptr_load(RZAtomicUPtr* atom, RZMemoryOrder order)
{
    // reinterpret as uint64_t for atomic ops
    uint64_t v = rz_atomic64_load((RZAtomicU64*) atom, order);
    return (void*) (uintptr_t) v;
}

void rz_atomic_ptr_store(RZAtomicUPtr* atom, void* value, RZMemoryOrder order)
{
    rz_atomic64_store((RZAtomicU64*) atom, (uint64_t) (uintptr_t) value, order);
}

void* rz_atomic_ptr_exchange(RZAtomicUPtr* atom, void* desired, RZMemoryOrder order)
{
    uint64_t old = rz_atomic64_exchange((RZAtomicU64*) atom, (uint64_t) (uintptr_t) desired, order);
    return (void*) (uintptr_t) old;
}

bool rz_atomic_ptr_cas(RZAtomicUPtr* atom, void* expected, void* desired, RZMemoryOrder order)
{
    return rz_atomic64_cas((RZAtomicU64*) atom, (uint64_t) (uintptr_t) expected, (uint64_t) (uintptr_t) desired, order);
}

#endif    // RAZIX_PLATFORM_WINDOWS
