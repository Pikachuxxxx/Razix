#include "Razix/Core/Memory/Allocators/RZLinearAllocator.h"
#include <gtest/gtest.h>

namespace Razix {
    namespace Memory {

        class RZLinearAllocatorTests : public ::testing::Test
        {
        protected:
            RZLinearAllocator allocator;
            const size_t      totalSize = 1024;
            void              SetUp() override
            {
                allocator.init(totalSize);
            }

            void TearDown() override
            {
                allocator.shutdown();
            }
        };

        // ============================================================================
        // Basic Initialization and Shutdown Tests
        // ============================================================================

        TEST_F(RZLinearAllocatorTests, InitAndShutdown)
        {
            SUCCEED();
        }

        TEST_F(RZLinearAllocatorTests, InitializeWithValidSize)
        {
            RZLinearAllocator testAllocator;
            size_t            size = 2048;
            testAllocator.init(size);

            EXPECT_EQ(testAllocator.getTotalSize(), size);
            EXPECT_EQ(testAllocator.getAllocatedSize(), 0);
            EXPECT_EQ(testAllocator.getRemainingSize(), size);

            testAllocator.shutdown();
        }

        TEST_F(RZLinearAllocatorTests, InitializeWithLargeSize)
        {
            RZLinearAllocator testAllocator;
            size_t            size = 1024 * 1024 * 10;    // 10MB
            testAllocator.init(size);

            EXPECT_EQ(testAllocator.getTotalSize(), size);
            EXPECT_EQ(testAllocator.getAllocatedSize(), 0);
            EXPECT_EQ(testAllocator.getRemainingSize(), size);

            testAllocator.shutdown();
        }

        TEST_F(RZLinearAllocatorTests, ShutdownFreesMemory)
        {
            RZLinearAllocator testAllocator;
            testAllocator.init(1024);
            testAllocator.shutdown();

            EXPECT_EQ(testAllocator.getTotalSize(), 0);
            EXPECT_EQ(testAllocator.getAllocatedSize(), 0);
        }

        TEST_F(RZLinearAllocatorTests, ClearResetsAllocatedSize)
        {
            allocator.allocate(512);

            EXPECT_EQ(allocator.getAllocatedSize(), 512);
            EXPECT_EQ(allocator.getRemainingSize(), 512);

            allocator.clear();

            EXPECT_EQ(allocator.getAllocatedSize(), 0);
            EXPECT_EQ(allocator.getRemainingSize(), totalSize);
        }

        // ============================================================================
        // Basic Allocation Tests
        // ============================================================================

        TEST_F(RZLinearAllocatorTests, Allocate)
        {
            void* mem = allocator.allocate(128);
            EXPECT_NE(mem, nullptr);
        }

        TEST_F(RZLinearAllocatorTests, AllocateSingleBlock)
        {
            void* ptr = allocator.allocate(256);

            EXPECT_NE(ptr, nullptr);
            EXPECT_EQ(allocator.getAllocatedSize(), 256);
            EXPECT_EQ(allocator.getRemainingSize(), 768);
        }

        TEST_F(RZLinearAllocatorTests, AllocateMultipleBlocks)
        {
            void* ptr1 = allocator.allocate(256);
            void* ptr2 = allocator.allocate(256);
            void* ptr3 = allocator.allocate(256);

            EXPECT_NE(ptr1, nullptr);
            EXPECT_NE(ptr2, nullptr);
            EXPECT_NE(ptr3, nullptr);

            EXPECT_EQ(allocator.getAllocatedSize(), 768);
            EXPECT_EQ(allocator.getRemainingSize(), 256);
        }

        TEST_F(RZLinearAllocatorTests, AllocateExactSize)
        {
            void* ptr = allocator.allocate(totalSize);

            EXPECT_NE(ptr, nullptr);
            EXPECT_EQ(allocator.getAllocatedSize(), totalSize);
            EXPECT_EQ(allocator.getRemainingSize(), 0);
        }

        TEST_F(RZLinearAllocatorTests, AllocateZeroSize)
        {
            void* ptr1 = allocator.allocate(32);
            void* ptr2 = allocator.allocate(0);

            EXPECT_EQ(ptr1, ptr2);
            EXPECT_EQ(allocator.getAllocatedSize(), 32);
        }

        // ============================================================================
        // Out of Memory Tests
        // ============================================================================

        TEST_F(RZLinearAllocatorTests, OutOfMemory)
        {
            allocator.allocate(totalSize);
            void* mem = allocator.allocate(1);
            EXPECT_EQ(mem, nullptr);
        }

        TEST_F(RZLinearAllocatorTests, AllocateExceedsCapacity)
        {
            void* ptr = allocator.allocate(2048);

            EXPECT_EQ(ptr, nullptr);
            EXPECT_EQ(allocator.getAllocatedSize(), 0);
        }

        TEST_F(RZLinearAllocatorTests, AllocateMultipleUntilFull)
        {
            void* ptr1 = allocator.allocate(256);
            void* ptr2 = allocator.allocate(256);
            void* ptr3 = allocator.allocate(256);
            void* ptr4 = allocator.allocate(256);
            void* ptr5 = allocator.allocate(256);    // Should fail

            EXPECT_NE(ptr1, nullptr);
            EXPECT_NE(ptr2, nullptr);
            EXPECT_NE(ptr3, nullptr);
            EXPECT_NE(ptr4, nullptr);
            EXPECT_EQ(ptr5, nullptr);
            EXPECT_EQ(allocator.getAllocatedSize(), 1024);
        }

        // ============================================================================
        // Linear Behavior Tests
        // ============================================================================

        TEST_F(RZLinearAllocatorTests, PointersAreSequential)
        {
            void* ptr1 = allocator.allocate(256);
            void* ptr2 = allocator.allocate(256);
            void* ptr3 = allocator.allocate(256);

            EXPECT_LT((uintptr_t) ptr1, (uintptr_t) ptr2);
            EXPECT_LT((uintptr_t) ptr2, (uintptr_t) ptr3);
        }

        TEST_F(RZLinearAllocatorTests, PointersAreContiguous)
        {
            void* ptr1 = allocator.allocate(256);
            void* ptr2 = allocator.allocate(256);

            // ptr2 should be exactly 256 bytes after ptr1
            EXPECT_EQ((uint8_t*) ptr2, (uint8_t*) ptr1 + 256);
        }

        // ============================================================================
        // Clear and Reset Tests
        // ============================================================================

        TEST_F(RZLinearAllocatorTests, AllocateAndClear)
        {
            void* mem1 = allocator.allocate(128);
            EXPECT_NE(mem1, nullptr);

            allocator.clear();

            void* mem2 = allocator.allocate(128);
            EXPECT_NE(mem2, nullptr);
            EXPECT_EQ(mem1, mem2);
        }

        TEST_F(RZLinearAllocatorTests, AllocateMultiple)
        {
            void* ptr1 = allocator.allocate(256);
            allocator.clear();
            void* ptr2 = allocator.allocate(256);
            allocator.clear();
            void* ptr3 = allocator.allocate(256);

            EXPECT_EQ(ptr1, ptr2);
            EXPECT_EQ(ptr2, ptr3);
            EXPECT_EQ(allocator.getAllocatedSize(), 256);
        }

        // ============================================================================
        // Deallocate Tests
        // ============================================================================

        TEST_F(RZLinearAllocatorTests, DeallocateIsNoOp)
        {
            void* mem1 = allocator.allocate(128);
            allocator.deallocate(mem1);    // Should not affect anything
            void* mem2 = allocator.allocate(256);
            EXPECT_NE(mem2, nullptr);
            EXPECT_NE(mem1, mem2);
        }

        TEST_F(RZLinearAllocatorTests, AllocationAlignment)
        {
            RZLinearAllocator aligned_allocator;
            aligned_allocator.init(totalSize, 32);
            void* mem_32 = aligned_allocator.allocate(256);
            EXPECT_NE(mem_32, nullptr);
            // Note: Linear allocator might not guarantee alignment for each allocation if not designed for it
            // This test might need adjustment based on implementation
            // For now, we assume the base pointer is aligned.
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_32) % 32, 0);
            aligned_allocator.shutdown();
        }

        // ============================================================================
        // Write/Read Data Tests
        // ============================================================================

        TEST_F(RZLinearAllocatorTests, WriteAndReadData)
        {
            int* intPtr = (int*) allocator.allocate(sizeof(int) * 10);

            EXPECT_NE(intPtr, nullptr);

            // Write data
            for (int i = 0; i < 10; ++i) {
                intPtr[i] = i * 100;
            }

            // Read data
            for (int i = 0; i < 10; ++i) {
                EXPECT_EQ(intPtr[i], i * 100);
            }
        }

        TEST_F(RZLinearAllocatorTests, WriteMultipleTypes)
        {
            RZLinearAllocator testAllocator;
            testAllocator.init(2048);

            int*   intPtr   = (int*) testAllocator.allocate(sizeof(int) * 5);
            float* floatPtr = (float*) testAllocator.allocate(sizeof(float) * 5);
            char*  charPtr  = (char*) testAllocator.allocate(sizeof(char) * 20);

            EXPECT_NE(intPtr, nullptr);
            EXPECT_NE(floatPtr, nullptr);
            EXPECT_NE(charPtr, nullptr);

            // Write data
            intPtr[0]   = 42;
            floatPtr[0] = 3.14f;
            charPtr[0]  = 'A';

            // Read data
            EXPECT_EQ(intPtr[0], 42);
            EXPECT_FLOAT_EQ(floatPtr[0], 3.14f);
            EXPECT_EQ(charPtr[0], 'A');

            testAllocator.shutdown();
        }

        TEST_F(RZLinearAllocatorTests, AllocateMixedSizes)
        {
            void* ptr1 = allocator.allocate(64);
            void* ptr2 = allocator.allocate(128);
            void* ptr3 = allocator.allocate(32);

            EXPECT_NE(ptr1, nullptr);
            EXPECT_NE(ptr2, nullptr);
            EXPECT_NE(ptr3, nullptr);
            EXPECT_EQ(allocator.getAllocatedSize(), 224);
        }
    }    // namespace Memory
}    // namespace Razix