// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSmartPointers.h"

namespace Razix {
    namespace rzstl {

        RefCounter::RefCounter()
        {
            m_RefCounter.InitRefCount();
            m_RefCounterInit.InitRefCount(0);
            m_WeakRefCounter.InitRefCount(0);
        }

        RefCounter::~RefCounter()
        {
        }

        bool RefCounter::InitRef()
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

        bool RefCounter::Reference()
        {
            return m_RefCounter.Ref();
        }

        bool RefCounter::Unreference()
        {
            return m_RefCounter.Unref();
        }

        bool RefCounter::WeakReference()
        {
            return m_WeakRefCounter.Ref();
        }

        bool RefCounter::WeakUnreference()
        {
            bool die = m_WeakRefCounter.Unref() && m_RefCounter.count == 0;
            return die;
        }

        int RefCounter::GetReferenceCount() const
        {
            return m_RefCounter.GetRefCount();
        }

        int RefCounter::GetWeakReferenceCount() const
        {
            return m_WeakRefCounter.GetRefCount();
        }
    }    // namespace rzstl
}    // namespace Razix