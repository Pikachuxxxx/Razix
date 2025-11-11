// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZLinearAllocator.h"

#include "Razix/Core/Memory/RZAllocationMetrics.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include <iostream>

namespace Razix {
    namespace Memory {

        void RZLinearAllocator::init(size_t size)
        {
            m_AllocatedSize = 0;
            m_TotalSize     = size;
        }

        void RZLinearAllocator::shutdown()
        {
            clear();
            rz_free(m_Chunk);
            m_Chunk         = NULL;
            m_AllocatedSize = 0;
            m_TotalSize     = 0;
        }

        void* RZLinearAllocator::allocate(size_t size, size_t alignment)
        {
            if (m_Chunk == NULL) {
                m_Chunk = (uint8_t*) rz_malloc(m_TotalSize, alignment);
            }

            if (size == 0)
                return NULL;

            size_t currentAddr = (size_t) (m_Chunk + m_AllocatedSize);

            if (m_AllocatedSize + size > m_TotalSize) {
                return NULL;
            }

            m_AllocatedSize += size;

            return (void*) (m_Chunk + m_AllocatedSize);
        }

        size_t RZLinearAllocator::getRemainingSize() const
        {
            return (m_AllocatedSize >= m_TotalSize) ? 0 : (m_TotalSize - m_AllocatedSize);
        }

    }    // namespace Memory
}    // namespace Razix
