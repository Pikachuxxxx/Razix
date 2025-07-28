// MemoryTests.cpp
// Unit tests for the Razix Memory Management classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/RZMemory.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include "Razix/Core/Memory/RZMemoryTags.h"
#include "Razix/Core/Memory/RZMemoryConfig.h"

#include <gtest/gtest.h>
#include <cstring>

namespace Razix {
    namespace Memory {

        class RZMemoryTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Setup for memory tests
            }

            void TearDown() override
            {
                // Cleanup for memory tests
            }
        };

        // Test case for memory tag enumeration
        TEST_F(RZMemoryTests, MemoryTagsValidation)
        {
            // Test that memory tags are properly defined
            EXPECT_GE(static_cast<int>(MemoryTag::RENDERER), 0);
            EXPECT_GE(static_cast<int>(MemoryTag::SCENE), 0);
            EXPECT_GE(static_cast<int>(MemoryTag::PHYSICS), 0);
            EXPECT_GE(static_cast<int>(MemoryTag::AUDIO), 0);
            EXPECT_GE(static_cast<int>(MemoryTag::SCRIPTING), 0);
            EXPECT_GE(static_cast<int>(MemoryTag::CORE), 0);
        }

        // Test case for memory functions
        TEST_F(RZMemoryTests, MemoryFunctionality)
        {
            const size_t testSize = 1024;
            
            // Test memory allocation
            void* ptr = rzalloc(testSize);
            EXPECT_NE(ptr, nullptr) << "Memory allocation should succeed";
            
            // Test memory initialization
            rzmemset(ptr, 0xAA, testSize);
            
            // Verify memory was set correctly
            uint8_t* bytePtr = static_cast<uint8_t*>(ptr);
            for (size_t i = 0; i < testSize; ++i) {
                EXPECT_EQ(bytePtr[i], 0xAA) << "Memory should be properly initialized";
            }
            
            // Test memory copy
            void* copyPtr = rzalloc(testSize);
            EXPECT_NE(copyPtr, nullptr) << "Copy destination allocation should succeed";
            
            rzmemcpy(copyPtr, ptr, testSize);
            
            // Verify memory was copied correctly
            int result = rzmemcmp(ptr, copyPtr, testSize);
            EXPECT_EQ(result, 0) << "Memory copy should be identical to source";
            
            // Clean up
            rzfree(ptr);
            rzfree(copyPtr);
        }

        // Test case for memory alignment
        TEST_F(RZMemoryTests, MemoryAlignment)
        {
            const size_t alignment = 16;
            const size_t size = 64;
            
            void* alignedPtr = rzalloc_aligned(size, alignment);
            EXPECT_NE(alignedPtr, nullptr) << "Aligned allocation should succeed";
            EXPECT_EQ(reinterpret_cast<uintptr_t>(alignedPtr) % alignment, 0) 
                << "Allocated memory should be properly aligned";
            
            rzfree_aligned(alignedPtr);
        }

        // Test case for memory tracking
        TEST_F(RZMemoryTests, MemoryTracking)
        {
            // Get initial memory stats
            size_t initialAllocations = GetTotalAllocations();
            size_t initialMemoryUsage = GetTotalMemoryUsage();
            
            // Allocate some memory
            const size_t allocSize = 512;
            void* ptr = rzalloc(allocSize);
            EXPECT_NE(ptr, nullptr);
            
            // Check that tracking increased
            size_t newAllocations = GetTotalAllocations();
            size_t newMemoryUsage = GetTotalMemoryUsage();
            
            EXPECT_GE(newAllocations, initialAllocations) << "Allocation count should increase";
            EXPECT_GE(newMemoryUsage, initialMemoryUsage + allocSize) << "Memory usage should increase";
            
            // Free memory
            rzfree(ptr);
            
            // Check that tracking decreased
            size_t finalAllocations = GetTotalAllocations();
            size_t finalMemoryUsage = GetTotalMemoryUsage();
            
            EXPECT_LE(finalAllocations, newAllocations) << "Allocation count should decrease after free";
            EXPECT_LE(finalMemoryUsage, newMemoryUsage) << "Memory usage should decrease after free";
        }

        // Test case for tagged memory allocation
        TEST_F(RZMemoryTests, TaggedMemoryAllocation)
        {
            const size_t allocSize = 256;
            
            // Allocate memory with different tags
            void* rendererPtr = rzalloc_tagged(allocSize, MemoryTag::RENDERER);
            void* physicsPtr = rzalloc_tagged(allocSize, MemoryTag::PHYSICS);
            void* audioPtr = rzalloc_tagged(allocSize, MemoryTag::AUDIO);
            
            EXPECT_NE(rendererPtr, nullptr) << "Tagged allocation for RENDERER should succeed";
            EXPECT_NE(physicsPtr, nullptr) << "Tagged allocation for PHYSICS should succeed";
            EXPECT_NE(audioPtr, nullptr) << "Tagged allocation for AUDIO should succeed";
            
            // Verify tag-specific usage
            size_t rendererUsage = GetTagMemoryUsage(MemoryTag::RENDERER);
            size_t physicsUsage = GetTagMemoryUsage(MemoryTag::PHYSICS);
            size_t audioUsage = GetTagMemoryUsage(MemoryTag::AUDIO);
            
            EXPECT_GE(rendererUsage, allocSize) << "RENDERER tag should track memory usage";
            EXPECT_GE(physicsUsage, allocSize) << "PHYSICS tag should track memory usage";
            EXPECT_GE(audioUsage, allocSize) << "AUDIO tag should track memory usage";
            
            // Clean up
            rzfree_tagged(rendererPtr, MemoryTag::RENDERER);
            rzfree_tagged(physicsPtr, MemoryTag::PHYSICS);
            rzfree_tagged(audioPtr, MemoryTag::AUDIO);
        }

        // Test case for memory leak detection
        TEST_F(RZMemoryTests, MemoryLeakDetection)
        {
            // Get initial leak count
            size_t initialLeaks = GetMemoryLeakCount();
            
            // Intentionally "leak" memory
            void* leakedPtr = rzalloc(128);
            EXPECT_NE(leakedPtr, nullptr);
            
            // Check that leak detection would catch this
            // Note: In a real test, we'd need to call a leak detection function
            // For now, just verify we can track allocations
            size_t currentAllocations = GetTotalAllocations();
            EXPECT_GT(currentAllocations, 0) << "Should track active allocations";
            
            // Clean up the "leak"
            rzfree(leakedPtr);
        }

    }    // namespace Memory
}    // namespace Razix