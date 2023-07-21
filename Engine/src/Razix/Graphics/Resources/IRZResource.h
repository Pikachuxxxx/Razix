#pragma once

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/RZHandle.h"
#include "Razix/Core/RZSmartPointers.h"
#include "Razix/Core/RZUUID.h"
#include "Razix/Graphics/Resources/RZResourceManager.h"

namespace Razix {
    namespace Graphics {

        template<typename T>
        class IRZResource
        {
        public:
            IRZResource()
            {
                m_AtomicRefCounter.Ref();
            }

            // Note: Shouldn't this be Virtual?
            ~IRZResource()
            {
                auto refValue = m_AtomicRefCounter.Unref();
                if (refValue <= 0) {
                    // TODO: Since this has no references Inform the pool it belongs to deallocate it's memory
                    RAZIX_CORE_TRACE("Resource ${0}$ has no References! | UUID : {1} | pool idx : {2} ", m_ResourceName, m_UUID.prettyString(), m_Handle.getIndex());
                }
            }

            // This is the Magic right here, these will make sure new will allocate the memory from their respective ResourcePools, no need to use placement new and make things looks messy
            // Operators are STATIC FUNCTIONS!
            void* operator new(size_t size);
            void* operator new[](size_t size);
            void* operator new[](size_t size, void* where);

            void operator delete(void* pointer);
            void operator delete[](void* pointer);

            inline const std::string& getName() { return m_ResourceName; }
            inline void               setName(const std::string& name) { m_ResourceName = name; }

            inline u32  getPoolIndex() { return m_Handle.getIndex(); }
            inline void setPoolIndex(u32 index) { m_Handle.setIndex(index); }

            inline RZHandle<T>& getHandle() { return m_Handle; }
            inline RZUUID&      getUUID() { return m_UUID; }

        protected:
            rzstl::ReferenceCounter m_AtomicRefCounter;
            RZUUID                  m_UUID;
            RZHandle<T>             m_Handle;
            std::string             m_ResourceName;

        private:
            //template<typename U>
            //friend RZResourcePoolTyped<U>;
        };

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
            // Memory is deallocated manually using pool allocator indices and we will never use free
            // in fact crash the app if someone tried to use and issue a warning saying it's the wrong way to do it
            // Now in essence it's an IRZResource and we get it's handle from the pointer (m_Handle wont' work cause this is a static method)
            auto res = (IRZResource*) (pointer);
            RZResourceManager::Get().getPool<T>().release(res->getPoolIndex());
        }

        template<typename T>
        void IRZResource<T>::operator delete[](void* pointer)
        {
            // Memory is deallocated manually using pool allocator indices and we will never use free
            // in fact crash the app if someone tried to use and issue a warning saying it's the wrong way to do it
            // Now in essence it's an IRZResource and we get it's handle from the pointer (m_Handle wont' work cause this is a static method)
            auto res = (IRZResource*) (pointer);
            RZResourceManager::Get().getPool<T>().release(res->getPoolIndex());
        }
    }    // namespace Graphics
}    // namespace Razix