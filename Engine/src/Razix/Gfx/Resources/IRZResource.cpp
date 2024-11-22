// clang-format off
#include "rzxpch.h"
// clang-format on
#include "IRZResource.h"

namespace Razix {
    namespace Gfx {

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

#endif

    }    // namespace Gfx
}    // namespace Razix
