// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Core/Memory/Allocators/RZBumpAllocator.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

namespace Razix {
    namespace Memory {

        void RZBumpAllocator::init(size_t size, size_t alignment)
        {
            m_TotalSize = size;
            m_Alignment = alignment;
            m_Chunk     = (uint8_t*) rz_malloc(size, alignment);
        }

        void RZBumpAllocator::shutdown()
        {
            clear();
            rz_free(m_Chunk);
            m_Chunk         = NULL;
            m_AllocatedSize = 0;
            m_TotalSize     = 0;
        }

        void RZBumpAllocator::beginFrame()
        {
            m_AllocatedSize = 0;
            memset(m_Chunk, 0x00, m_TotalSize);
        }

        void RZBumpAllocator::endFrame()
        {
            m_AllocatedSize = 0;
            memset(m_Chunk, 0x00, m_TotalSize);
        }

        void* RZBumpAllocator::allocate(size_t size)
        {
            if (m_AllocatedSize + size > m_TotalSize)
                return nullptr;

            void* address = m_Chunk + m_AllocatedSize;
            m_AllocatedSize += size;
            memset(address, 0x00, size);
            return address;
        }

        void* RZBumpAllocator::allocate(size_t size, size_t alignment)
        {
            size_t currentAddress = reinterpret_cast<size_t>(m_Chunk) + m_AllocatedSize;
            size_t padding        = (alignment - (currentAddress % alignment)) % alignment;

            if (m_AllocatedSize + padding + size > m_TotalSize)
                return nullptr;

            m_AllocatedSize += padding;
            void* address = m_Chunk + m_AllocatedSize;
            m_AllocatedSize += size;
            memset(address, 0x00, size);
            return address;
        }

        size_t RZBumpAllocator::getRemainingSize() const
        {
            return (m_AllocatedSize >= m_TotalSize) ? 0 : (m_TotalSize - m_AllocatedSize);
        }

    }    // namespace Memory
}    // namespace Razix
