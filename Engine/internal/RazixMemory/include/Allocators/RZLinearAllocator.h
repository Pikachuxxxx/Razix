#pragma once

#include "IRZAllocator.h"

#include <cstdint>

namespace Razix {
    namespace Memory {
        class RZLinearAllocator : public IRZAllocator
        {
        public:
            RZLinearAllocator() = default;
            ~RZLinearAllocator() = default;

            void init(size_t size) override;

            void shutdown() override;

            void* allocate(size_t size, size_t alignment) override;
            /* This allocator does not allocate on a per-pointer basis */
            void  deallocate(void* ptr) override {}

            void clear() { m_AllocatedSize = 0; }

        private:
            uint8_t* m_Chunk    = nullptr;
            size_t   m_AllocatedSize = 0;
            size_t   m_TotalSize     = 0;
        };
    }    // namespace Memory
}    // namespace Razix
