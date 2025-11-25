// MemoryFunctionsTests.cpp
#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include <gtest/gtest.h>
#include <string.h>

namespace Razix {
    namespace Memory {

        class RZMemoryFunctionsTests : public ::testing::Test
        {
        };

        TEST_F(RZMemoryFunctionsTests, MallocAndFree)
        {
            void* mem = rz_malloc(128, 8);
            EXPECT_NE(mem, nullptr);
            rz_free(mem);
        }

        TEST_F(RZMemoryFunctionsTests, MallocAligned)
        {
            void* mem = rz_malloc_aligned(128);
            EXPECT_NE(mem, nullptr);
            rz_free(mem);
        }

        TEST_F(RZMemoryFunctionsTests, Alignment)
        {
            void* mem_16 = rz_malloc(128, 16);
            EXPECT_NE(mem_16, nullptr);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_16) % 16, 0);
            rz_free(mem_16);

            void* mem_32 = rz_malloc(256, 32);
            EXPECT_NE(mem_32, nullptr);
            EXPECT_EQ(reinterpret_cast<uintptr_t>(mem_32) % 32, 0);
            rz_free(mem_32);
        }

        TEST_F(RZMemoryFunctionsTests, MemAlign)
        {
            size_t aligned_size = rz_mem_align(123, 16);
            EXPECT_EQ(aligned_size, 128);
            aligned_size = rz_mem_align(128, 16);
            EXPECT_EQ(aligned_size, 128);
        }

        TEST_F(RZMemoryFunctionsTests, Realloc)
        {
            char* mem = (char*)rz_malloc(128, 8);
            EXPECT_NE(mem, nullptr);
            strcpy(mem, "Hello");

            char* new_mem = (char*)rz_realloc(mem, 256, 8);
            EXPECT_NE(new_mem, nullptr);
            EXPECT_STREQ(new_mem, "Hello");
            rz_free(new_mem);
        }
        
        TEST_F(RZMemoryFunctionsTests, ReallocAligned)
        {
            char* mem = (char*)rz_malloc_aligned(128);
            EXPECT_NE(mem, nullptr);
            strcpy(mem, "Hello");

            char* new_mem = (char*)rz_realloc_aligned(mem, 256);
            EXPECT_NE(new_mem, nullptr);
            EXPECT_STREQ(new_mem, "Hello");
            rz_free(new_mem);
        }

        TEST_F(RZMemoryFunctionsTests, Calloc)
        {
            size_t count = 10;
            size_t size = sizeof(int);
            int* mem = (int*)rz_calloc(count, size, 16);
            EXPECT_NE(mem, nullptr);
            for(size_t i = 0; i < count; ++i) {
                EXPECT_EQ(mem[i], 0);
            }
            rz_free(mem);
        }

        TEST_F(RZMemoryFunctionsTests, CallocAligned)
        {
            size_t count = 10;
            size_t size = sizeof(int);
            int* mem = (int*)rz_calloc_aligned(count, size);
            EXPECT_NE(mem, nullptr);
            for(size_t i = 0; i < count; ++i) {
                EXPECT_EQ(mem[i], 0);
            }
            rz_free(mem);
        }

        TEST_F(RZMemoryFunctionsTests, MemCopyToHeap)
        {
            char data[] = "Test data";
            size_t size = sizeof(data);
            void* new_mem = rz_mem_copy_to_heap(data, size);
            EXPECT_NE(new_mem, nullptr);
            EXPECT_NE(new_mem, data);
            EXPECT_EQ(memcmp(new_mem, data, size), 0);
            rz_free(new_mem);
        }
    }
}
