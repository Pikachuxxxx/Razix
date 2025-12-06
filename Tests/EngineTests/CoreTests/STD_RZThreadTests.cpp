#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/thread.h"

#include <gtest/gtest.h>

#include <chrono>
#include <string.h>

namespace Razix {
namespace {
    using SteadyClock = std::chrono::steady_clock;

    template<typename F>
    uint64_t MeasureMicros(F&& fn)
    {
        const auto start = SteadyClock::now();
        fn();
        const auto end = SteadyClock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    struct ThreadBarrier
    {
        RZAtomicU32 ready;
        RZAtomicU32 start;
        RZAtomicU32 done;
    };

    static void BarrierInit(ThreadBarrier& barrier)
    {
        rz_atomic32_store(&barrier.ready, 0u, RZ_MEMORY_ORDER_RELAXED);
        rz_atomic32_store(&barrier.start, 0u, RZ_MEMORY_ORDER_RELAXED);
        rz_atomic32_store(&barrier.done, 0u, RZ_MEMORY_ORDER_RELAXED);
    }

    static void SpinUntilEquals(RZAtomicU32* atom, uint32_t expected)
    {
        while (rz_atomic32_load(atom, RZ_MEMORY_ORDER_ACQUIRE) != expected)
            rz_thread_yield();
    }

    static void BarrierWaitForReady(ThreadBarrier& barrier, uint32_t expected)
    {
        SpinUntilEquals(&barrier.ready, expected);
    }

    static void BarrierWaitForDone(ThreadBarrier& barrier, uint32_t expected)
    {
        SpinUntilEquals(&barrier.done, expected);
    }

    static void BarrierSignalStart(ThreadBarrier& barrier)
    {
        rz_atomic32_store(&barrier.start, 1u, RZ_MEMORY_ORDER_RELEASE);
    }

    static void BarrierWaitForStart(ThreadBarrier& barrier)
    {
        SpinUntilEquals(&barrier.start, 1u);
    }

    static void DetachCurrentThread()
    {
        const RZThreadHandle self = rz_thread_get_current_handle();
        if (self != RAZIX_INVALID_THREAD_ID && self != 0)
            rz_thread_detach(self);
    }

    struct MainFlagData
    {
        RZAtomicU32 ready;
        RZAtomicU32 isMain;
    };

    static void ThreadCheckMain(void* userData)
    {
        DetachCurrentThread();
        MainFlagData* data = static_cast<MainFlagData*>(userData);
        const uint32_t value = rz_thread_is_main() ? 1u : 0u;
        rz_atomic32_store(&data->isMain, value, RZ_MEMORY_ORDER_RELEASE);
        rz_atomic32_store(&data->ready, 1u, RZ_MEMORY_ORDER_RELEASE);
    }

    struct IdCapturePayload
    {
        ThreadBarrier*             barrier;
        RZDynamicArray<uint64_t>*  ids;
        uint32_t                   slot;
    };

    static void ThreadCaptureId(void* userData)
    {
        DetachCurrentThread();
        IdCapturePayload* payload = static_cast<IdCapturePayload*>(userData);
        rz_atomic32_add(&payload->barrier->ready, 1u, RZ_MEMORY_ORDER_ACQ_REL);
        BarrierWaitForStart(*payload->barrier);
        (*payload->ids)[payload->slot] = rz_thread_get_current_id();
        rz_atomic32_add(&payload->barrier->done, 1u, RZ_MEMORY_ORDER_RELEASE);
    }

    struct MixedApiPayload
    {
        ThreadBarrier* barrier;
        uint32_t        iterations;
        RZAtomicU32*    zeroIds;
        RZAtomicU32*    zeroHandles;
        RZAtomicU32*    loopCount;
    };

    static void ThreadMixedApi(void* userData)
    {
        DetachCurrentThread();
        MixedApiPayload* payload = static_cast<MixedApiPayload*>(userData);
        rz_atomic32_add(&payload->barrier->ready, 1u, RZ_MEMORY_ORDER_ACQ_REL);
        BarrierWaitForStart(*payload->barrier);

        for (uint32_t i = 0; i < payload->iterations; ++i) {
            if ((i % 3u) == 0u)
                rz_thread_sleep(1);

            rz_thread_busy_wait_micro(200);
            rz_thread_yield();

            if (rz_thread_get_current_id() == 0u)
                rz_atomic32_add(payload->zeroIds, 1u, RZ_MEMORY_ORDER_RELAXED);
            if (rz_thread_get_current_handle() == 0u)
                rz_atomic32_add(payload->zeroHandles, 1u, RZ_MEMORY_ORDER_RELAXED);

            rz_atomic32_add(payload->loopCount, 1u, RZ_MEMORY_ORDER_RELAXED);
        }

        rz_atomic32_add(&payload->barrier->done, 1u, RZ_MEMORY_ORDER_RELEASE);
    }

    struct YieldStormPayload
    {
        ThreadBarrier* barrier;
        uint32_t        iterations;
    };

    static void ThreadYieldStorm(void* userData)
    {
        DetachCurrentThread();
        YieldStormPayload* payload = static_cast<YieldStormPayload*>(userData);
        rz_atomic32_add(&payload->barrier->ready, 1u, RZ_MEMORY_ORDER_ACQ_REL);
        BarrierWaitForStart(*payload->barrier);

        for (uint32_t i = 0; i < payload->iterations; ++i)
            rz_thread_yield();

        rz_atomic32_add(&payload->barrier->done, 1u, RZ_MEMORY_ORDER_RELEASE);
    }

    struct BusyWaitPayload
    {
        ThreadBarrier* barrier;
        uint32_t        iterations;
    };

    static void ThreadBusyWait(void* userData)
    {
        DetachCurrentThread();
        BusyWaitPayload* payload = static_cast<BusyWaitPayload*>(userData);
        rz_atomic32_add(&payload->barrier->ready, 1u, RZ_MEMORY_ORDER_ACQ_REL);
        BarrierWaitForStart(*payload->barrier);

        for (uint32_t i = 0; i < payload->iterations; ++i) {
            rz_thread_busy_wait_micro(400);
            rz_thread_yield();
        }

        rz_atomic32_add(&payload->barrier->done, 1u, RZ_MEMORY_ORDER_RELEASE);
    }

    struct ParallelSleepPayload
    {
        ThreadBarrier* barrier;
        uint32_t        iterations;
        uint32_t        sleepMs;
    };

    static void ThreadParallelSleep(void* userData)
    {
        DetachCurrentThread();
        ParallelSleepPayload* payload = static_cast<ParallelSleepPayload*>(userData);
        rz_atomic32_add(&payload->barrier->ready, 1u, RZ_MEMORY_ORDER_ACQ_REL);
        BarrierWaitForStart(*payload->barrier);

        for (uint32_t i = 0; i < payload->iterations; ++i) {
            rz_thread_sleep(payload->sleepMs);
            rz_thread_yield();
        }

        rz_atomic32_add(&payload->barrier->done, 1u, RZ_MEMORY_ORDER_RELEASE);
    }

}    // namespace

// Basic API sanity -----------------------------------------------------------
TEST(RZThreadBasicApiTests, MainThreadFlagRoundTrip)
{
    rz_thread_set_main();
    EXPECT_TRUE(rz_thread_is_main());
}

TEST(RZThreadBasicApiTests, MainFlagIsThreadLocal)
{
    MainFlagData data{};
    rz_atomic32_store(&data.ready, 0u, RZ_MEMORY_ORDER_RELAXED);
    rz_atomic32_store(&data.isMain, 1u, RZ_MEMORY_ORDER_RELAXED);

    RZThreadHandle handle = rz_thread_create("MainFlagCheck", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ThreadCheckMain, &data);
    rz_thread_set_main();
    RAZIX_UNUSED(handle);

    SpinUntilEquals(&data.ready, 1u);
    EXPECT_EQ(rz_atomic32_load(&data.isMain, RZ_MEMORY_ORDER_ACQUIRE), 0u);
}

TEST(RZThreadBasicApiTests, CurrentThreadIdIsStable)
{
    const uint64_t id1 = rz_thread_get_current_id();
    const uint64_t id2 = rz_thread_get_current_id();
    EXPECT_EQ(id1, id2);
}

TEST(RZThreadBasicApiTests, CurrentThreadHandleIsNonZero)
{
    const RZThreadHandle handle = rz_thread_get_current_handle();
    EXPECT_NE(handle, static_cast<RZThreadHandle>(0));
}

TEST(RZThreadBasicApiTests, CurrentThreadNameIsReadable)
{
    const char* name = rz_thread_get_current_name();
    ASSERT_NE(name, nullptr);
    EXPECT_GT(strlen(name), 0u);
}

TEST(RZThreadBasicApiTests, PriorityQueryReturnsValidValue)
{
    const int priority = rz_thread_get_priority();
    EXPECT_GE(priority, -1);
}

TEST(RZThreadBasicApiTests, SetNameRoundTrip)
{
    const char* newName = "ThreadNameTest";
    rz_thread_set_name(newName);
    const char* reported = rz_thread_get_current_name();
    ASSERT_NE(reported, nullptr);
    EXPECT_NE(strstr(reported, newName), nullptr);
}

// Timing helpers -------------------------------------------------------------
TEST(RZThreadTimingTests, SleepHonorsMinimumMilliseconds)
{
    constexpr auto requested = std::chrono::milliseconds(5);
    const auto start         = SteadyClock::now();
    rz_thread_sleep(static_cast<uint32_t>(requested.count()));
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(SteadyClock::now() - start);
    EXPECT_GE(elapsed.count(), requested.count() - 1);
}

TEST(RZThreadTimingTests, BusyWaitMicroHonorsMinimum)
{
    constexpr uint32_t requested = 1000u;
    const auto elapsedMicros     = MeasureMicros([&]() {
        rz_thread_busy_wait_micro(requested);
    });
    EXPECT_GE(elapsedMicros, requested);
    EXPECT_LT(elapsedMicros, requested * 10u);
}

TEST(RZThreadTimingTests, BusyWaitScalesWithDuration)
{
    const auto shortWait = MeasureMicros([]() {
        rz_thread_busy_wait_micro(200);
    });
    const auto longWait = MeasureMicros([]() {
        rz_thread_busy_wait_micro(800);
    });
    EXPECT_GT(longWait, shortWait);
}

// Benchmark-style checks -----------------------------------------------------
TEST(RZThreadBenchmarkTests, YieldLatencyRemainsUnderBudget)
{
    constexpr int iterations = 2000;
    const auto     start      = SteadyClock::now();
    for (int i = 0; i < iterations; ++i)
        rz_thread_yield();
    const auto total = std::chrono::duration_cast<std::chrono::milliseconds>(SteadyClock::now() - start);
    EXPECT_LT(total.count(), 500);
}

TEST(RZThreadBenchmarkTests, BusyWaitMicroAverageIsReasonable)
{
    constexpr uint32_t iterations = 64u;
    constexpr uint32_t requested  = 500u;
    uint64_t           totalMicros = 0;
    for (uint32_t i = 0; i < iterations; ++i)
        totalMicros += MeasureMicros([&]() {
            rz_thread_busy_wait_micro(requested);
        });
    const uint64_t average = totalMicros / iterations;
    EXPECT_GE(average, requested);
    EXPECT_LT(average, requested * 8u);
}

// Stress tests ----------------------------------------------------------------
TEST(RZThreadStressTests, ConcurrentIdsAreUnique)
{
    constexpr uint32_t threadCount = 16u;

    ThreadBarrier barrier;
    BarrierInit(barrier);

    RZDynamicArray<uint64_t> ids;
    ids.resize(threadCount);

    RZDynamicArray<IdCapturePayload> payloads;
    payloads.resize(threadCount);

    RZDynamicArray<RZThreadHandle> handles;
    handles.reserve(threadCount);

    for (uint32_t i = 0; i < threadCount; ++i) {
        payloads[i].barrier = &barrier;
        payloads[i].ids     = &ids;
        payloads[i].slot    = i;
        handles.push_back(rz_thread_create("CaptureId", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ThreadCaptureId, &payloads[i]));
    }

    BarrierWaitForReady(barrier, threadCount);
    BarrierSignalStart(barrier);
    BarrierWaitForDone(barrier, threadCount);

    bool duplicateFound = false;
    for (uint32_t i = 0; i < threadCount && !duplicateFound; ++i) {
        for (uint32_t j = i + 1; j < threadCount; ++j) {
            if (ids[i] == ids[j]) {
                duplicateFound = true;
                break;
            }
        }
    }

    EXPECT_FALSE(duplicateFound);
}

TEST(RZThreadStressTests, MixedApiCallsStayResponsive)
{
    constexpr uint32_t threadCount = 8u;
    constexpr uint32_t iterations  = 200u;

    ThreadBarrier barrier;
    BarrierInit(barrier);

    RZAtomicU32 zeroIds     = 0u;
    RZAtomicU32 zeroHandles = 0u;
    RZAtomicU32 loopCount   = 0u;

    RZDynamicArray<MixedApiPayload> payloads;
    payloads.resize(threadCount);

    RZDynamicArray<RZThreadHandle> handles;
    handles.reserve(threadCount);

    for (uint32_t i = 0; i < threadCount; ++i) {
        payloads[i].barrier     = &barrier;
        payloads[i].iterations  = iterations;
        payloads[i].zeroIds     = &zeroIds;
        payloads[i].zeroHandles = &zeroHandles;
        payloads[i].loopCount   = &loopCount;
        handles.push_back(rz_thread_create("MixedApi", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ThreadMixedApi, &payloads[i]));
    }

    BarrierWaitForReady(barrier, threadCount);
    BarrierSignalStart(barrier);
    BarrierWaitForDone(barrier, threadCount);

    EXPECT_EQ(rz_atomic32_load(&zeroIds, RZ_MEMORY_ORDER_SEQ_CST), 0u);
    EXPECT_EQ(rz_atomic32_load(&zeroHandles, RZ_MEMORY_ORDER_SEQ_CST), 0u);
    EXPECT_EQ(rz_atomic32_load(&loopCount, RZ_MEMORY_ORDER_SEQ_CST), threadCount * iterations);
}

TEST(RZThreadStressTests, HighThreadCountYieldStormFinishesQuickly)
{
    constexpr uint32_t threadCount = 24u;
    constexpr uint32_t iterations  = 2000u;

    ThreadBarrier barrier;
    BarrierInit(barrier);

    RZDynamicArray<YieldStormPayload> payloads;
    payloads.resize(threadCount);
    RZDynamicArray<RZThreadHandle> handles;
    handles.reserve(threadCount);

    for (uint32_t i = 0; i < threadCount; ++i) {
        payloads[i].barrier    = &barrier;
        payloads[i].iterations = iterations;
        handles.push_back(rz_thread_create("YieldStorm", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ThreadYieldStorm, &payloads[i]));
    }

    BarrierWaitForReady(barrier, threadCount);
    const auto start = SteadyClock::now();
    BarrierSignalStart(barrier);
    BarrierWaitForDone(barrier, threadCount);
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(SteadyClock::now() - start);

    EXPECT_LT(elapsed.count(), 15000);
}

TEST(RZThreadStressTests, BusyWaitContentionStillMakesProgress)
{
    constexpr uint32_t threadCount = 12u;
    constexpr uint32_t iterations  = 150u;

    ThreadBarrier barrier;
    BarrierInit(barrier);

    RZDynamicArray<BusyWaitPayload> payloads;
    payloads.resize(threadCount);
    RZDynamicArray<RZThreadHandle> handles;
    handles.reserve(threadCount);

    for (uint32_t i = 0; i < threadCount; ++i) {
        payloads[i].barrier    = &barrier;
        payloads[i].iterations = iterations;
        handles.push_back(rz_thread_create("BusyWait", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ThreadBusyWait, &payloads[i]));
    }

    BarrierWaitForReady(barrier, threadCount);
    BarrierSignalStart(barrier);
    BarrierWaitForDone(barrier, threadCount);

    EXPECT_EQ(rz_atomic32_load(&barrier.done, RZ_MEMORY_ORDER_SEQ_CST), threadCount);
}

TEST(RZThreadStressTests, ParallelSleepDoesNotBlockOthers)
{
    constexpr uint32_t threadCount = 10u;
    constexpr uint32_t iterations  = 10u;
    constexpr uint32_t sleepMs     = 3u;

    ThreadBarrier barrier;
    BarrierInit(barrier);

    RZDynamicArray<ParallelSleepPayload> payloads;
    payloads.resize(threadCount);
    RZDynamicArray<RZThreadHandle> handles;
    handles.reserve(threadCount);

    for (uint32_t i = 0; i < threadCount; ++i) {
        payloads[i].barrier    = &barrier;
        payloads[i].iterations = iterations;
        payloads[i].sleepMs    = sleepMs;
        handles.push_back(rz_thread_create("ParallelSleep", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ThreadParallelSleep, &payloads[i]));
    }

    BarrierWaitForReady(barrier, threadCount);
    const auto start = SteadyClock::now();
    BarrierSignalStart(barrier);
    BarrierWaitForDone(barrier, threadCount);
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(SteadyClock::now() - start);

    const uint32_t upperBound = threadCount * sleepMs * 25u;
    EXPECT_LT(static_cast<uint64_t>(elapsed.count()), static_cast<uint64_t>(upperBound));
}

}    // namespace Razix
