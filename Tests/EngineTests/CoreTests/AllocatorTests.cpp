// AllocatorTests.cpp
// Unit tests for the Razix Memory Allocator classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZAllocators.h"
#include "Razix/Core/Memory/Allocators/RZStackAllocator.h"
#include "Razix/Core/Memory/Allocators/RZLinearAllocator.h"
#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"
#include "Razix/Core/Memory/Allocators/RZRingAllocator.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace Memory {

        class RZAllocatorTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Setup for allocator tests
            }

            void TearDown() override
            {
                // Cleanup for allocator tests
            }
        };

        // Test case for RZStackAllocator
        TEST_F(RZAllocatorTests, StackAllocatorBasicFunctionality)
        {
            RZStackAllocator allocator;
            
            // Test initialization
            allocator.init(1024);
            
            // Test basic allocation
            void* ptr1 = allocator.allocate(64, 8);
            EXPECT_NE(ptr1, nullptr) << "Stack allocator should successfully allocate memory";
            
            void* ptr2 = allocator.allocate(128, 16);
            EXPECT_NE(ptr2, nullptr) << "Stack allocator should successfully allocate memory";
            EXPECT_NE(ptr1, ptr2) << "Allocated pointers should be different";
            
            // Test clear operation
            allocator.clear();
            
            // Should be able to allocate at the beginning again
            void* ptr3 = allocator.allocate(32, 4);
            EXPECT_NE(ptr3, nullptr) << "Stack allocator should successfully allocate after clear";
            
            allocator.shutdown();
        }

        // Test case for RZLinearAllocator
        TEST_F(RZAllocatorTests, LinearAllocatorBasicFunctionality)
        {
            RZLinearAllocator allocator;
            
            // Test initialization
            allocator.init(1024);
            
            // Test basic allocation
            void* ptr1 = allocator.allocate(64, 8);
            EXPECT_NE(ptr1, nullptr) << "Linear allocator should successfully allocate memory";
            
            void* ptr2 = allocator.allocate(128, 16);
            EXPECT_NE(ptr2, nullptr) << "Linear allocator should successfully allocate memory";
            EXPECT_GT(static_cast<char*>(ptr2), static_cast<char*>(ptr1)) << "Linear allocator should allocate sequentially";
            
            // Test reset operation
            allocator.reset();
            
            // Should be able to allocate at the beginning again
            void* ptr3 = allocator.allocate(32, 4);
            EXPECT_NE(ptr3, nullptr) << "Linear allocator should successfully allocate after reset";
            
            allocator.shutdown();
        }

        // Test case for RZHeapAllocator
        TEST_F(RZAllocatorTests, HeapAllocatorBasicFunctionality)
        {
            RZHeapAllocator allocator;
            
            // Test initialization
            allocator.init(1024);
            
            // Test basic allocation and deallocation
            void* ptr1 = allocator.allocate(64, 8);
            EXPECT_NE(ptr1, nullptr) << "Heap allocator should successfully allocate memory";
            
            void* ptr2 = allocator.allocate(128, 16);
            EXPECT_NE(ptr2, nullptr) << "Heap allocator should successfully allocate memory";
            
            // Test deallocation
            allocator.deallocate(ptr1);
            allocator.deallocate(ptr2);
            
            // Should be able to allocate after deallocation
            void* ptr3 = allocator.allocate(256, 32);
            EXPECT_NE(ptr3, nullptr) << "Heap allocator should successfully allocate after deallocation";
            
            allocator.deallocate(ptr3);
            allocator.shutdown();
        }

        // Test case for RZRingAllocator
        TEST_F(RZAllocatorTests, RingAllocatorBasicFunctionality)
        {
            RZRingAllocator allocator;
            
            // Test initialization
            allocator.init(1024);
            
            // Test basic allocation
            void* ptr1 = allocator.allocate(64, 8);
            EXPECT_NE(ptr1, nullptr) << "Ring allocator should successfully allocate memory";
            
            void* ptr2 = allocator.allocate(128, 16);
            EXPECT_NE(ptr2, nullptr) << "Ring allocator should successfully allocate memory";
            
            // Test deallocation
            allocator.deallocate(ptr1);
            allocator.deallocate(ptr2);
            
            allocator.shutdown();
        }

        // Test case for allocation alignment
        TEST_F(RZAllocatorTests, AllocationAlignment)
        {
            RZStackAllocator allocator;
            allocator.init(1024);
            
            // Test different alignments
            void* ptr8 = allocator.allocate(32, 8);
            EXPECT_NE(ptr8, nullptr);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr8) % 8, 0) << "Allocation should be 8-byte aligned";
            
            allocator.clear();
            
            void* ptr16 = allocator.allocate(32, 16);
            EXPECT_NE(ptr16, nullptr);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr16) % 16, 0) << "Allocation should be 16-byte aligned";
            
            allocator.shutdown();
        }

        // Test case for allocation size validation
        TEST_F(RZAllocatorTests, AllocationSizeValidation)
        {
            RZLinearAllocator allocator;
            allocator.init(1024);
            
            // Test zero size allocation
            void* ptr_zero = allocator.allocate(0, 4);
            EXPECT_EQ(ptr_zero, nullptr) << "Zero size allocation should return nullptr";
            
            // Test normal size allocation
            void* ptr_normal = allocator.allocate(64, 4);
            EXPECT_NE(ptr_normal, nullptr) << "Normal size allocation should succeed";
            
            allocator.shutdown();
        }

    }    // namespace Memory
}    // namespace Razix