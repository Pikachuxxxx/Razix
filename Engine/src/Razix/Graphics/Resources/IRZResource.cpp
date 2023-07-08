// clang-format off
#include "rzxpch.h"
// clang-format on
#include "IRZResource.h"

namespace Razix {
    namespace Graphics {

#if 0
template<typename T>
        void* IRZResource<T>::operator new(size_t size)
        {
            // Return the pre allocated memory from it's respective Resource Memory Pool
            return RZResourceManager::Get().getPool<T>().obtain();
        }

        template<typename T>
        void* IRZResource<T>::operator new[](size_t size)
        {
            return RZResourceManager::Get().getPool<T>().obtain();
        }

        template<typename T>
        void* IRZResource<T>::operator new[](size_t size, void* where)
        {
            (void) size;
            return (where);
        }

        template<typename T>
        void IRZResource<T>::operator delete(void* pointer)
        {
            RZResourceManager::Get().getPool<T>().release(m_Handle.getIndex());
        }

        template<typename T>
        void IRZResource<T>::operator delete[](void* pointer)
        {
            RZResourceManager::Get().getPool<T>().release(m_Handle.getIndex());
        }
#endif
    }    // namespace Graphics
}    // namespace Razix
