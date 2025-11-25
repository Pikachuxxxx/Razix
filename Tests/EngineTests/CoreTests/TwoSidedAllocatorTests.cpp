// TwoSidedAllocatorTests.cpp
#include "Razix/Core/Memory/Allocators/RZTwoSidedAllocator.h"
#include <gtest/gtest.h>

namespace Razix {
    namespace Memory {

        class RZTwoSidedAllocatorTests : public ::testing::Test
        {
        protected:
            RZTwoSidedAllocator allocator;
            const size_t totalSize = 1024;

            void SetUp() override
            {
                allocator.init(totalSize);
            }

            void TearDown() override
            {
                allocator.shutdown();
            }
        };

        TEST_F(RZTwoSidedAllocatorTests, InitAndShutdown)
        {
            SUCCEED();
        }

        TEST_F(RZTwoSidedAllocatorTests, AllocateTop)
        {
            void* mem = allocator.allocate_top(128);
            EXPECT_NE(mem, nullptr);
        }

        TEST_F(RZTwoSidedAllocatorTests, AllocateBottom)
        {
            void* mem = allocator.allocate_bottom(128);
            EXPECT_NE(mem, nullptr);
        }

        TEST_F(RZTwoSidedAllocatorTests, AllocateBoth)
        {
            void* top_mem = allocator.allocate_top(128);
            EXPECT_NE(top_mem, nullptr);
            void* bottom_mem = allocator.allocate_bottom(128);
            EXPECT_NE(bottom_mem, nullptr);
            EXPECT_NE(top_mem, bottom_mem);
        }

        TEST_F(RZTwoSidedAllocatorTests, TopAlignment)
        {
            RZTwoSidedAllocator aligned_allocator;
            aligned_allocator.init(totalSize, 32);
            void* mem_32 = aligned_allocator.allocate_top(128);
            EXPECT_NE(mem_32, nullptr);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_32) % 16, 0);
            aligned_allocator.shutdown();
        }

        TEST_F(RZTwoSidedAllocatorTests, BottomAlignment)
        {
            RZTwoSidedAllocator aligned_allocator;
            aligned_allocator.init(totalSize, 32);
            void* mem_32 = aligned_allocator.allocate_bottom(128);
            EXPECT_NE(mem_32, nullptr);
            // Bottom allocations are trickier to align. This will depend on implementation.
            // This test might fail and need adjustment.
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_32) % 16, 0);
            aligned_allocator.shutdown();
        }

        TEST_F(RZTwoSidedAllocatorTests, OutOfMemory)
        {
            allocator.allocate_top(totalSize / 2);
            allocator.allocate_bottom(totalSize / 2);
            void* mem = allocator.allocate_top(1);
            EXPECT_EQ(mem, nullptr);
            mem = allocator.allocate_bottom(1);
            EXPECT_EQ(mem, nullptr);
        }

        TEST_F(RZTwoSidedAllocatorTests, DeallocateTop)
        {
            void* mem1 = allocator.allocate_top(128);
            allocator.deallocate_top(128);
            void* mem2 = allocator.allocate_top(128);
            EXPECT_EQ(mem1, mem2);
        }

        TEST_F(RZTwoSidedAllocatorTests, DeallocateBottom)
        {
            void* mem1 = allocator.allocate_bottom(128);
            allocator.deallocate_bottom(128);
            void* mem2 = allocator.allocate_bottom(128);
            EXPECT_EQ(mem1, mem2);
        }

        TEST_F(RZTwoSidedAllocatorTests, Clear)
        {
            void* top_mem1 = allocator.allocate_top(128);
            void* bottom_mem1 = allocator.allocate_bottom(128);
            
            allocator.clear_top();
            allocator.clear_bottom();

            void* top_mem2 = allocator.allocate_top(128);
            void* bottom_mem2 = allocator.allocate_bottom(128);

            EXPECT_EQ(top_mem1, top_mem2);
            EXPECT_EQ(bottom_mem1, bottom_mem2);
        }
    }
}
