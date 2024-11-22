#pragma once

#include "IRZAllocator.h"

#include <cstdint>

namespace Razix {
    namespace Memory {
        class RZStackAllocator : public IRZAllocator
        {
        public:
            RZStackAllocator()  = default;
            ~RZStackAllocator() = default;

            void init(size_t size) override;

            void shutdown() override;

            void* allocate(size_t size, size_t alignment) override;
            void deallocate(void* ptr) override;

            void clear() { m_AllocatedSize = 0; }

        private:
            uint8_t* m_StackChunk    = nullptr;
            size_t   m_AllocatedSize = 0;
            size_t   m_TotalSize     = 0;
        };
    }    // namespace Memory
}    // namespace Razix
