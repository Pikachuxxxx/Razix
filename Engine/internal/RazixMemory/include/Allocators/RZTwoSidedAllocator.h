#pragma once

#include "IRZAllocator.h"

#include <cstdint>

namespace Razix {
    namespace Memory {

        class RZTwoSidedAllocator : public IRZAllocator
        {
        public:
            RZTwoSidedAllocator()  = default;
            ~RZTwoSidedAllocator() = default;

            void init(size_t chunkSize) override;
            void shutdown() override;

            void* allocate(size_t size, size_t alignment) override { return nullptr; }
            void  deallocate(void* ptr) override {}

            void* allocate_top(size_t size, size_t alignment);
            void* allocate_bottom(size_t size, size_t alignment);

            void deallocate_top(size_t size);
            void deallocate_bottom(size_t size);

            void clear_top();
            void clear_bottom();

        private:
            uint8_t* m_Chunk     = nullptr; /* Allocator chunk                                */
            size_t   m_TotalSize = 0;       /* Total size of the allocator                    */
            size_t   m_Top       = 0;       /* Marker from top of the TwoSidedAllocator       */
            size_t   m_Bottom    = 0;       /* Marker from bottom of the TwoSidedAllocator    */
        };

    }    // namespace Memory
}    // namespace Razix