#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"

// [Source] : https://github.com/PacktPublishing/Mastering-Graphics-Programming-with-Vulkan/blob/2ad4e94a0e003d37dd3dbef46cc033a483f133d6/source/raptor/foundation/data_structures.hpp
// TODO: since RZResourcePool is only used by the typed version of this, remove it and make the template allocator simpler and use a RZLinearAllocator, this allocator ir provided the by central CPU and GPU allocators for the engine

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

            T*   obtain(rz_handle& handle);
            void release(rz_handle& handle);

            // TODO: Make this return a rzstl::SharedRef
            // TODO: Figure out how to offer both version and make it safe
            T* get(rz_handle handle) const;

            std::vector<rz_handle> getHandles();

        private:
            T* getInternal(rz_handle& handle);

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
        std::vector<rz_handle> RZResourcePoolTyped<T>::getHandles()
        {
            std::vector<rz_handle> handles;

            if (m_FreeIndicesHead != 0) {
                for (u32 i = 0; i < m_FreeIndicesHead; ++i) {
                    handles.push_back(((IRZResource<T>*) accessResource(i))->getHandle());
                }
            }
            return handles;
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::obtain(rz_handle& handle)
        {
            u32 index = RZResourcePool::allocateResource();
            rz_handle_set_index(&handle, index);
            rz_handle_set_generation(&handle, ++index);

            if (index != UINT32_MAX) {
                T* resource = getInternal(handle);
                return resource;
            }

            return nullptr;
        }

        template<typename T>
        inline void RZResourcePoolTyped<T>::release(rz_handle& handle)
        {
            T* resource = getInternal(handle);
            resource->DestroyResource();
            rz_handle_set_generation(&handle, 0);
            RZResourcePool::releaseResource(rz_handle_get_index(*handle));
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::getInternal(rz_handle& handle)
        {
            if (handle.isValid())
                return (T*) RZResourcePool::accessResource(rz_handle_get_index(*handle));
            else
                return nullptr;
        }

        template<typename T>
        inline T* RZResourcePoolTyped<T>::get(rz_handle handle) const
        {
            if (handle.isValid())
                return (/*const */ T*) RZResourcePool::accessResource(rz_handle_get_index(*handle));
            else
                return nullptr;
        }
    }    // namespace Gfx
}    // namespace Razix
