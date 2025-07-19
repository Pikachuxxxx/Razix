#pragma once

#include "RZMemoryConfig.h"

#include <cstdint>
#include <stddef.h>

namespace Razix {
    namespace Memory {

        /**
         * These the normal (aka fast) and debug version of custom memory allocation functions
         * 
         * Razix customized RZMalloc and RZFree, we provide fast versions of malloc and free 
         * along with pool based version that requires the client to register memory pools 
         * that will be used to allocate memory from based on categorical/flag options 
         * 
         * This pool based model provides efficient packing and stores memory together 
         * 
         * We also have faster versions of malloc and free without the pool design that 
         * will be used by the custom allocators to manage the memory by themselves
         * 
         * We will also provide with config macros to switch between debug/tracking 
         * mechanisms + warning while allocations
         */

        /**
         * Allocates a block of memory at the given alignment
         * 
         * @param size The size of the memory block to allocate
         * @param alignment The alignment of the memory block
         * @returns address to the aligned contiguous block of memory 
         */
        void* RZMalloc(size_t size, size_t alignment);
        /**
         * Allocated a block of memory at 16-byte alignment
         * 
         * @note: 16-byte makes it easy for AVX operations
         * 
         * @param size The size of the memory block to allocate
         * @returns address to the aligned contiguous block of memory
         */
        void* RZMalloc(size_t size);
        void* RZMalloc(size_t size, const char* filename, uint32_t lineNumber, const char* tag);
        void* RZMalloc(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag);
        void* RZMemCopyToHeap(void* data, size_t size);
        void* RZRealloc(void* oldPtr, size_t newSize, size_t alignment);
        void* RZRealloc(void* oldPtr, size_t newSize);
        void* RZCalloc(size_t count, size_t size, size_t alignment);
        void* RZCalloc(size_t count, size_t size);

        void RZFree(void* address);

        /**
         * Calculates the aligned memory size 
         * 
         * @param size Size of the allocation
         * @param alignment The alignment of the allocation to be done
         * 
         * @returns The Aligned memory size
         */
        size_t RZMemAlign(size_t size, size_t alignment);

#ifdef RAZIX_MEMORY_DEBUG

        void* RZDebugMalloc(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag);

        void RZDebugFree(void* address);

#endif
    }    // namespace Memory
}    // namespace Razix
