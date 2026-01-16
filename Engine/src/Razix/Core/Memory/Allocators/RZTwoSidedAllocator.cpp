// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTwoSidedAllocator.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include <iostream>

namespace Razix {
    namespace Memory {

        void RZTwoSidedAllocator::init(size_t size, size_t alignment)
        {
            m_Chunk     = (uint8_t*) rz_malloc(size, alignment);
            m_Top       = 0;
            m_Bottom    = size;
            m_TotalSize = size;
        }

        void RZTwoSidedAllocator::shutdown()
        {
            rz_free(m_Chunk);
        }

        void* RZTwoSidedAllocator::allocate_top(size_t size)
        {
            const size_t new_allocated_size = m_Top + size;
            if (new_allocated_size > m_Bottom) {
                std::cout << "[Two Sided Allocator] Stack Overflow!" << std::endl;
                return nullptr;
            }

            void* address = m_Chunk + m_Top;
            m_Top         = new_allocated_size;
            return address;
        }

        void* RZTwoSidedAllocator::allocate_bottom(size_t size)
        {
            const size_t new_start = m_Bottom - size;
            if (new_start < m_Top) {
                std::cout << "[Two Sided Allocator] Stack Overflow!" << std::endl;
                return nullptr;
            }

            m_Bottom = new_start;
            return m_Chunk + m_Bottom;
        }

        void RZTwoSidedAllocator::deallocate_top(size_t size)
        {
            if (size > m_Top) {
                m_Top = 0;
            } else {
                m_Top -= size;
            }
        }

        void RZTwoSidedAllocator::deallocate_bottom(size_t size)
        {
            if (m_Bottom + size > m_TotalSize) {
                m_Bottom = m_TotalSize;
            } else {
                m_Bottom += size;
            }
        }

        void RZTwoSidedAllocator::clear_top()
        {
            m_Top = 0;
        }

        void RZTwoSidedAllocator::clear_bottom()
        {
            m_Bottom = m_TotalSize;
        }

    }    // namespace Memory
}    // namespace Razix