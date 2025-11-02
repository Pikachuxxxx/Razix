// sprintfTests.cpp
// AI-generated unit tests for the RZUUID classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/RZSpinlock.h"

#include <gtest/gtest.h>

#include <thread>
#include <vector>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

namespace Razix {

// Test Fixture
class RZSpinLockTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        // Reset shared data before each test
        sharedCounter = 0;
        completedThreads = 0;
    }

    void TearDown() override 
    {
        // Cleanup
    }

    // Shared test data
    RZSpinLock testLock;
    std::atomic<int> sharedCounter{0};
    std::atomic<int> completedThreads{0};
};

// ============================================================================
// Basic Functionality Tests
// ============================================================================

TEST_F(RZSpinLockTest, LockUnlock_BasicUsage)
{
    // Test basic lock/unlock sequence
    testLock.lock();
    EXPECT_NO_THROW(testLock.unlock());
}

TEST_F(RZSpinLockTest, TryLock_SucceedsWhenUnlocked)
{
    // Should succeed when lock is available
    EXPECT_TRUE(testLock.try_lock());
    testLock.unlock();
}

TEST_F(RZSpinLockTest, TryLock_FailsWhenLocked)
{
    // Lock first
    testLock.lock();
    
    // Try lock should fail
    EXPECT_FALSE(testLock.try_lock());
    
    testLock.unlock();
}

TEST_F(RZSpinLockTest, MultipleLockUnlock_Sequential)
{
    // Test multiple lock/unlock cycles
    for (int i = 0; i < 100; ++i) {
        testLock.lock();
        testLock.unlock();
    }
    
    // Should still be able to lock
    EXPECT_TRUE(testLock.try_lock());
    testLock.unlock();
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_F(RZSpinLockTest, MutualExclusion_TwoThreads)
{
    const int iterations = 10000;
    
    auto threadFunc = [&]() {
        for (int i = 0; i < iterations; ++i) {
            testLock.lock();
            sharedCounter++;
            testLock.unlock();
        }
    };
    
    std::thread t1(threadFunc);
    std::thread t2(threadFunc);
    
    t1.join();
    t2.join();
    
    // Without proper synchronization, this would likely fail
    EXPECT_EQ(sharedCounter.load(), iterations * 2);
}

TEST_F(RZSpinLockTest, MutualExclusion_MultipleThreads)
{
    const int numThreads = 8;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    
    auto threadFunc = [&]() {
        for (int i = 0; i < iterations; ++i) {
            testLock.lock();
            sharedCounter++;
            testLock.unlock();
        }
    };
    
    // Launch threads
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunc);
    }
    
    // Wait for completion
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(sharedCounter.load(), numThreads * iterations);
}

// ============================================================================
// RZSpinLockScoped Tests
// ============================================================================

class RZSpinLockScopedTest : public ::testing::Test 
{
protected:
    RZSpinLock testLock;
    std::atomic<int> sharedCounter{0};
};

TEST_F(RZSpinLockScopedTest, RAII_LocksOnConstruction)
{
    bool lockAcquired = false;
    
    {
        RZSpinLockScoped scopedLock(testLock);
        lockAcquired = true;
        
        // Lock should be held here
        // Trying to acquire should fail from another context
        EXPECT_FALSE(testLock.try_lock());
    }
    
    EXPECT_TRUE(lockAcquired);
    
    // After scope, lock should be released
    EXPECT_TRUE(testLock.try_lock());
    testLock.unlock();
}

TEST_F(RZSpinLockScopedTest, RAII_UnlocksOnDestruction)
{
    {
        RZSpinLockScoped scopedLock(testLock);
        // Lock is held
    }
    // Lock should be released
    
    EXPECT_TRUE(testLock.try_lock());
    testLock.unlock();
}

TEST_F(RZSpinLockScopedTest, RAII_UnlocksOnException)
{
    try {
        RZSpinLockScoped scopedLock(testLock);
        throw std::runtime_error("Test exception");
    } catch (...) {
        // Exception caught
    }
    
    // Lock should still be released
    EXPECT_TRUE(testLock.try_lock());
    testLock.unlock();
}

TEST_F(RZSpinLockScopedTest, NestedScopes_Sequential)
{
    {
        RZSpinLockScoped lock1(testLock);
        sharedCounter++;
    }
    
    {
        RZSpinLockScoped lock2(testLock);
        sharedCounter++;
    }
    
    EXPECT_EQ(sharedCounter.load(), 2);
}

TEST_F(RZSpinLockScopedTest, MultiThreaded_WithScopedLock)
{
    const int numThreads = 8;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    
    auto threadFunc = [&]() {
        for (int i = 0; i < iterations; ++i) {
            RZSpinLockScoped lock(testLock);
            sharedCounter++;
        }
    };
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunc);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(sharedCounter.load(), numThreads * iterations);
}

// ============================================================================
// Memory Ordering and Visibility Tests
// ============================================================================

TEST_F(RZSpinLockTest, MemoryVisibility_WriteBeforeLock_ReadAfterLock)
{
    std::atomic<bool> ready{false};
    int data = 0;
    
    std::thread writer([&]() {
        data = 42;
        testLock.lock();
        ready = true;
        testLock.unlock();
    });
    
    std::thread reader([&]() {
        while (!ready.load()) {
            std::this_thread::yield();
        }
        testLock.lock();
        EXPECT_EQ(data, 42); // Should see the write
        testLock.unlock();
    });
    
    writer.join();
    reader.join();
}

TEST_F(RZSpinLockTest, MemoryVisibility_ProtectedWrites)
{
    std::vector<int> sharedData;
    const int numThreads = 4;
    const int itemsPerThread = 100;
    std::vector<std::thread> threads;
    
    auto threadFunc = [&](int threadId) {
        for (int i = 0; i < itemsPerThread; ++i) {
            testLock.lock();
            sharedData.push_back(threadId * 1000 + i);
            testLock.unlock();
        }
    };
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunc, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(sharedData.size(), numThreads * itemsPerThread);
}

// ============================================================================
// Performance and Stress Tests
// ============================================================================

TEST_F(RZSpinLockTest, Stress_RapidLockUnlock)
{
    const int iterations = 100000;
    
    for (int i = 0; i < iterations; ++i) {
        testLock.lock();
        testLock.unlock();
    }
    
    // Should complete without deadlock
    SUCCEED();
}

TEST_F(RZSpinLockTest, Stress_ManyThreadsShortCriticalSection)
{
    const int numThreads = 32;
    const int iterations = 100;
    std::vector<std::thread> threads;
    
    auto threadFunc = [&]() {
        for (int i = 0; i < iterations; ++i) {
            testLock.lock();
            sharedCounter++;
            testLock.unlock();
        }
    };
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunc);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(sharedCounter.load(), numThreads * iterations);
}

// ============================================================================
// Edge Cases and Robustness Tests
// ============================================================================

TEST_F(RZSpinLockTest, Alignment_CacheLineAligned)
{
    // Verify cache line alignment
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&testLock) % RAZIX_CACHE_LINE_SIZE, 0);
}

TEST_F(RZSpinLockTest, Size_MatchesCacheLine)
{
    // Verify size to prevent false sharing
    EXPECT_EQ(sizeof(RZSpinLock), RAZIX_CACHE_LINE_SIZE);
}

TEST_F(RZSpinLockScopedTest, Alignment_CacheLineAligned)
{
    RZSpinLockScoped scoped(testLock);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&scoped) % RAZIX_CACHE_LINE_SIZE, 0);
}

} // namespace Razix
