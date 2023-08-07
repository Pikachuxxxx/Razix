// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourcePool.h"

#include <internal/RazixMemory/include/RZMemoryFunctions.h>

#include "IRZResource.h"

namespace Razix {
    namespace Graphics {

        static const u32 k_invalid_index = 0xffffffff;

        void RZResourcePool::init(u32 poolSize, u32 resourceSize)
        {
            m_PoolSize     = poolSize;
            m_ResourceSize = resourceSize;

            // Group allocate ( resource size + u32 )
            m_MemoryChunk = (u8*) Memory::RZMalloc(poolSize * (resourceSize + sizeof(u32)), 16);

            // Allocate and add free indices
            m_FreeIndices     = (u32*) (m_MemoryChunk + poolSize * resourceSize);
            m_FreeIndicesHead = 0;

            for (u32 i = 0; i < poolSize; ++i)
                m_FreeIndices[i] = i;

            m_UsedIndices = 0;
        }

        void RZResourcePool::destroy()
        {
            if (m_FreeIndicesHead != 0) {
                RAZIX_CORE_ERROR("Resource pool has un-freed resources.\n");

                for (u32 i = 0; i < m_FreeIndicesHead; ++i) {
                    RAZIX_CORE_ERROR("\tResource id={0}\n", m_FreeIndices[i]);
                }
            }

            RAZIX_CORE_ASSERT(m_UsedIndices == 0, "[Pool Allocator] Pool still has used indices");

            Memory::RZFree(m_MemoryChunk);
        }

        u32 RZResourcePool::allocateResource()
        {
            // TODO: add bits for checking if resource is alive and use bit masks.
            if (m_FreeIndicesHead < m_PoolSize) {
                const u32 free_index = m_FreeIndices[m_FreeIndicesHead++];
                ++m_UsedIndices;
                return free_index;
            }
            // Error: no more resources left!
            RAZIX_CORE_ASSERT(false, "[Pool Allocator] No More resoruces left!");
            return k_invalid_index;
        }

        void RZResourcePool::releaseResource(u32 index)
        {
            // TODO: add bits for checking if resource is alive and use bit masks.
            m_FreeIndices[--m_FreeIndicesHead] = index;
            --m_UsedIndices;
        }

        const void* RZResourcePool::accessResource(u32 index) const
        {
            if (index != k_invalid_index) {
                auto resource = &m_MemoryChunk[index * m_ResourceSize];
                return resource;
            }
            return nullptr;
        }

        void RZResourcePool::freePool()
        {
            m_FreeIndicesHead = 0;
            m_UsedIndices     = 0;

            for (uint32_t i = 0; i < m_PoolSize; ++i)
                m_FreeIndices[i] = i;
        }
    }    // namespace Graphics
}    // namespace Razix