#pragma once

#include "Razix/Core/Memory/Allocators/IRZAllocator.h"
#include "Razix/Core/RZCore.h"

#include "Razix/Core/RZDataTypes.h"

namespace Razix {
    namespace Memory {
        class RAZIX_API RZLinearAllocator final : public IRZAllocator
        {
        public:
            RZLinearAllocator()  = default;
            ~RZLinearAllocator() = default;

            void init(size_t size, size_t alignment = 16) override;

            void shutdown() override;

            void* allocate(size_t size) override;
            void  deallocate(void* ptr) override {}

            void clear() { m_AllocatedSize = 0; }

            size_t getRemainingSize() const;
            size_t getAllocatedSize() const { return m_AllocatedSize; }
            size_t getTotalSize() const { return m_TotalSize; }

        private:
            uint8_t* m_Chunk         = nullptr;
            size_t   m_AllocatedSize = 0;
            size_t   m_TotalSize     = 0;
            size_t   m_Alignment     = 16;
        };
    }    // namespace Memory
}    // namespace Razix