#pragma once

#include "Razix/Core/RZCore.h"
#include "IRZAllocator.h"

#include "Razix/Core/RZDataTypes.h"

namespace Razix {
    namespace Memory {

        class RAZIX_API RZHeapAllocator final : public IRZAllocator
        {
        public:
            RZHeapAllocator()  = default;
            ~RZHeapAllocator() = default;

            void init(size_t size, size_t alignment = 16) override;
            void shutdown() override;

            void* allocate(size_t size) override;

            void deallocate(void* ptr) override;

            const uint64_t getChunkStartAddress() const { return (uint64_t) m_ChunkAddress; }
            const void*    getCurrentAllocation() { return m_CurrentAllocation; }

        private:
            void*  m_TLSFHandle;
            void*  m_ChunkAddress;
            size_t m_AllocatedSize     = 0;
            size_t m_TotalChunkSize    = 0;
            size_t m_Alignment         = 16;
            void*  m_CurrentAllocation = nullptr;
        };

    }    // namespace Memory
}    // namespace Razix
