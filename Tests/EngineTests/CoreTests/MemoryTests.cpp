// MemoryTests.cpp
// Unit tests for the Razix Memory Management classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"

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

        // Test case for basic RZMalloc and RZFree functionality
        TEST_F(RZMemoryTests, BasicAllocationDeallocation)
        {
            const size_t testSize = 1024;
            
            // Test basic allocation with default 16-byte alignment
            void* ptr = RZMalloc(testSize);
            EXPECT_NE(ptr, nullptr) << "RZMalloc should succeed for valid size";
            
            // Verify 16-byte alignment (default)
            EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % 16, 0) 
                << "Default RZMalloc should provide 16-byte alignment";
            
            // Test that we can write and read from allocated memory
            memset(ptr, 0xAA, testSize);
            uint8_t* bytePtr = static_cast<uint8_t*>(ptr);
            for (size_t i = 0; i < testSize; ++i) {
                EXPECT_EQ(bytePtr[i], 0xAA) << "Allocated memory should be writable";
            }
            
            // Clean up
            RZFree(ptr);
        }

        // Test case for aligned memory allocation
        TEST_F(RZMemoryTests, AlignedAllocation)
        {
            const size_t testSize = 512;
            const size_t alignment32 = 32;
            const size_t alignment64 = 64;
            
            // Test 32-byte aligned allocation
            void* ptr32 = RZMalloc(testSize, alignment32);
            EXPECT_NE(ptr32, nullptr) << "32-byte aligned allocation should succeed";
            EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr32) % alignment32, 0) 
                << "Memory should be 32-byte aligned";
            
            // Test 64-byte aligned allocation
            void* ptr64 = RZMalloc(testSize, alignment64);
            EXPECT_NE(ptr64, nullptr) << "64-byte aligned allocation should succeed";
            EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr64) % alignment64, 0) 
                << "Memory should be 64-byte aligned";
            
            // Clean up
            RZFree(ptr32);
            RZFree(ptr64);
        }

        // Test case for debug allocation with tracking information
        TEST_F(RZMemoryTests, DebugAllocation)
        {
            const size_t testSize = 256;
            const char* filename = __FILE__;
            const uint32_t lineNumber = __LINE__;
            const char* tag = "TEST_TAG";
            
            // Test debug allocation with tracking
            void* ptr = RZMalloc(testSize, filename, lineNumber, tag);
            EXPECT_NE(ptr, nullptr) << "Debug allocation should succeed";
            
            // Verify 16-byte alignment is maintained
            EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % 16, 0) 
                << "Debug allocation should maintain 16-byte alignment";
            
            // Clean up
            RZFree(ptr);
        }

        // Test case for debug aligned allocation with tracking
        TEST_F(RZMemoryTests, DebugAlignedAllocation)
        {
            const size_t testSize = 128;
            const size_t alignment = 32;
            const char* filename = __FILE__;
            const uint32_t lineNumber = __LINE__;
            const char* tag = "ALIGNED_TEST";
            
            // Test debug aligned allocation
            void* ptr = RZMalloc(testSize, alignment, filename, lineNumber, tag);
            EXPECT_NE(ptr, nullptr) << "Debug aligned allocation should succeed";
            EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignment, 0) 
                << "Debug allocation should respect custom alignment";
            
            // Clean up
            RZFree(ptr);
        }

        // Test case for memory alignment calculation
        TEST_F(RZMemoryTests, MemoryAlignmentCalculation)
        {
            // Test alignment calculation for various sizes and alignments
            EXPECT_EQ(RZMemAlign(100, 16), 112) << "100 bytes should align to 112 with 16-byte alignment";
            EXPECT_EQ(RZMemAlign(128, 16), 128) << "128 bytes should remain 128 with 16-byte alignment";
            EXPECT_EQ(RZMemAlign(129, 16), 144) << "129 bytes should align to 144 with 16-byte alignment";
            
            EXPECT_EQ(RZMemAlign(100, 32), 128) << "100 bytes should align to 128 with 32-byte alignment";
            EXPECT_EQ(RZMemAlign(64, 32), 64) << "64 bytes should remain 64 with 32-byte alignment";
            EXPECT_EQ(RZMemAlign(65, 32), 96) << "65 bytes should align to 96 with 32-byte alignment";
        }

        // Test case for zero-size allocation
        TEST_F(RZMemoryTests, ZeroSizeAllocation)
        {
            // Test zero-size allocation behavior
            void* ptr = RZMalloc(0);
            // The behavior for zero-size allocation can vary by implementation
            // Some implementations return nullptr, others return a valid pointer
            // We just verify it doesn't crash and if non-null, can be freed
            if (ptr != nullptr) {
                RZFree(ptr);
            }
            // Test passes if no crash occurs
            SUCCEED();
        }

        // Test case for multiple allocations and frees
        TEST_F(RZMemoryTests, MultipleAllocations)
        {
            const size_t numAllocations = 10;
            const size_t allocationSize = 64;
            void* ptrs[numAllocations];
            
            // Allocate multiple blocks
            for (size_t i = 0; i < numAllocations; ++i) {
                ptrs[i] = RZMalloc(allocationSize);
                EXPECT_NE(ptrs[i], nullptr) << "Allocation " << i << " should succeed";
                
                // Write unique pattern to each block
                memset(ptrs[i], static_cast<int>(i), allocationSize);
            }
            
            // Verify each block still has its unique pattern
            for (size_t i = 0; i < numAllocations; ++i) {
                uint8_t* bytePtr = static_cast<uint8_t*>(ptrs[i]);
                for (size_t j = 0; j < allocationSize; ++j) {
                    EXPECT_EQ(bytePtr[j], static_cast<uint8_t>(i)) 
                        << "Block " << i << " should maintain its data integrity";
                }
            }
            
            // Free all blocks
            for (size_t i = 0; i < numAllocations; ++i) {
                RZFree(ptrs[i]);
            }
        }

#ifdef RAZIX_MEMORY_DEBUG
        // Test case for debug memory functions (only if debug mode is enabled)
        TEST_F(RZMemoryTests, DebugMemoryFunctions)
        {
            const size_t testSize = 256;
            const size_t alignment = 16;
            const char* filename = __FILE__;
            const uint32_t lineNumber = __LINE__;
            const char* tag = "DEBUG_TEST";
            
            // Test debug malloc
            void* ptr = RZDebugMalloc(testSize, alignment, filename, lineNumber, tag);
            EXPECT_NE(ptr, nullptr) << "RZDebugMalloc should succeed";
            EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignment, 0) 
                << "Debug allocation should respect alignment";
            
            // Test debug free
            RZDebugFree(ptr);
            // Test passes if no crash occurs during debug free
        }
#endif

    }    // namespace Memory
}    // namespace Razix