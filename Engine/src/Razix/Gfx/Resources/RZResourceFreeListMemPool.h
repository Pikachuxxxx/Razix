#pragma once

#include "Razix/Core/RZHandle.h"
#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"

// [Source] : https://github.com/PacktPublishing/Mastering-Graphics-Programming-with-Vulkan/blob/2ad4e94a0e003d37dd3dbef46cc033a483f133d6/source/raptor/foundation/data_structures.hpp
// TODO: since RZResourcePool is only used by the typed version of this, remove it and make the template allocator simpler and use a RZLinearAllocator, this allocator ir provided the by central CPU and GPU allocators for the engine

namespace Razix {
    namespace Gfx {

        class RAZIX_API RZResourceFreeListMemPool
        {
        public:
            RZResourceFreeListMemPool()  = default;
            ~RZResourceFreeListMemPool() = default;

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
        struct RZResourceFreeListMemPoolTyped final : public RZResourceFreeListMemPool
        {
            void printResources();

            T*   obtain(rz_handle& handle);
            void release(rz_handle& handle);

            // TODO: Make this return a rzstl::SharedRef
            // TODO: Figure out how to offer both version and make it safe
            T* get(rz_handle handle) const;

            RZDynamicArray<rz_handle> getHandles();

        private:
            T* getInternal(rz_handle& handle);

            void initResource(void* resource, u32 index, u32 genIdx);
        };

        template<typename T>
        void RZResourceFreeListMemPoolTyped<T>::printResources()
        {
            if (m_FreeIndicesHead != 0) {
                for (u32 i = 0; i < m_FreeIndicesHead; i++) {
                    RAZIX_CORE_TRACE("\tResource id={0}, name={1}\n", m_FreeIndices[i], ((rz_gfx_resource*) accessResource(i))->pName);
                }
            }
        }

        template<typename T>
        RZDynamicArray<rz_handle> RZResourceFreeListMemPoolTyped<T>::getHandles()
        {
            RZDynamicArray<rz_handle> handles;

            if (m_FreeIndicesHead != 0) {
                for (u32 i = 0; i < m_FreeIndicesHead; ++i) {
                    handles.push_back(((rz_gfx_resource*) accessResource(i))->handle);
                }
            }
            return handles;
        }

        template<typename T>
        inline T* RZResourceFreeListMemPoolTyped<T>::obtain(rz_handle& handle)
        {
            u32 index = RZResourceFreeListMemPool::allocateResource();
            rz_handle_set_index(&handle, index);
            rz_handle_set_generation(&handle, ++index);

            if (index != UINT32_MAX) {
                T* resource = getInternal(handle);
                return resource;
            }

            return nullptr;
        }

        template<typename T>
        inline void RZResourceFreeListMemPoolTyped<T>::release(rz_handle& handle)
        {
            rz_handle_set_generation(&handle, 0);
            RZResourceFreeListMemPool::releaseResource(rz_handle_get_index(&handle));
        }

        template<typename T>
        inline T* RZResourceFreeListMemPoolTyped<T>::getInternal(rz_handle& handle)
        {
            if (rz_handle_is_valid(&handle))
                return (T*) RZResourceFreeListMemPool::accessResource(rz_handle_get_index(&handle));
            else
                return nullptr;
        }

        template<typename T>
        inline T* RZResourceFreeListMemPoolTyped<T>::get(rz_handle handle) const
        {
            if (rz_handle_is_valid(&handle))
                return (/*const */ T*) RZResourceFreeListMemPool::accessResource(rz_handle_get_index(&handle));
            else
                return nullptr;
        }
    }    // namespace Gfx
}    // namespace Razix
