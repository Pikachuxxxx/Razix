#include "Razix/Core/Memory/RZMemoryFunctions.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <corecrt_malloc.h>
#elif RAZIX_PLATFORM_UNIX
    #include <memory.h>
    #include <stdlib.h>
#endif

#include <string.h>    // for memset, memcpy

void* rz_malloc(size_t size, size_t alignment)
{
    // TODO: Begin tracking allocation here

    void* address = NULL;

#ifdef RAZIX_DEBUG
    address = rz_debug_malloc(size, alignment, NULL, 0, NULL);
#else
    #ifdef RAZIX_PLATFORM_WINDOWS
    address = _aligned_malloc(size, alignment);
    #elif RAZIX_PLATFORM_UNIX
    if (posix_memalign(&address, alignment, size) != 0) {
        address = NULL;
    }
    #endif
#endif

    // Zero out the allocated memory
    if (address)
        memset(address, 0, size);

    return address;

    // TODO: End tracking allocation here
}

void* rz_malloc_aligned(size_t size)
{
    // 16-byte makes it easy for AVX operations and SIMD so yeah we choose 16 for now
    return rz_malloc(size, 16);
}

void* rz_malloc_debug(size_t size, const char* filename, uint32_t lineNumber, const char* tag)
{
    return rz_malloc(size, 16);
}

void* rz_malloc_debug_aligned(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag)
{
    return rz_malloc(size, alignment);
}

void* rz_mem_copy_to_heap(void* data, size_t size)
{
    uint8_t* heapData = (uint8_t*) rz_malloc(size, RAZIX_16B_ALIGN);
    if (heapData && data)
        memcpy(heapData, data, size);
    return heapData;
}

void* rz_realloc(void* oldPtr, size_t newSize, size_t alignment)
{
    if (newSize == 0) {
        rz_free(oldPtr);
        return NULL;
    }

    if (!oldPtr) {
        return rz_malloc(newSize, alignment);
    }

#ifdef RAZIX_PLATFORM_WINDOWS
    oldPtr = _aligned_realloc(oldPtr, newSize, alignment);
#elif RAZIX_PLATFORM_UNIX
    void* newPtr = NULL;
    if (posix_memalign(&newPtr, alignment, newSize) != 0) {
        newPtr = NULL;
        oldPtr = NULL;
    }
    if (newPtr) {
        memcpy(newPtr, oldPtr, newSize);
        free(oldPtr);
        oldPtr = newPtr;
    } else {
        oldPtr = NULL;
    }
#endif

    return oldPtr;
}

void* rz_realloc_aligned(void* oldPtr, size_t newSize)
{
    return rz_realloc(oldPtr, newSize, 16);
}

void* rz_calloc(size_t count, size_t size, size_t alignment)
{
    size_t totalSize = count * size;
    return rz_malloc(totalSize, alignment);    // already zeroes out memory
}

void* rz_calloc_aligned(size_t count, size_t size)
{
    return rz_calloc(count, size, 16);
}

void rz_free(void* address)
{
    // TODO: Begin tracking allocation here

#ifdef RAZIX_DEBUG
    rz_debug_free(address);
#else
    if (address) {
    #ifdef RAZIX_PLATFORM_WINDOWS
        _aligned_free(address);
    #elif RAZIX_PLATFORM_UNIX
        free(address);
    #endif
    }
#endif
    // TODO: End tracking allocation here
}

size_t rz_mem_align(size_t size, size_t alignment)
{
    const size_t alignment_mask = alignment - 1;
    return (size + alignment_mask) & ~alignment_mask;
}

#ifdef RAZIX_DEBUG

void* rz_debug_malloc(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag)
{
    // In addition to the allocation we will append a memory info struct which will store more information about the allocation(s)
    // Plus reports the allocation that was done
    // We create a struct as mentioned and then add it to the total allocation size --> Init it with allocation info --> Append it --> Align it and boom sent it for usage --> user wont know it's a bit bigger
    //size_t total_size = size; /* + sizeof(AllocationInfo) */

    void* addr = NULL;

    #ifdef RAZIX_PLATFORM_WINDOWS
    addr = _aligned_malloc(size, alignment);
    #elif RAZIX_PLATFORM_UNIX
    posix_memalign(&addr, alignment, size);
    #endif

    //printf("[Memory Alloc]  sz : %zu | alignment : %zu | tag : %s | addr : %llx", size, alignment, tag, (uintptr_t) addr);
    return addr;
}

void rz_debug_free(void* address)
{
    // Removes the allocation info that was done from the tracker
    // Add the allocation info size --> sets to 0xCD --> Free's the memory to the OS
    // Kinda works for now
    if (address) {
    #ifdef RAZIX_PLATFORM_WINDOWS
        _aligned_free(address);
    #elif RAZIX_PLATFORM_UNIX
        free(address);
    #endif
    }
}

#endif
