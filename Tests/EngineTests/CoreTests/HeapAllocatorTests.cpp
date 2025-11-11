// HeapAllocatorTests.cpp
#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"
#include <gtest/gtest.h>

namespace Razix {
    namespace Memory {

        class RZHeapAllocatorTests : public ::testing::Test
        {
        protected:
            RZHeapAllocator allocator;
            const size_t chunkSize = 1024 * 10; // 10 KB

            void SetUp() override
            {
                allocator.init(chunkSize);
            }

            void TearDown() override
            {
                allocator.shutdown();
            }
        };

        TEST_F(RZHeapAllocatorTests, InitAndShutdown)
        {
            SUCCEED();
        }

        TEST_F(RZHeapAllocatorTests, AllocateAndDeallocate)
        {
            void* mem = allocator.allocate(128, 8);
            EXPECT_NE(mem, nullptr);
            allocator.deallocate(mem);
        }

        TEST_F(RZHeapAllocatorTests, AllocateMultiple)
        {
            void* mem1 = allocator.allocate(128, 8);
            EXPECT_NE(mem1, nullptr);
            void* mem2 = allocator.allocate(256, 16);
            EXPECT_NE(mem2, nullptr);
            EXPECT_NE(mem1, mem2);
            allocator.deallocate(mem1);
            allocator.deallocate(mem2);
        }

        TEST_F(RZHeapAllocatorTests, AllocationAlignment)
        {
            void* mem_16 = allocator.allocate(128, 16);
            EXPECT_NE(mem_16, nullptr);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_16) % 16, 0);
            allocator.deallocate(mem_16);

            void* mem_32 = allocator.allocate(256, 32);
            EXPECT_NE(mem_32, nullptr);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_32) % 32, 0);
            allocator.deallocate(mem_32);
        }

        TEST_F(RZHeapAllocatorTests, ZeroSizeAllocation)
        {
            void* mem = allocator.allocate(0, 8);
            EXPECT_EQ(mem, nullptr);
        }
    }
}
