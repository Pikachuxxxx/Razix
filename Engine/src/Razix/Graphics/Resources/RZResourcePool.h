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
            void freePool();

        protected:
            u32 allocateResource();

            const void* accessResource(u32 index) const;
            void        releaseResource(u32 index);

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

            T*   obtain(RZHandle<T>& handle);
            void release(RZHandle<T>& handle);

            // TODO: Make this return a rzstl::SharedRef
            // TODO: Figure out how to offer both version and make it safe
            T* get(RZHandle<T> handle) const;

        private:
            T* getInternal(RZHandle<T>& handle);

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
        inline T* RZResourcePoolTyped<T>::obtain(RZHandle<T>& handle)
        {
            u32 index = RZResourcePool::allocateResource();
            handle.setIndex(index);
            handle.setGeneration(++index);
            if (index != u32_max) {
                T* resource = getInternal(handle);
                return resource;
            }

            return nullptr;
        }

        template<typename T>
        inline void RZResourcePoolTyped<T>::release(RZHandle<T>& handle)
        {
            T* resource = getInternal(handle);
            resource->Destroy();
            handle.setGeneration(0);
            RZResourcePool::releaseResource(handle.getIndex());
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::getInternal(RZHandle<T>& handle)
        {
            return (T*) RZResourcePool::accessResource(handle.getIndex());
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::get(RZHandle<T> handle) const
        {
            return (/*const */T*) RZResourcePool::accessResource(handle.getIndex());
        }
    }    // namespace Graphics
}    // namespace Razix
