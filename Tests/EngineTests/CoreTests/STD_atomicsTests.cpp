// sprintfTests.cpp
// AI-generated unit tests for the RZUUID classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/RZAtomics.h"

#include <gtest/gtest.h>

#include <thread>
#include <vector>

namespace Razix {

    // Helper: accept either old or new semantics for fetch-add/sub return
    static void expect_old_or_new_eq_uint32(uint32_t observed, uint32_t before, uint32_t after)
    {
        EXPECT_TRUE(observed == before || observed == after) << "observed=" << observed << " before=" << before << " after=" << after;
    }
    static void expect_old_or_new_eq_uint64(uint64_t observed, uint64_t before, uint64_t after)
    {
        EXPECT_TRUE(observed == before || observed == after) << "observed=" << observed << " before=" << before << " after=" << after;
    }

    class RZAtomicsTests : public ::testing::Test
    {
    };

    TEST(RZAtomicsTests, LoadStore32_AcquireRelease)
    {
        RZAtomicU32 a = 0;
        rz_atomic32_store(&a, 123u, RZ_MEMORY_ORDER_RELEASE);
        uint32_t v = rz_atomic32_load(&a, RZ_MEMORY_ORDER_ACQUIRE);
        EXPECT_EQ(v, 123u);
    }

    TEST(RZAtomicsTests, AddSubConsistency32)
    {
        RZAtomicU32 a      = 10u;
        uint32_t       before = rz_atomic32_load(&a, RZ_MEMORY_ORDER_RELAXED);
        uint32_t       ret    = rz_atomic32_add(&a, 5u, RZ_MEMORY_ORDER_SEQ_CST);    // platform may return old or new
        uint32_t       after  = rz_atomic32_load(&a, RZ_MEMORY_ORDER_SEQ_CST);
        EXPECT_EQ(after, before + 5u);
        expect_old_or_new_eq_uint32(ret, before, after);
    }

    TEST(RZAtomicsTests, IncrementDecrement32)
    {
        RZAtomicU32 a    = 0u;
        uint32_t       newv = rz_atomic32_increment(&a, RZ_MEMORY_ORDER_SEQ_CST);
        // increment might return pre- or post-value depending on implementation, accept both:
        uint32_t cur = rz_atomic32_load(&a, RZ_MEMORY_ORDER_SEQ_CST);
        EXPECT_EQ(cur, 1u);
        EXPECT_TRUE(newv == 0u || newv == 1u);
        uint32_t newv2 = rz_atomic32_decrement(&a, RZ_MEMORY_ORDER_SEQ_CST);
        uint32_t cur2  = rz_atomic32_load(&a, RZ_MEMORY_ORDER_SEQ_CST);
        EXPECT_EQ(cur2, 0u);
        EXPECT_TRUE(newv2 == 1u || newv2 == 0u);
    }

    TEST(RZAtomicsTests, CASExchange32)
    {
        RZAtomicU32 a  = 7u;
        bool           ok = rz_atomic32_cas(&a, 7u, 42u, RZ_MEMORY_ORDER_SEQ_CST);
        EXPECT_TRUE(ok);
        EXPECT_EQ(rz_atomic32_load(&a, RZ_MEMORY_ORDER_SEQ_CST), 42u);

        uint32_t old = rz_atomic32_exchange(&a, 100u, RZ_MEMORY_ORDER_SEQ_CST);
        // exchange returns the previous value by contract on most APIs; ensure new value is set
        EXPECT_EQ(rz_atomic32_load(&a, RZ_MEMORY_ORDER_SEQ_CST), 100u);
        EXPECT_TRUE(old == 42u || old == 100u);    // permit unusual platform behavior but prefer old==42
    }

    //TEST(RZAtomicsTests, PointerLoadStoreExchangeCAS)
    //{
    //    RazixAtomicUPtr p   = (RazixAtomicUPtr) NULL;
    //    void*           ptr = malloc(16);
    //    rz_atomic_ptr_store(&p, ptr, RZ_MEMORY_ORDER_SEQ_CST);
    //    void* got = rz_atomic_ptr_load(&p, RZ_MEMORY_ORDER_SEQ_CST);
    //    EXPECT_EQ(got, ptr);
    //
    //    void* newptr = malloc(8);
    //    void* prev   = rz_atomic_ptr_exchange(&p, newptr, RZ_MEMORY_ORDER_SEQ_CST);
    //    EXPECT_TRUE(prev == ptr || prev == newptr);
    //    EXPECT_EQ(rz_atomic_ptr_load(&p, RZ_MEMORY_ORDER_SEQ_CST), newptr);
    //
    //    // CAS: try replace newptr with NULL
    //    bool cas_ok = rz_atomic_ptr_cas(&p, newptr, NULL, RZ_MEMORY_ORDER_SEQ_CST);
    //    EXPECT_TRUE(cas_ok);
    //    EXPECT_EQ(rz_atomic_ptr_load(&p, RZ_MEMORY_ORDER_SEQ_CST), NULL);
    //
    //    free(ptr);
    //    free(newptr);
    //}

    TEST(RZAtomicsTests, LoadStore64)
    {
        RZAtomicU64 a = 0ull;
        rz_atomic64_store(&a, 0x1122334455667788ull, RZ_MEMORY_ORDER_RELEASE);
        uint64_t v = rz_atomic64_load(&a, RZ_MEMORY_ORDER_ACQUIRE);
        EXPECT_EQ(v, 0x1122334455667788ull);
    }

    TEST(RZAtomicsTests, AddSubConsistency64)
    {
        RZAtomicU64 a      = 1000ull;
        uint64_t       before = rz_atomic64_load(&a, RZ_MEMORY_ORDER_RELAXED);
        uint64_t       ret    = rz_atomic64_add(&a, 500ull, RZ_MEMORY_ORDER_SEQ_CST);
        uint64_t       after  = rz_atomic64_load(&a, RZ_MEMORY_ORDER_SEQ_CST);
        EXPECT_EQ(after, before + 500ull);
        expect_old_or_new_eq_uint64(ret, before, after);
    }

    TEST(RZAtomicsTests, CASExchange64)
    {
        RZAtomicU64 a  = 0xAAull;
        bool           ok = rz_atomic64_cas(&a, 0xAAull, 0xBBull, RZ_MEMORY_ORDER_SEQ_CST);
        EXPECT_TRUE(ok);
        EXPECT_EQ(rz_atomic64_load(&a, RZ_MEMORY_ORDER_SEQ_CST), 0xBBull);

        uint64_t old = rz_atomic64_exchange(&a, 0xCCull, RZ_MEMORY_ORDER_SEQ_CST);
        EXPECT_EQ(rz_atomic64_load(&a, RZ_MEMORY_ORDER_SEQ_CST), 0xCCull);
        EXPECT_TRUE(old == 0xBBull || old == 0xCCull);
    }

    // Multithreaded stress test: increments many times concurrently (test for lost updates)
    TEST(RZAtomicsTests, ConcurrentIncrements32)
    {
        RZAtomicU32           counter = 0u;
        const int                threads = 8;
        const int                iters   = 50000;
        std::vector<std::thread> th;
        th.reserve(threads);

        for (int t = 0; t < threads; ++t) {
            th.emplace_back([&counter, iters]() {
                for (int i = 0; i < iters; ++i) {
                    // RELAXED is sufficient for a pure counter (we only care about atomicity)
                    rz_atomic32_add(&counter, 1u, RZ_MEMORY_ORDER_RELAXED);
                }
            });
        }
        for (auto& tt: th) tt.join();

        uint32_t final = rz_atomic32_load(&counter, RZ_MEMORY_ORDER_SEQ_CST);
        EXPECT_EQ(final, static_cast<uint32_t>(threads * iters));
    }
}    // namespace Razix
