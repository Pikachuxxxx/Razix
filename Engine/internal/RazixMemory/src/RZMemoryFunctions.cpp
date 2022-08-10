#include "RZMemoryFunctions.h"

#include <corecrt_malloc.h>

namespace Razix {
    
    void* RZMalloc(size_t size, size_t alignment)
    {
        // TODO: Begin tracking allocation here

        void* address = nullptr;

#ifdef RAZIX_MEMORY_DEBUG
        address = RZDebugMalloc(size, alignment, NULL, 0, NULL);
#elif defined(RAZIX_PLATFORM_CONSOLE) || (RAZIX_PLATFORM_UNIX)
        address = memalign(size, alignment);
#elif defined(_WIN32)
        address = _aligned_malloc(size, alignment);
#endif

        return address;

        // TODO: End tracking allocation here
    }

    void* RZMalloc(size_t size)
    {
        // 16-byte makes it easy for AVX operations and SIMD so yeah we choose 16 for now
        return RZMalloc(size, 16);
    }

    void RZFree(void* address)
    {
        // TODO: Begin tracking allocation here

#ifdef RAZIX_MEMORY_DEBUG
        RZDebugFree(address);
#elif defined(RAZIX_PLATFORM_CONSOLE) || (RAZIX_PLATFORM_UNIX)
        free(address);
#elif defined(_WIN32)
        _aligned_free(address);
#endif
        // TODO: End tracking allocation here
    }

#ifdef RAZIX_MEMORY_DEBUG

    void* RZDebugMalloc(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag)
    {
        // In addition to the allocation we will append a memory info struct which will store more information about the allocation(s)
        // Plus reports the allocation that was done
        // We create a struct as mentioned and then add it to the total allocation size --> Init it with allocation info --> Append it --> Align it and boom sent it for usage --> user wont know it's a bit bigger
        size_t total_size = size; /* + sizeof(AllocationInfo) */
        return _aligned_malloc(size, alignment);
    }

    void RZDebugFree(void* address)
    {
        // Removes the allocation info that was done from the tracker
        // Add the allocation info size --> sets to 0xCD --> Free's the memory to the OS
        // Kinda works for now
        _aligned_free(address);
    }

#endif
}