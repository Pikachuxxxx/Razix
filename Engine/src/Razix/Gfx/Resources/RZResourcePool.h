#pragma once

#include "Razix/Core/RZHandle.h"

#include "Internal/RazixMemory/include/Allocators/RZHeapAllocator.h"

#include <tracy/Tracy.hpp>

// [Source] : https://github.com/PacktPublishing/Mastering-Graphics-Programming-with-Vulkan/blob/2ad4e94a0e003d37dd3dbef46cc033a483f133d6/source/raptor/foundation/data_structures.hpp

namespace Razix {
    namespace Gfx {

        class RZCPUMemoryManager;
        class RZGPUMemoryManager;

        template<typename U>
        class IRZResource;

        class RAZIX_API RZResourcePool
        {
        public:
            RZResourcePool()  = default;
            ~RZResourcePool() = default;

            void init(u32 poolSize, u32 resourceSize, u32 alignment = 16);
            void destroy();
            void freePool();

        protected:
            u32 allocateResource();

            const void* accessResource(u32 index) const;
            void        releaseResource(u32 index);

        protected:
            u8*  m_MemoryChunk     = nullptr;
            u32* m_FreeIndices     = nullptr;
            u32  m_PoolSize        = 0;
            u32  m_ResourceSize    = 0;
            u32  m_FreeIndicesHead = 0;
            u32  m_UsedIndices     = 0;
        };

        template<typename T>
        struct RZResourcePoolTyped final : public RZResourcePool
        {
            void printResources();

            T*   obtain(RZHandle<T>& handle);
            void release(RZHandle<T>& handle);

            // TODO: Make this return a rzstl::SharedRef
            // TODO: Figure out how to offer both version and make it safe
            T* get(RZHandle<T> handle) const;

            std::vector<RZHandle<T>> getHandles();

        private:
            T* getInternal(RZHandle<T>& handle);

            void initResource(void* resource, u32 index, u32 genIdx);
        };

        template<typename T>
        void RZResourcePoolTyped<T>::printResources()
        {
            if (m_FreeIndicesHead != 0) {
                for (u32 i = 0; i < m_FreeIndicesHead; i++) {
                    RAZIX_CORE_TRACE("\tResource id={0}, name={1}\n", m_FreeIndices[i], ((IRZResource<T>*) accessResource(i))->getName().c_str());
                }
            }
        }

        template<typename T>
        std::vector<RZHandle<T>> RZResourcePoolTyped<T>::getHandles()
        {
            std::vector<RZHandle<T>> handles;

            if (m_FreeIndicesHead != 0) {
                for (u32 i = 0; i < m_FreeIndicesHead; ++i) {
                    handles.push_back(((IRZResource<T>*) accessResource(i))->getHandle());
                }
            }
            return handles;
        }

        //template<typename T>
        //inline void RZResourcePoolTyped<T>::init(u32 pool_size, u32 resource_size, u32 alignment)
        //{
        //    RZResourcePool::init(pool_size, resource_size, alignment);
        //}
        //
        //template<typename T>
        //inline void RZResourcePoolTyped<T>::shutdown()
        //{
        //    RZResourcePool::shutdown();
        //}

        template<typename T>
        inline T* RZResourcePoolTyped<T>::obtain(RZHandle<T>& handle)
        {
            u32 index = RZResourcePool::allocateResource();
            handle.setIndex(index);
            handle.setGeneration(++index);
            if (index != u32_max) {
                T* resource = getInternal(handle);
                TracyAlloc(resource, m_ResourceSize);
                return resource;
            }

            return nullptr;
        }

        template<typename T>
        inline void RZResourcePoolTyped<T>::release(RZHandle<T>& handle)
        {
            T* resource = getInternal(handle);
            resource->DestroyResource();
            handle.setGeneration(0);
            TracyFree(resource);
            RZResourcePool::releaseResource(handle.getIndex());
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::getInternal(RZHandle<T>& handle)
        {
            if (handle.isValid())
                return (T*) RZResourcePool::accessResource(handle.getIndex());
            else
                return nullptr;
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::get(RZHandle<T> handle) const
        {
            if (handle.isValid())
                return (/*const */ T*) RZResourcePool::accessResource(handle.getIndex());
            else
                return nullptr;
        }
    }    // namespace Gfx
}    // namespace Razix
