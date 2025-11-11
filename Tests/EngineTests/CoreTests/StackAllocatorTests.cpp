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
            void* mem = allocator.allocate(128);
            EXPECT_NE(mem, nullptr);
        }

        TEST_F(RZStackAllocatorTests, AllocateAndClear)
        {
            void* mem1 = allocator.allocate(128);
            EXPECT_NE(mem1, nullptr);
            
            allocator.clear();

            void* mem2 = allocator.allocate(128);
            EXPECT_NE(mem2, nullptr);
            EXPECT_EQ(mem1, mem2);
        }

        TEST_F(RZStackAllocatorTests, AllocateMultiple)
        {
            void* mem1 = allocator.allocate(128);
            EXPECT_NE(mem1, nullptr);
            void* mem2 = allocator.allocate(256);
            EXPECT_NE(mem2, nullptr);
            EXPECT_NE(mem1, mem2);
        }

        TEST_F(RZStackAllocatorTests, AllocationAlignment)
        {
            RZStackAllocator aligned_allocator;
            aligned_allocator.init(totalSize, 32);
            void* mem_32 = aligned_allocator.allocate(256);
            EXPECT_NE(mem_32, nullptr);
            // Note: Stack allocator might not guarantee alignment for each allocation if not designed for it
            // This test might need adjustment based on implementation
            // For now, we assume the base pointer is aligned.
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_32) % 32, 0);
            aligned_allocator.shutdown();
        }

        TEST_F(RZStackAllocatorTests, OutOfMemory)
        {
            allocator.allocate(totalSize);
            void* mem = allocator.allocate(1);
            EXPECT_EQ(mem, nullptr);
        }

        TEST_F(RZStackAllocatorTests, DeallocateIsNoOp)
        {
            void* mem1 = allocator.allocate(128);
            void* mem2 = allocator.allocate(128);
            allocator.deallocate(mem2); // Should not affect anything
            allocator.deallocate(mem1); // Should not affect anything
            
            void* mem3 = allocator.allocate(256);
            EXPECT_NE(mem3, nullptr);
        }
    }
}
