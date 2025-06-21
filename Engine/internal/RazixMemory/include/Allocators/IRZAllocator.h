#pragma once

#include <stddef.h>

namespace Razix {
    namespace Memory {

        /**
         * Interface for creating various allocators 
         * 
         * Note: This cannot be used for creating STL allocator
         */
        class IRZAllocator
        {
        public:
            IRZAllocator()          = default;
            virtual ~IRZAllocator() = default;

            /* Initialize the allocator */
            virtual void init(size_t size) = 0;
            /* Shutdown the allocator and release all it's resources safely and verify */
            virtual void shutdown() = 0;

            /* Allocated the given size memory as the given alignment as per the allocator design */
            virtual void* allocate(size_t size, size_t alignment) = 0;
            /* Debug version of the allocator */
            virtual void* allocate(size_t size, size_t alignment, const char* name, const char* file, int line) { return 0; }

            /* Deallocate the pointer from the allocator as per the design */
            virtual void deallocate(void* ptr) = 0;
        };
    }    // namespace Memory
}    // namespace Razix
