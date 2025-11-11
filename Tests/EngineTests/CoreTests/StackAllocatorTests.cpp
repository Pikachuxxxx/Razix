// StackAllocatorTests.cpp
#include "Razix/Core/Memory/Allocators/RZStackAllocator.h"
#include <gtest/gtest.h>

namespace Razix {
    namespace Memory {

        class RZStackAllocatorTests : public ::testing::Test
        {
        protected:
            RZStackAllocator allocator;
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

        TEST_F(RZStackAllocatorTests, InitAndShutdown)
        {
            SUCCEED();
        }

        TEST_F(RZStackAllocatorTests, Allocate)
        {
            void* mem = allocator.allocate(128, 8);
            EXPECT_NE(mem, nullptr);
        }

        TEST_F(RZStackAllocatorTests, AllocateAndClear)
        {
            void* mem1 = allocator.allocate(128, 8);
            EXPECT_NE(mem1, nullptr);
            
            allocator.clear();

            void* mem2 = allocator.allocate(128, 8);
            EXPECT_NE(mem2, nullptr);
            EXPECT_EQ(mem1, mem2);
        }

        TEST_F(RZStackAllocatorTests, AllocateMultiple)
        {
            void* mem1 = allocator.allocate(128, 8);
            EXPECT_NE(mem1, nullptr);
            void* mem2 = allocator.allocate(256, 16);
            EXPECT_NE(mem2, nullptr);
            EXPECT_NE(mem1, mem2);
        }

        TEST_F(RZStackAllocatorTests, AllocationAlignment)
        {
            void* mem_16 = allocator.allocate(128, 16);
            EXPECT_NE(mem_16, nullptr);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_16) % 16, 0);
        }

        TEST_F(RZStackAllocatorTests, OutOfMemory)
        {
            allocator.allocate(totalSize, 8);
            void* mem = allocator.allocate(1, 8);
            EXPECT_EQ(mem, nullptr);
        }

        TEST_F(RZStackAllocatorTests, DeallocateIsNoOp)
        {
            void* mem1 = allocator.allocate(128, 8);
            void* mem2 = allocator.allocate(128, 8);
            allocator.deallocate(mem2); // Should not affect anything
            allocator.deallocate(mem1); // Should not affect anything
            
            void* mem3 = allocator.allocate(256, 8);
            EXPECT_NE(mem3, nullptr);
        }
    }
}
