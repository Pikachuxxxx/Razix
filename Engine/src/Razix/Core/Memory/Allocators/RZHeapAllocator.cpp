// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZHeapAllocator.h"

#include "Razix/Core/Memory/RZAllocationMetrics.h"
#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include <iostream>

#include "Razix/Core/Memory/vendor/tlsf/tlsf.h"

namespace Razix {
    namespace Memory {

        static void deallocation_pool_walker(void* ptr, size_t size, int used, void* user_data)
        {
#ifdef RAZIX_DEBUG
            if (used)
                printf("[Heap Allocator] Found active allocation at : %p, size: %zu \n", ptr, size);
#endif
        }

        void RZHeapAllocator::init(size_t chunkSize, size_t alignment)
        {
            m_TotalChunkSize = chunkSize;
            m_Alignment      = alignment;

            // Allocate a big chunk of heap for TLSF to manage
            // Note: by default rz_malloc_aligned gives 16-byte aligned memory, if you need raw malloc and you can manage yourself use the OG functions
            m_ChunkAddress = rz_malloc(chunkSize, alignment);

            // Create a TLSF memory pool from which the heap allocations is done
            m_TLSFHandle = tlsf_create_with_pool(m_ChunkAddress, chunkSize);

#ifdef RAZIX_DEBUG
            if (float(chunkSize / 1024) < 1.0f)
                std::cout << "[Heap Allocator] Creating TLSL pool... | size : " << float(chunkSize / 1024) << "Kb" << std::endl;
            else
                std::cout << "[Heap Allocator] Creating TLSL pool... | size : " << float(chunkSize / (1024 * 1024)) << "Mb" << std::endl;
#endif
        }

        void RZHeapAllocator::shutdown()
        {
            // Get the allocations of the chunk and by walking through the pool and make sure there's isn't any un-freed memory hanging around
            RZMemAllocatorStats allocatorStats{0, m_TotalChunkSize, 0};

            pool_t pool = tlsf_get_pool(m_TLSFHandle);
            tlsf_walk_pool(pool, deallocation_pool_walker, (void*) &allocatorStats);

            if (allocatorStats.allocatedBytes) {
#ifdef RAZIX_DEBUG
                std::cout << "[Heap Allocator] Allocation still active | total size : " << m_TotalChunkSize << " allocated bytes : " << m_AllocatedSize << std::endl;
#endif
                throw std::runtime_error("[Heap Allocator] Allocation still active");
            }

            tlsf_destroy(m_TLSFHandle);

            rz_free(m_ChunkAddress);
        }

        void* RZHeapAllocator::allocate(size_t size)
        {
            m_CurrentAllocation = tlsf_memalign(m_TLSFHandle, m_Alignment, size);
            size_t actual_size  = tlsf_block_size(m_CurrentAllocation);
            m_AllocatedSize += actual_size;
            return m_CurrentAllocation;
        }

        void RZHeapAllocator::deallocate(void* ptr)
        {
            size_t actual_size = tlsf_block_size(ptr);
            m_AllocatedSize -= actual_size;

            // Point to previous allocation
            *(size_t*) m_CurrentAllocation = m_AllocatedSize;

            tlsf_free(m_TLSFHandle, ptr);
        }
    }    // namespace Memory
}    // namespace Razix
