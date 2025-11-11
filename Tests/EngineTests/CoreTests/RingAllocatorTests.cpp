// RingAllocatorTests.cpp
#include "Razix/Core/Memory/Allocators/RZRingAllocator.h"
#include <gtest/gtest.h>

namespace Razix {
    namespace Memory {

        class RZRingAllocatorTests : public ::testing::Test
        {
        protected:
            RZRingAllocator<int> allocator;
            const size_t totalSize = 10;

            void SetUp() override
            {
                allocator.init(totalSize);
            }

            void TearDown() override
            {
                allocator.shutdown();
            }
        };

        TEST_F(RZRingAllocatorTests, InitAndShutdown)
        {
            SUCCEED();
        }

        TEST_F(RZRingAllocatorTests, PutAndGet)
        {
            EXPECT_TRUE(allocator.empty());
            allocator.put(10);
            EXPECT_FALSE(allocator.empty());
            int val = allocator.get();
            EXPECT_EQ(val, 10);
            EXPECT_TRUE(allocator.empty());
        }

        TEST_F(RZRingAllocatorTests, IsFull)
        {
            EXPECT_FALSE(allocator.isFull());
            for (size_t i = 0; i < totalSize; ++i) {
                allocator.put((int)i);
            }
            EXPECT_TRUE(allocator.isFull());
        }

        TEST_F(RZRingAllocatorTests, WrapAround)
        {
            for (size_t i = 0; i < totalSize; ++i) {
                allocator.put((int)i);
            }
            EXPECT_TRUE(allocator.isFull());
            
            // First element should be 0
            int val = allocator.get();
            EXPECT_EQ(val, 0);

            // Overwrite first element
            allocator.put(100);
            EXPECT_TRUE(allocator.isFull());

            // First element should be 1 now
            val = allocator.get();
            EXPECT_EQ(val, 1);

            // Put another element
            allocator.put(101);
            EXPECT_TRUE(allocator.isFull()); 

            // Get all elements
            for (size_t i = 2; i < totalSize; ++i) {
                val = allocator.get();
                EXPECT_EQ(val, (int)i);
            }
            val = allocator.get();
            EXPECT_EQ(val, 100);
            val = allocator.get();
            EXPECT_EQ(val, 101);
            EXPECT_TRUE(allocator.empty());
        }

        TEST_F(RZRingAllocatorTests, Reset)
        {
            for (int i = 0; i < 5; ++i) {
                allocator.put(i);
            }
            EXPECT_FALSE(allocator.empty());
            EXPECT_FALSE(allocator.isFull());

            allocator.reset();
            EXPECT_TRUE(allocator.empty());
            EXPECT_FALSE(allocator.isFull());
        }
    }
}
