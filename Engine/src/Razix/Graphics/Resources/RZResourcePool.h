#pragma once

#include "Razix/Core/RZHandle.h"

#include "Internal/RazixMemory/include/Allocators/RZHeapAllocator.h"

namespace Razix {
    namespace Graphics {

        class RZCPUMemoryManager;
        class RZGPUMemoryManager;

        template<typename U>
        class IRZResource;

        class RZResourcePool
        {
        public:
            RZResourcePool()  = default;
            ~RZResourcePool() = default;

            void init(u32 poolSize, u32 resourceSize);
            void destroy();

            u32 allocateResource();

            const void* accessResource(u32 index);
            void        releaseResource(u32 index);

            void freePool();

        private:
            u8*  m_MemoryChunk     = nullptr;
            u32* m_FreeIndices     = nullptr;
            u32  m_PoolSize        = 0;
            u32  m_ResourceSize    = 0;
            u32  m_FreeIndicesHead = 0;
            u32  m_UsedIndices     = 0;
        };

        template<typename T>
        struct RZResourcePoolTyped : public RZResourcePool
        {
            void init(u32 pool_size, u32 resource_size);
            void shutdown();

            T*   obtain(u32& index);
            void release(u32 index);

            T*       get(u32 index);
            const T* get(u32 index) const;

        private:
            void initResource(void* resource, u32 index, u32 genIdx);
        };

        template<typename T>
        inline void RZResourcePoolTyped<T>::init(u32 pool_size, u32 resource_size)
        {
            RZResourcePool::init(pool_size, resource_size);
        }

        template<typename T>
        inline void RZResourcePoolTyped<T>::shutdown()
        {
            RZResourcePool::shutdown();
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::obtain(u32& index)
        {
            index = RZResourcePool::allocateResource();
            if (index != u32_max) {
                T* resource = get(index);
                //initResource(resource, resource_index, resource_index);
                return resource;
            }

            return nullptr;
        }

        template<typename T>
        inline void RZResourcePoolTyped<T>::release(u32 index)
        {
            T* resource = get(index);
            //(IRZResource*) (resource)->getHandle().setGeneration(0);
            RZResourcePool::releaseResource(index);
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::get(u32 index)
        {
            return (T*) RZResourcePool::accessResource(index);
        }

        template<typename T>
        inline const T* RZResourcePoolTyped<T>::get(u32 index) const
        {
            return (const T*) RZResourcePool::accessResource(index);
        }
    }    // namespace Graphics
}    // namespace Razix
