#pragma once

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/RZHandle.h"
#include "Razix/Core/UUID/RZUUID.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

#include "RZSTL/smart_pointers.h"

#define GET_INSTANCE_SIZE static u32 GetInstanceSize()


#ifdef RAZIX_RENDER_API_VULKAN
    #define GET_INSTANCE_SIZE_IMPL_VULKAN(TYPE) \
        case Razix::Gfx::RenderAPI::VULKAN:     \
            return sizeof(VK##TYPE);            \
            break;
#else
    #define GET_INSTANCE_SIZE_IMPL_VULKAN(TYPE) \
        case Razix::Gfx::RenderAPI::VULKAN:     \
            return 0;                           \
            break;
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #define GET_INSTANCE_SIZE_IMPL_DIRECTX12(TYPE) \
        case Razix::Gfx::RenderAPI::D3D12:         \
            return sizeof(DX12##TYPE);             \
            break;
#else
    #define GET_INSTANCE_SIZE_IMPL_DIRECTX12(TYPE) \
        case Razix::Gfx::RenderAPI::D3D12:         \
            return 0;                              \
            break;
#endif

#define GET_INSTANCE_SIZE_IMPL(TYPE)                      \
    u32 RZ##TYPE::GetInstanceSize()                       \
    {                                                     \
        switch (Gfx::RZGraphicsContext::GetRenderAPI()) { \
            GET_INSTANCE_SIZE_IMPL_VULKAN(TYPE)           \
            GET_INSTANCE_SIZE_IMPL_DIRECTX12(TYPE)        \
            default: return sizeof(RZ##TYPE); break;      \
        }                                                 \
    }

namespace Razix {
    namespace Gfx {

        template<typename T>
        class IRZResource
        {
        public:
            IRZResource()
            {
                m_AtomicRefCounter.Ref();
            }

            // Note: Shouldn't this be Virtual?
            virtual ~IRZResource()
            {
                auto refValue = m_AtomicRefCounter.Unref();
                if (refValue <= 0) {
                    RAZIX_CORE_ERROR("Resource ${0}$ has no References! | UUID : {1} | pool idx : {2} !!! NEEDS TO BE RELEASED FROM POOL !!!", m_ResourceName, m_UUID.prettyString(), m_Handle.getIndex());
                    // TODO: Since this has no references Inform the pool it belongs to deallocate it's memory
                    // TODO: Release this from memory!???
                    // TODO: Should this be in Handle instead (think makes more sense, since we only deal with raw pointers ref counting doesn't actually work?)? and what happens if we pass around references of handles does ref counting still work?
                    //RZResourceManager::Get().getPool<T>().release(getHandle());
                }
            }

            virtual void DestroyResource() = 0;

#if 0
            // This is the Magic right here, these will make sure new will allocate the memory from their respective ResourcePools, no need to use placement new and make things looks messy
            // Note: Operators are STATIC FUNCTIONS!
            //void* operator new(size_t size);
            void* operator new(size_t size, void* where);
            //void* operator new[](size_t size);
            void* operator new[](size_t size, void* where);

            //void operator delete(void* pointer);
            void operator delete(void* pointer, void* where);
            //void operator delete[](void* pointer);
            void operator delete[](void* pointer, void* where);
#endif

            inline const std::string& getName() { return m_ResourceName; }
            inline void               setName(const std::string& name) { m_ResourceName = name; }
            inline u32  getPoolIndex() { return m_Handle.getIndex(); }
            inline void setPoolIndex(u32 index) { m_Handle.setIndex(index); }
            inline u32  getGenerationIndex() { return m_Handle.getGeneration(); }
            inline void setGenerationIndex(u32 index) { m_Handle.setGeneration(index); }
            inline RZHandle<T>& getHandle() { return m_Handle; }
            inline void         setHandle(const RZHandle<T>& handle) { m_Handle = handle; }
            inline RZUUID& getUUID() { return m_UUID; }

        protected:
            rzstl::ReferenceCounter m_AtomicRefCounter = {};
            RZUUID                  m_UUID             = {};
            RZHandle<T>             m_Handle           = {};
            std::string             m_ResourceName     = "$UNNAMED_RESOURCE$";

        };

#if 0
template<typename T>
        void* IRZResource<T>::operator new(size_t size)
        {
            RAZIX_ASSERT(false, "[Resource] new cannot be used to allocate memory! Please use T::Create(where, ...) or RZResourceManager::CreateXXX(TDesc&)");

            // Return the pre allocated memory from it's respective Resource Memory Pool
            RZHandle<T> handle;
            void*       ptr = RZResourceManager::Get().getPool<T>().obtain(handle);
            ///////////////////////////////////////////
            // THIS INFO IS BEING LOST!
            IRZResource* resource = (IRZResource*) ptr;
            resource->setHandle(handle);
            ///////////////////////////////////////////
            return ptr;
        }

        template<typename T>
        void* IRZResource<T>::operator new(size_t size, void* where)
        {
            // Placement new DOES nothing at this point, throw and error if we allocate memory from pool this way
            //RAZIX_ASSERT(false, "[Resource] placement new cannot be used to allocate memory! Please use T::Create(where, ...) or RZResourceManager::CreateXXX(TDesc&)");
            (void) size;
            return (where);
        }

 template<typename T>
        void* IRZResource<T>::operator new[](size_t size)
        {
            RAZIX_ASSERT(false, "[Resource] new cannot be used to allocate memory! Please use T::Create(where, ...) or RZResourceManager::CreateXXX(TDesc&)");

            // Return the pre allocated memory from it's respective Resource Memory Pool
            RZHandle<T>  handle;
            void*        ptr      = RZResourceManager::Get().getPool<T>().obtain(handle);
            IRZResource* resource = (IRZResource*) ptr;
            resource->setHandle(handle);
            return ptr;
        }

        template<typename T>
        void* IRZResource<T>::operator new[](size_t size, void* where)
        {
            // Placement new DOES nothing at this point, throw and error if we allocate memory from pool this way
            //RAZIX_ASSERT(false, "[Resource] Placement new cannot be used to allocate memory! Please use T::Create or RZResourceManager::CreateXXX(TDesc&)");
            (void) size;
            return (where);
        }

 template<typename T>
        void IRZResource<T>::operator delete(void* pointer)
        {
            // Memory is deallocated manually using pool allocator indices and we will never use free
            // in fact crash the app if someone tried to use and issue a warning saying it's the wrong way to do it
            // Now in essence it's an IRZResource and we get it's handle from the pointer (m_Handle wont' work cause this is a static method)
            // TODO: Destroy Generation Index
            auto res = (IRZResource*) (pointer);
            res->setGenerationIndex(0);
            RZResourceManager::Get().getPool<T>().release(res->getHandle());
        }

        template<typename T>
        void IRZResource<T>::operator delete(void* pointer, void* where)
        {
            RAZIX_UNREF_VAR((void*)(pointer));
            RAZIX_UNREF_VAR((void*)(where));
        }

template<typename T>
        void IRZResource<T>::operator delete[](void* pointer)
        {
            // Memory is deallocated manually using pool allocator indices and we will never use free
            // in fact crash the app if someone tried to use and issue a warning saying it's the wrong way to do it
            // Now in essence it's an IRZResource and we get it's handle from the pointer (m_Handle wont' work cause this is a static method)
            // TODO: Destroy Generation Index
            auto res = (IRZResource*) (pointer);
            res->setGenerationIndex(0);
            RZResourceManager::Get().getPool<T>().release(res->getHandle());
        }


        template<typename T>
        void IRZResource<T>::operator delete[](void* pointer, void* where)
        {
            RAZIX_UNREF_VAR((void*)(pointer));
            RAZIX_UNREF_VAR((void*)(where));
        }
#endif
    }    // namespace Gfx
}    // namespace Razix
