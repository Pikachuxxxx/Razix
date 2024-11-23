#include "Allocators/RZStackAllocator.h"

#include "RZAllocationMetrics.h"
#include "RZMemoryFunctions.h"

#include <iostream>

namespace Razix {
    namespace Memory {

        void RZStackAllocator::init(size_t size)
        {
            m_StackChunk    = (uint8_t*) RZMalloc(size);    // Allocate a huge chunk of 16-byte aligned memory
            m_AllocatedSize = 0;
            m_TotalSize     = size;
        }

        void RZStackAllocator::shutdown()
        {
            RZFree(m_StackChunk);
        }

        void* RZStackAllocator::allocate(size_t size, size_t alignment)
        {
            const size_t new_start          = RZMemAlign(m_AllocatedSize, alignment);
            const size_t new_allocated_size = new_start + size;
            if (new_allocated_size > m_TotalSize) {
                std::cout << "[Stack Allocator] Stack Overflow!" << std::endl;
                return nullptr;
            }

            m_AllocatedSize = new_allocated_size;
            return m_StackChunk + new_start;
        }

        void RZStackAllocator::deallocate(void* ptr)
        {
            // TODO: Add asserts
            const size_t size_at_pointer = (uint8_t*) ptr - m_StackChunk;
            m_AllocatedSize = size_at_pointer;
        }
    }    // namespace Memory
}    // namespace Razix