// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTwoSidedAllocator.h"

#include "Razix/Core/Memory/RZAllocationMetrics.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include <iostream>

namespace Razix {
    namespace Memory {

        void RZTwoSidedAllocator::init(size_t chunkSize)
        {
            m_Chunk     = (uint8_t*) rz_malloc_aligned(chunkSize);
            m_Top       = chunkSize;
            m_Bottom    = 0;
            m_TotalSize = chunkSize;
        }

        void RZTwoSidedAllocator::shutdown()
        {
            rz_free(m_Chunk);
        }

        void* RZTwoSidedAllocator::allocate_top(size_t size, size_t alignment)
        {
            const size_t new_start = rz_mem_align(m_Top - size, alignment);
            if (new_start <= m_Bottom) {
                std::cout << "[Two Sided Allocator] Stack Overflow!" << std::endl;
                return nullptr;
            }

            m_Top = new_start;
            return m_Chunk + new_start;
        }

        void* RZTwoSidedAllocator::allocate_bottom(size_t size, size_t alignment)
        {
            const size_t new_start          = rz_mem_align(m_Bottom, alignment);
            const size_t new_allocated_size = new_start + size;
            if (new_allocated_size > m_Top) {
                std::cout << "[Two Sided Allocator] Stack Overflow!" << std::endl;
                return nullptr;
            }

            m_Bottom = new_allocated_size;
            return m_Chunk + new_start;
        }

        void RZTwoSidedAllocator::deallocate_top(size_t size)
        {
            if (size > m_TotalSize - m_Top) {
                m_Top = m_TotalSize;
            } else {
                m_Top += size;
            }
        }

        void RZTwoSidedAllocator::deallocate_bottom(size_t size)
        {
            if (size > m_Bottom) {
                m_Bottom = 0;
            } else {
                m_Bottom -= size;
            }
        }

        void RZTwoSidedAllocator::clear_top()
        {
            m_Top = m_TotalSize;
        }

        void RZTwoSidedAllocator::clear_bottom()
        {
            m_Bottom = 0;
        }
    }    // namespace Memory
}    // namespace Razix
