#ifndef _RZ_MEMORY_FUNCTIONS_H_
#define _RZ_MEMORY_FUNCTIONS_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

    /**
     * These the normal (aka fast) and debug version of custom memory allocation functions
     * 
     * Razix customized rz_malloc_aligned and rz_free, we provide fast versions of malloc and free 
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

    RAZIX_API void*  rz_malloc(size_t size, size_t alignment);
    RAZIX_API void*  rz_malloc_aligned(size_t size);
    RAZIX_API void*  rz_malloc_debug(size_t size, const char* filename, uint32_t lineNumber, const char* tag);
    RAZIX_API void*  rz_malloc_debug_aligned(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag);
    RAZIX_API void*  rz_mem_copy_to_heap(void* data, size_t size);
    RAZIX_API void*  rz_realloc(void* oldPtr, size_t newSize, size_t alignment);
    RAZIX_API void*  rz_realloc_aligned(void* oldPtr, size_t newSize);
    RAZIX_API void*  rz_calloc(size_t count, size_t size, size_t alignment);
    RAZIX_API void*  rz_calloc_aligned(size_t count, size_t size);
    RAZIX_API void   rz_free(void* address);
    RAZIX_API size_t rz_mem_align(size_t size, size_t alignment);

#ifdef RAZIX_MEMORY_DEBUG
    RAZIX_API void* rz_debug_malloc(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag);
    RAZIX_API void  rz_debug_free(void* address);
#endif

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif    // _RZ_MEMORY_FUNCTIONS_H_