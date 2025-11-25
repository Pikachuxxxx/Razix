// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Core/Memory/Allocators/RZLinearAllocator.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

namespace Razix {
    namespace Memory {

        void RZLinearAllocator::init(size_t size, size_t alignment)
        {
            m_TotalSize = size;
            m_Alignment = alignment;
            m_Chunk     = (uint8_t*) rz_malloc(size, alignment);
        }

        void RZLinearAllocator::shutdown()
        {
            clear();
            rz_free(m_Chunk);
            m_Chunk         = NULL;
            m_AllocatedSize = 0;
            m_TotalSize     = 0;
        }

        void* RZLinearAllocator::allocate(size_t size)
        {
            if (m_AllocatedSize + size > m_TotalSize)
                return nullptr;

            m_AllocatedSize += size;
            void* address = m_Chunk + m_AllocatedSize;
            return address;
        }

        size_t RZLinearAllocator::getRemainingSize() const
        {
            return (m_AllocatedSize >= m_TotalSize) ? 0 : (m_TotalSize - m_AllocatedSize);
        }

    }    // namespace Memory
}    // namespace Razix
