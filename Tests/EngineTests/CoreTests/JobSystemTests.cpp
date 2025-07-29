// JobSystemTests.cpp
// Unit tests for the RZJobSystem
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Threading/RZJobSystem.h"

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>

namespace Razix {

    class RZJobSystemTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Initialize job system for tests
            JobSystem::OnInit();
        }

        void TearDown() override
        {
            // Cleanup after tests
        }
    };

    // Test case for JobDispatchArgs structure
    TEST_F(RZJobSystemTests, JobDispatchArgsStructure)
    {
        JobSystem::JobDispatchArgs args;
        
        // Test that all fields are accessible
        args.jobIndex = 5;
        args.groupID = 10;
        args.groupIndex = 2;
        args.isFirstJobInGroup = true;
        args.isLastJobInGroup = false;
        args.sharedmemory = nullptr;
        
        EXPECT_EQ(args.jobIndex, 5);
        EXPECT_EQ(args.groupID, 10);
        EXPECT_EQ(args.groupIndex, 2);
        EXPECT_TRUE(args.isFirstJobInGroup);
        EXPECT_FALSE(args.isLastJobInGroup);
        EXPECT_EQ(args.sharedmemory, nullptr);
    }

    // Test case for Context structure
    TEST_F(RZJobSystemTests, ContextStructure)
    {
        JobSystem::Context ctx;
        
        // Test atomic counter initialization
        EXPECT_EQ(ctx.counter.load(), 0);
        
        // Test atomic operations
        ctx.counter++;
        EXPECT_EQ(ctx.counter.load(), 1);
        
        ctx.counter.store(42);
        EXPECT_EQ(ctx.counter.load(), 42);
    }

    // Test case for thread count retrieval
    TEST_F(RZJobSystemTests, ThreadCount)
    {
        u32 threadCount = JobSystem::GetThreadCount();
        
        // Thread count should be reasonable (at least 1, typically hardware_concurrency)
        EXPECT_GT(threadCount, 0);
        EXPECT_LE(threadCount, 128); // Reasonable upper bound for testing
    }

    // Test case for dispatch group count calculation
    TEST_F(RZJobSystemTests, DispatchGroupCount)
    {
        // Test various job count and group size combinations
        u32 groups1 = JobSystem::DispatchGroupCount(10, 3);
        EXPECT_EQ(groups1, 4); // ceil(10/3) = 4
        
        u32 groups2 = JobSystem::DispatchGroupCount(9, 3);
        EXPECT_EQ(groups2, 3); // ceil(9/3) = 3
        
        u32 groups3 = JobSystem::DispatchGroupCount(1, 1);
        EXPECT_EQ(groups3, 1); // ceil(1/1) = 1
        
        u32 groups4 = JobSystem::DispatchGroupCount(100, 10);
        EXPECT_EQ(groups4, 10); // ceil(100/10) = 10
    }

    // Test case for simple job execution
    TEST_F(RZJobSystemTests, SimpleJobExecution)
    {
        JobSystem::Context ctx;
        std::atomic<int> counter{0};
        
        // Execute a simple job
        JobSystem::Execute(ctx, [&counter](JobSystem::JobDispatchArgs args) {
            counter++;
        });
        
        // Wait for completion
        JobSystem::Wait(ctx);
        
        // Verify job was executed
        EXPECT_EQ(counter.load(), 1);
        EXPECT_FALSE(JobSystem::IsBusy(ctx));
    }

    // Test case for multiple job execution
    TEST_F(RZJobSystemTests, MultipleJobExecution)
    {
        JobSystem::Context ctx;
        std::atomic<int> counter{0};
        const int jobCount = 10;
        
        // Execute multiple jobs
        for (int i = 0; i < jobCount; ++i) {
            JobSystem::Execute(ctx, [&counter](JobSystem::JobDispatchArgs args) {
                counter++;
            });
        }
        
        // Wait for all jobs to complete
        JobSystem::Wait(ctx);
        
        // Verify all jobs were executed
        EXPECT_EQ(counter.load(), jobCount);
        EXPECT_FALSE(JobSystem::IsBusy(ctx));
    }

    // Test case for job dispatch
    TEST_F(RZJobSystemTests, JobDispatch)
    {
        JobSystem::Context ctx;
        std::atomic<int> counter{0};
        const u32 jobCount = 20;
        const u32 groupSize = 4;
        
        // Dispatch jobs
        JobSystem::Dispatch(ctx, jobCount, groupSize, [&counter](JobSystem::JobDispatchArgs args) {
            counter++;
        });
        
        // Wait for completion
        JobSystem::Wait(ctx);
        
        // Verify all jobs were executed
        EXPECT_EQ(counter.load(), jobCount);
        EXPECT_FALSE(JobSystem::IsBusy(ctx));
    }

    // Test case for job dispatch with shared memory
    TEST_F(RZJobSystemTests, JobDispatchWithSharedMemory)
    {
        JobSystem::Context ctx;
        std::atomic<int> counter{0};
        const u32 jobCount = 8;
        const u32 groupSize = 2;
        const sz sharedMemorySize = 64;
        
        // Dispatch jobs with shared memory
        JobSystem::Dispatch(ctx, jobCount, groupSize, [&counter](JobSystem::JobDispatchArgs args) {
            counter++;
            // Verify shared memory is accessible (non-null when requested)
            if (args.sharedmemory != nullptr) {
                // Just verify we can access it without crashing
                volatile char* mem = static_cast<char*>(args.sharedmemory);
                *mem = 42; // Write test value
            }
        }, sharedMemorySize);
        
        // Wait for completion
        JobSystem::Wait(ctx);
        
        // Verify all jobs were executed
        EXPECT_EQ(counter.load(), jobCount);
        EXPECT_FALSE(JobSystem::IsBusy(ctx));
    }

    // Test case for job dispatch arguments validation
    TEST_F(RZJobSystemTests, JobDispatchArgsValidation)
    {
        JobSystem::Context ctx;
        const u32 jobCount = 6;
        const u32 groupSize = 2;
        
        std::vector<u32> jobIndices;
        std::vector<u32> groupIndices;
        
        // Dispatch jobs and collect dispatch arguments
        JobSystem::Dispatch(ctx, jobCount, groupSize, [&](JobSystem::JobDispatchArgs args) {
            jobIndices.push_back(args.jobIndex);
            groupIndices.push_back(args.groupIndex);
        });
        
        // Wait for completion
        JobSystem::Wait(ctx);
        
        // Verify job indices are in expected range
        EXPECT_EQ(jobIndices.size(), jobCount);
        for (u32 index : jobIndices) {
            EXPECT_LT(index, jobCount);
        }
        
        // Verify group indices are reasonable
        u32 expectedGroups = JobSystem::DispatchGroupCount(jobCount, groupSize);
        for (u32 groupIndex : groupIndices) {
            EXPECT_LT(groupIndex, expectedGroups);
        }
    }

    // Test case for busy state checking
    TEST_F(RZJobSystemTests, BusyStateChecking)
    {
        JobSystem::Context ctx;
        std::atomic<bool> jobStarted{false};
        std::atomic<bool> shouldComplete{false};
        
        // Execute a job that waits
        JobSystem::Execute(ctx, [&](JobSystem::JobDispatchArgs args) {
            jobStarted = true;
            while (!shouldComplete.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
        
        // Wait for job to start
        while (!jobStarted.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        // Should be busy while job is running
        EXPECT_TRUE(JobSystem::IsBusy(ctx));
        
        // Signal job to complete
        shouldComplete = true;
        
        // Wait for completion
        JobSystem::Wait(ctx);
        
        // Should not be busy after completion
        EXPECT_FALSE(JobSystem::IsBusy(ctx));
    }

    // Test case for concurrent contexts
    TEST_F(RZJobSystemTests, ConcurrentContexts)
    {
        JobSystem::Context ctx1, ctx2;
        std::atomic<int> counter1{0}, counter2{0};
        
        // Execute jobs on different contexts
        JobSystem::Execute(ctx1, [&counter1](JobSystem::JobDispatchArgs args) {
            counter1++;
        });
        
        JobSystem::Execute(ctx2, [&counter2](JobSystem::JobDispatchArgs args) {
            counter2++;
        });
        
        // Wait for both contexts
        JobSystem::Wait(ctx1);
        JobSystem::Wait(ctx2);
        
        // Verify both jobs completed
        EXPECT_EQ(counter1.load(), 1);
        EXPECT_EQ(counter2.load(), 1);
        EXPECT_FALSE(JobSystem::IsBusy(ctx1));
        EXPECT_FALSE(JobSystem::IsBusy(ctx2));
    }

}    // namespace Razix