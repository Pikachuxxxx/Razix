#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZThreadCore.h"
#include "Razix/Core/std/sync.h"
#include "Razix/Core/std/thread.h"

#include <gtest/gtest.h>

#include <chrono>

namespace Razix {
    namespace {
        using SteadyClock = std::chrono::steady_clock;

        class RZSyncTestFixture : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                Razix::Debug::RZLog::StartUp();
            }

            void TearDown() override
            {
                Razix::Debug::RZLog::Shutdown();
            }
        };

        class RZSyncCriticalSectionTests : public RZSyncTestFixture
        {
        };

        class RZSyncConditionalVarTests : public RZSyncTestFixture
        {
        };

        class RZSyncIntegrationTests : public RZSyncTestFixture
        {
        };

        struct ProducerConsumerPayload
        {
            RZCriticalSection*        cs;
            RZConditionalVar*         cv;
            RZDynamicArray<uint32_t>* queue;
            uint32_t*                 head;
            uint32_t*                 tail;
            uint32_t                  items;
        };

        static void ConsumerThread(void* userData)
        {
            ProducerConsumerPayload* payload = static_cast<ProducerConsumerPayload*>(userData);
            RZScopedCriticalSection  lock(*payload->cs);

            uint32_t consumed = 0;
            while (consumed < payload->items) {
                while (*payload->tail == *payload->head)
                    payload->cv->wait(payload->cs);

                uint32_t index           = (*payload->head) % payload->queue->size();
                (*payload->queue)[index] = consumed;
                ++(*payload->head);
                consumed++;
                payload->cv->signal();
            }
        }

        static void ProducerThread(void* userData)
        {
            ProducerConsumerPayload* payload = static_cast<ProducerConsumerPayload*>(userData);
            RZScopedCriticalSection  lock(*payload->cs);

            for (uint32_t i = 0; i < payload->items; ++i) {
                while ((*payload->tail - *payload->head) >= payload->queue->size())
                    payload->cv->wait(payload->cs);

                uint32_t index           = (*payload->tail) % payload->queue->size();
                (*payload->queue)[index] = i;
                ++(*payload->tail);
                payload->cv->signal();
            }
        }

        struct ContentionPayload
        {
            RZCriticalSection* cs;
            uint32_t*          enterCount;
            uint32_t           iterations;
        };

        static void ContentionWorker(void* userData)
        {
            ContentionPayload* payload = static_cast<ContentionPayload*>(userData);

            for (uint32_t i = 0; i < payload->iterations; ++i) {
                payload->cs->lock();
                ++(*payload->enterCount);
                payload->cs->unlock();
            }
        }

        struct SingleSignalPayload
        {
            RZCriticalSection* cs;
            RZConditionalVar*  cv;
            uint32_t*          wakeCount;
            RZCriticalSection* readyLock;
            RZConditionalVar*  readyCv;
            bool*              readyFlag;
        };

        static void SingleSignalWaiter(void* userData)
        {
            SingleSignalPayload* payload = static_cast<SingleSignalPayload*>(userData);
            {
                RZScopedCriticalSection readyGuard(*payload->readyLock);
                *payload->readyFlag = true;
                payload->readyCv->signal();
            }

            RZScopedCriticalSection lock(*payload->cs);
            ++(*payload->wakeCount);
            payload->cv->wait(payload->cs);
            ++(*payload->wakeCount);
        }

        struct BroadcastPayload
        {
            RZCriticalSection* cs;
            RZConditionalVar*  cv;
            uint32_t*          wakeCount;
            uint32_t*          predicate;
        };

        static void BroadcastWaiter(void* userData)
        {
            BroadcastPayload*       payload = static_cast<BroadcastPayload*>(userData);
            RZScopedCriticalSection lock(*payload->cs);
            while (*payload->predicate == 0u)
                payload->cv->wait(payload->cs);
            ++(*payload->wakeCount);
        }

        struct BroadcastLatencyPayload
        {
            RZCriticalSection* cs;
            RZConditionalVar*  cv;
            uint32_t*          wakeCount;
            uint32_t*          fastWakeCount;
            uint32_t*          predicate;
        };

        static void BroadcastLatencyWaiter(void* userData)
        {
            BroadcastLatencyPayload* payload = static_cast<BroadcastLatencyPayload*>(userData);
            RZScopedCriticalSection  lock(*payload->cs);
            const auto               start = SteadyClock::now();
            while (*payload->predicate == 0u)
                payload->cv->wait(payload->cs);
            const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(SteadyClock::now() - start);
            ++(*payload->wakeCount);
            if (elapsed.count() < 5000)
                ++(*payload->fastWakeCount);
        }

    }    // namespace

    // RZCriticalSection basics ---------------------------------------------------
    TEST_F(RZSyncCriticalSectionTests, InitLockUnlock)
    {
        RZCriticalSection cs;
        cs.init();
        cs.lock();
        cs.unlock();
        cs.destroy();
    }

    TEST_F(RZSyncCriticalSectionTests, TryLockBehavior)
    {
        RZCriticalSection cs;
        cs.init();

        EXPECT_TRUE(cs.try_lock());
#ifdef RAZIX_PLATFORM_UNIX
        EXPECT_FALSE(cs.try_lock());    // On POSIX, a thread can relock a mutex it already holds (deadlock mutexes excluded)
#endif
        cs.unlock();

        cs.destroy();
    }

    TEST_F(RZSyncCriticalSectionTests, ScopedLockReleasesOnDestruct)
    {
        RZCriticalSection cs;
        cs.init();

        {
            RZScopedCriticalSection lock(cs);
#ifdef RAZIX_PLATFORM_UNIX
            EXPECT_FALSE(cs.try_lock());    // On POSIX, a thread can relock a mutex it already holds (deadlock mutexes excluded)
#endif
        }

        EXPECT_TRUE(cs.try_lock());
        cs.unlock();
        // RZScopedCriticalSection only automates lock/unlock; init/destroy stays manual.
        cs.destroy();
    }

    TEST_F(RZSyncCriticalSectionTests, ContentionProgress)
    {
        constexpr uint32_t threadCount = 10u;
        constexpr uint32_t iterations  = 2000u;

        RZCriticalSection cs = {};
        cs.init();

        uint32_t enterCount = 0u;

        RZDynamicArray<ContentionPayload> payloads;
        payloads.resize(threadCount);
        RZDynamicArray<RZThreadHandle> handles;
        handles.reserve(threadCount);

        for (uint32_t i = 0; i < threadCount; ++i) {
            payloads[i].cs         = &cs;
            payloads[i].enterCount = &enterCount;
            payloads[i].iterations = iterations;
            handles.push_back(rz_thread_create("CSContention", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ContentionWorker, &payloads[i]));
        }

        for (auto handle: handles)
            rz_thread_join(handle);

        EXPECT_EQ(enterCount, threadCount * iterations);

        cs.destroy();
    }

    // RZConditionalVar functionality ---------------------------------------------
    TEST_F(RZSyncConditionalVarTests, SingleSignalWakesOne)
    {
        RZCriticalSection cs;
        cs.init();
        RZConditionalVar cv;
        cv.init();

        uint32_t          wakeCount   = 0u;
        bool              waiterReady = false;
        RZCriticalSection readyCS;
        readyCS.init();
        RZConditionalVar readyCV;
        readyCV.init();

        SingleSignalPayload payload{&cs, &cv, &wakeCount, &readyCS, &readyCV, &waiterReady};
        RZThreadHandle      handle = rz_thread_create("CVSingle", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, SingleSignalWaiter, &payload);

        {
            RZScopedCriticalSection readyLock(readyCS);
            while (!waiterReady)
                readyCV.wait(&readyCS);
        }
        {
            RZScopedCriticalSection lock(cs);
            cv.signal();
        }

        rz_thread_join(handle);

        EXPECT_EQ(wakeCount, 2u);

        cv.destroy();
        cs.destroy();
        readyCV.destroy();
        readyCS.destroy();
    }

#if 0    // TODO: Re-enable once broadcast synchronization issue is resolved.
TEST_F(RZSyncConditionalVarTests, BroadcastWakesAll)
{
    constexpr uint32_t waiters = 6u;

    RZCriticalSection cs;
    cs.init();
    RZConditionalVar cv;
    cv.init();

    uint32_t wakeCount = 0u;
    uint32_t goFlag    = 0u;

    ThreadBarrier localBarrier;
    BarrierInit(localBarrier);

    RZDynamicArray<BroadcastPayload> payloads;
    payloads.resize(waiters);
    RZDynamicArray<RZThreadHandle> handles;
    handles.reserve(waiters);
    for (uint32_t i = 0; i < waiters; ++i) {
        payloads[i].cs        = &cs;
        payloads[i].cv        = &cv;
        payloads[i].barrier   = &localBarrier;
        payloads[i].wakeCount = &wakeCount;
        payloads[i].predicate = &goFlag;
        handles.push_back(rz_thread_create("CVBroadcast", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, BroadcastWaiter, &payloads[i]));
    }

    BarrierWaitForReady(localBarrier, waiters);
    BarrierSignalStart(localBarrier);
    {
        RZScopedCriticalSection lock(cs);
        goFlag = 1u;
        cv.broadcast();
    }
    BarrierWaitForDone(localBarrier, waiters);

    EXPECT_EQ(wakeCount, waiters);

    cv.destroy();
    cs.destroy();
    BarrierDestroy(localBarrier);
}
#endif

    TEST_F(RZSyncConditionalVarTests, TimedWaitTimesOut)
    {
        RZCriticalSection cs;
        cs.init();
        RZConditionalVar cv;
        cv.init();

        uint32_t       signals   = 0u;
        const uint32_t timeoutMs = 5u;
        const auto     start     = SteadyClock::now();
        {
            RZScopedCriticalSection lock(cs);
            cv.wait(&cs, timeoutMs);
            ++signals;
        }
        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(SteadyClock::now() - start);

        EXPECT_GE(elapsedMs.count(), 2);
        EXPECT_EQ(signals, 1u);

        cv.destroy();
        cs.destroy();
    }

    // Integrated producer-consumer -----------------------------------------------
    TEST_F(RZSyncIntegrationTests, ProducerConsumerNoLoss)
    {
        constexpr uint32_t itemCount  = 256u;
        constexpr uint32_t bufferSize = 32u;

        RZCriticalSection cs;
        cs.init();
        RZConditionalVar cv;
        cv.init();

        RZDynamicArray<uint32_t> buffer(bufferSize, 0u);
        uint32_t                 head = 0u;
        uint32_t                 tail = 0u;

        ProducerConsumerPayload producer{&cs, &cv, &buffer, &head, &tail, itemCount};
        ProducerConsumerPayload consumer{&cs, &cv, &buffer, &head, &tail, itemCount};

        RZThreadHandle producerHandle = rz_thread_create("Producer", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ProducerThread, &producer);
        RZThreadHandle consumerHandle = rz_thread_create("Consumer", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, ConsumerThread, &consumer);

        rz_thread_join(producerHandle);
        rz_thread_join(consumerHandle);

        EXPECT_EQ(head, itemCount);
        EXPECT_EQ(tail, itemCount);

        cv.destroy();
        cs.destroy();
    }

#if 0    // TODO: Re-enable once broadcast synchronization issue is resolved.
TEST_F(RZSyncIntegrationTests, BroadcastLatencyUnderThreshold)
{
    constexpr uint32_t waiters = 8u;

    RZCriticalSection cs;
    cs.init();
    RZConditionalVar cv;
    cv.init();

    ThreadBarrier barrier;
    BarrierInit(barrier);

    uint32_t wakeCount     = 0u;
    uint32_t fastWakeCount = 0u;
    uint32_t goFlag        = 0u;

    RZDynamicArray<BroadcastLatencyPayload> payloads;
    payloads.resize(waiters);
    RZDynamicArray<RZThreadHandle> handles;
    handles.reserve(waiters);
    for (uint32_t i = 0; i < waiters; ++i) {
        payloads[i].cs            = &cs;
        payloads[i].cv            = &cv;
        payloads[i].barrier       = &barrier;
        payloads[i].wakeCount     = &wakeCount;
        payloads[i].fastWakeCount = &fastWakeCount;
        payloads[i].predicate     = &goFlag;
        handles.push_back(rz_thread_create("CVLatency", RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_WORKER, BroadcastLatencyWaiter, &payloads[i]));
    }

    BarrierWaitForReady(barrier, waiters);
    BarrierSignalStart(barrier);
    const auto signalStart = SteadyClock::now();
    {
        RZScopedCriticalSection lock(cs);
        goFlag = 1u;
        cv.broadcast();
    }
    BarrierWaitForDone(barrier, waiters);
    const auto totalElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(SteadyClock::now() - signalStart);

    EXPECT_LE(totalElapsed.count(), 500);
    EXPECT_EQ(wakeCount, waiters);
    EXPECT_EQ(fastWakeCount, waiters);

    cv.destroy();
    cv.destroy();
    cs.destroy();
    BarrierDestroy(barrier);
#endif

}    // namespace Razix
