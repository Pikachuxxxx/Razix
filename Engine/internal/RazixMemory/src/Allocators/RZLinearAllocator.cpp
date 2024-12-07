#include "Allocators/RZLinearAllocator.h"

#include "RZAllocationMetrics.h"
#include "RZMemoryFunctions.h"

#include <iostream>

namespace Razix {
    namespace Memory {

        void RZLinearAllocator::init(size_t size)
        {
            m_Chunk         = (uint8_t*) RZMalloc(size);    // Allocate a huge chunk of 16-byte aligned memory
            m_AllocatedSize = 0;
            m_TotalSize     = size;
        }

        void RZLinearAllocator::shutdown()
        {
            clear();
            RZFree(m_Chunk);
        }

        void* RZLinearAllocator::allocate(size_t size, size_t alignment)
        {
            const size_t new_start          = RZMemAlign(m_AllocatedSize, alignment);
            const size_t new_allocated_size = new_start + size;
            if (new_allocated_size > m_TotalSize) {
                std::cout << "[Linear Allocator] Overflow!" << std::endl;
                return nullptr;
            }

            m_AllocatedSize = new_allocated_size;
            return m_Chunk + new_start;
        }
    }    // namespace Memory
}    // namespace Razix