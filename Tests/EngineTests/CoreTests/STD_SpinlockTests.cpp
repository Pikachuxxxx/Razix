// sprintfTests.cpp
// AI-generated unit tests for the RZUUID classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/spinlock.h"

#include "Razix/Core/RZThreadCore.h"

#include <gtest/gtest.h>

#include <thread>
#include <vector>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

namespace Razix {

// Test Fixture
class rz_spin_lockTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        // Reset shared data before each test
        sharedCounter = 0;
        completedThreads = 0;
        // Initialize the spinlock (if needed, though it's likely zero-initialized by default or constructor if it had one, but it's a struct now)
        // Assuming zero-initialization is enough or we should have an init function.
        // Looking at previous code, it was just a struct with atomic flag.
        // Let's assume zero-init is fine for now as per C style structs usually.
        // Actually, let's explicitly zero it out to be safe if it's POD.
        memset(&testLock, 0, sizeof(rz_spin_lock));
    }

    void TearDown() override 
    {
        // Cleanup
    }

    // Shared test data
    rz_spin_lock testLock;
    std::atomic<int> sharedCounter{0};
    std::atomic<int> completedThreads{0};
};

// ============================================================================
// Basic Functionality Tests
// ============================================================================

TEST_F(rz_spin_lockTest, LockUnlock_BasicUsage)
{
    // Test basic lock/unlock sequence
    rz_spinlock_lock(&testLock);
    EXPECT_NO_THROW(rz_spinlock_unlock(&testLock));
}

TEST_F(rz_spin_lockTest, TryLock_SucceedsWhenUnlocked)
{
    // Should succeed when lock is available
    EXPECT_TRUE(rz_spinlock_try_lock(&testLock));
    rz_spinlock_unlock(&testLock);
}

TEST_F(rz_spin_lockTest, TryLock_FailsWhenLocked)
{
    // Lock first
    rz_spinlock_lock(&testLock);
    
    // Try lock should fail
    EXPECT_FALSE(rz_spinlock_try_lock(&testLock));
    
    rz_spinlock_unlock(&testLock);
}

TEST_F(rz_spin_lockTest, MultipleLockUnlock_Sequential)
{
    // Test multiple lock/unlock cycles
    for (int i = 0; i < 100; ++i) {
        rz_spinlock_lock(&testLock);
        rz_spinlock_unlock(&testLock);
    }
    
    // Should still be able to lock
    EXPECT_TRUE(rz_spinlock_try_lock(&testLock));
    rz_spinlock_unlock(&testLock);
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_F(rz_spin_lockTest, MutualExclusion_TwoThreads)
{
    const int iterations = 10000;
    
    auto threadFunc = [&]() {
        for (int i = 0; i < iterations; ++i) {
            rz_spinlock_lock(&testLock);
            sharedCounter++;
            rz_spinlock_unlock(&testLock);
        }
    };
    
    std::thread t1(threadFunc);
    std::thread t2(threadFunc);
    
    t1.join();
    t2.join();
    
    // Without proper synchronization, this would likely fail
    EXPECT_EQ(sharedCounter.load(), iterations * 2);
}

TEST_F(rz_spin_lockTest, MutualExclusion_MultipleThreads)
{
    const int numThreads = 8;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    
    auto threadFunc = [&]() {
        for (int i = 0; i < iterations; ++i) {
            rz_spinlock_lock(&testLock);
            sharedCounter++;
            rz_spinlock_unlock(&testLock);
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
// RZScopedSpinLock Tests
// ============================================================================
class rz_spin_lockScopedTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        memset(&testLock, 0, sizeof(rz_spin_lock));
    }

    rz_spin_lock testLock;
    std::atomic<int> sharedCounter{0};
};

TEST_F(rz_spin_lockScopedTest, RAII_LocksOnConstruction)
{
    bool lockAcquired = false;
    
    {
        RZScopedSpinLock scopedLock(testLock);
        lockAcquired = true;
        
        // Lock should be held here
        // Trying to acquire should fail from another context
        EXPECT_FALSE(rz_spinlock_try_lock(&testLock));
    }
    
    EXPECT_TRUE(lockAcquired);
    
    // After scope, lock should be released
    EXPECT_TRUE(rz_spinlock_try_lock(&testLock));
    rz_spinlock_unlock(&testLock);
}

TEST_F(rz_spin_lockScopedTest, RAII_UnlocksOnDestruction)
{
    {
        RZScopedSpinLock scopedLock(testLock);
        // Lock is held
    }
    // Lock should be released
    
    EXPECT_TRUE(rz_spinlock_try_lock(&testLock));
    rz_spinlock_unlock(&testLock);
}

TEST_F(rz_spin_lockScopedTest, RAII_UnlocksOnException)
{
    try {
        RZScopedSpinLock scopedLock(testLock);
        throw std::runtime_error("Test exception");
    } catch (...) {
        // Exception caught
    }
    
    // Lock should still be released
    EXPECT_TRUE(rz_spinlock_try_lock(&testLock));
    rz_spinlock_unlock(&testLock);
}

TEST_F(rz_spin_lockScopedTest, NestedScopes_Sequential)
{
    {
        RZScopedSpinLock lock1(testLock);
        sharedCounter++;
    }
    
    {
        RZScopedSpinLock lock2(testLock);
        sharedCounter++;
    }
    
    EXPECT_EQ(sharedCounter.load(), 2);
}

TEST_F(rz_spin_lockScopedTest, MultiThreaded_WithScopedLock)
{
    const int numThreads = 8;
    const int iterations = 1000;
    std::vector<std::thread> threads;
    
    auto threadFunc = [&]() {
        for (int i = 0; i < iterations; ++i) {
            RZScopedSpinLock lock(testLock);
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

TEST_F(rz_spin_lockTest, MemoryVisibility_WriteBeforeLock_ReadAfterLock)
{
    std::atomic<bool> ready{false};
    int data = 0;
    
    std::thread writer([&]() {
        data = 42;
        rz_spinlock_lock(&testLock);
        ready = true;
        rz_spinlock_unlock(&testLock);
    });
    
    std::thread reader([&]() {
        while (!ready.load()) {
            std::this_thread::yield();
        }
        rz_spinlock_lock(&testLock);
        EXPECT_EQ(data, 42); // Should see the write
        rz_spinlock_unlock(&testLock);
    });
    
    writer.join();
    reader.join();
}

TEST_F(rz_spin_lockTest, MemoryVisibility_ProtectedWrites)
{
    std::vector<int> sharedData;
    const int numThreads = 4;
    const int itemsPerThread = 100;
    std::vector<std::thread> threads;
    
    auto threadFunc = [&](int threadId) {
        for (int i = 0; i < itemsPerThread; ++i) {
            rz_spinlock_lock(&testLock);
            sharedData.push_back(threadId * 1000 + i);
            rz_spinlock_unlock(&testLock);
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

TEST_F(rz_spin_lockTest, Stress_RapidLockUnlock)
{
    const int iterations = 100000;
    
    for (int i = 0; i < iterations; ++i) {
        rz_spinlock_lock(&testLock);
        rz_spinlock_unlock(&testLock);
    }
    
    // Should complete without deadlock
    SUCCEED();
}

TEST_F(rz_spin_lockTest, Stress_ManyThreadsShortCriticalSection)
{
    const int numThreads = 32;
    const int iterations = 100;
    std::vector<std::thread> threads;
    
    auto threadFunc = [&]() {
        for (int i = 0; i < iterations; ++i) {
            rz_spinlock_lock(&testLock);
            sharedCounter++;
            rz_spinlock_unlock(&testLock);
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

TEST_F(rz_spin_lockTest, Alignment_CacheLineAligned)
{
    // Verify cache line alignment
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&testLock) % RAZIX_CACHE_LINE_SIZE, 0);
}

TEST_F(rz_spin_lockTest, Size_MatchesCacheLine)
{
    // Verify size to prevent false sharing
    EXPECT_EQ(sizeof(rz_spin_lock), RAZIX_CACHE_LINE_SIZE);
}

TEST_F(rz_spin_lockScopedTest, Alignment_CacheLineAligned)
{
    RZScopedSpinLock scoped(testLock);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&scoped) % RAZIX_CACHE_LINE_SIZE, 0);
}

} // namespace Razix
