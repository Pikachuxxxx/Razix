// clang-format off
#include "rzxpch.h"
// clang-format on

#include "Razix/Core/Memory/Allocators/RZStackAllocator.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"

namespace Razix {
    namespace Memory {

        void RZStackAllocator::init(size_t size, size_t alignment)
        {
            m_TotalSize  = size;
            m_Alignment  = alignment;
            m_StackChunk = (uint8_t*) rz_malloc(size, alignment);
        }

        void RZStackAllocator::shutdown()
        {
            rz_free(m_StackChunk);
        }

        void* RZStackAllocator::allocate(size_t size)
        {
            if (m_AllocatedSize + size > m_TotalSize)
                return nullptr;

            void* address = m_StackChunk + m_AllocatedSize;
            m_AllocatedSize += size;
            return address;
        }

        void RZStackAllocator::deallocate(void* ptr)
        {
            // In a stack allocator, deallocation is typically done by clearing the entire stack or by popping the last allocation.
            // A single deallocate call is often a no-op.
        }

    }    // namespace Memory
}    // namespace Razix