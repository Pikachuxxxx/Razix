// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSmartPointers.h"

namespace Razix {
    namespace rzstl {

        RefCounterManager::RefCounterManager()
        {
            m_RefCounter.InitRefCount();
            m_RefCounterInit.InitRefCount(0);
            m_WeakRefCounter.InitRefCount(0);
        }

        RefCounterManager::~RefCounterManager()
        {
        }

        bool RefCounterManager::InitRef()
        {
            if (Reference()) {
                if (m_RefCounterInit.GetRefCount() > 0) {
                    m_RefCounterInit.Unref();
                    Unreference();
                }
                return true;
            } else
                return false;
        }

        bool RefCounterManager::Reference()
        {
            return m_RefCounter.Ref();
        }

        bool RefCounterManager::Unreference()
        {
            return m_RefCounter.Unref();
        }

        bool RefCounterManager::WeakReference()
        {
            return m_WeakRefCounter.Ref();
        }

        bool RefCounterManager::WeakUnreference()
        {
            bool die = m_WeakRefCounter.Unref() && m_RefCounter.count == 0;
            return die;
        }

        int RefCounterManager::GetReferenceCount() const
        {
            return m_RefCounter.GetRefCount();
        }

        int RefCounterManager::GetWeakReferenceCount() const
        {
            return m_WeakRefCounter.GetRefCount();
        }
    }    // namespace rzstl
}    // namespace Razix