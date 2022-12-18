// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourceEntry.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            //-----------------------------------------------------------------------------------
            // Resource Entry Class
            //-----------------------------------------------------------------------------------

            void RZResourceEntry::create(void *allocator)
            {
                RAZIX_CORE_ASSERT(isTransient(), "Cannot create non-transient resource");
                m_Concept->create(allocator);
            }

            void RZResourceEntry::destroy(void *allocator)
            {
                RAZIX_CORE_ASSERT(isTransient(), "Cannot create non-transient resource");
                m_Concept->destroy(allocator);
            }

            std::string RZResourceEntry::toString() const
            {
                return m_Concept->toString();
            }

            //-----------------------------------------------------------------------------------
            // Resource Entry Class
            //-----------------------------------------------------------------------------------
            template<typename T>
            inline RZResourceEntry::RZResourceEntry(uint32_t id, typename T::CreateDesc &&desc, T &&obj, uint32_t version, bool imported /*= false*/)
                : m_ID{id}, m_Concept{std::make_unique<RZTypeResource<T>>(std::forward<typename T::CreateDesc>(desc), std::forward<T>(obj))}, m_Version(version), m_Imported(imported)
            {
            }

            template<typename T>
            inline T &Razix::Graphics::FrameGraph::RZResourceEntry::get()
            {
                return getTypeResource<T>()->resource;
            }

            template<typename T>
            typename const T::CreateDesc &RZResourceEntry::getDescriptor() const
            {
                return getTypeResource<T>()->descriptor;
            }

            template<typename T>
            auto *RZResourceEntry::getTypeResource() const
            {
                auto *typeResource = dynamic_cast<RZTypeResource<T> *>(m_Concept.get());
                return typeResource;
            }

            //-----------------------------------------------------------------------------------
            // TypeResource Class
            //-----------------------------------------------------------------------------------

            template<typename T>
            std::string RZResourceEntry::RZTypeResource<T>::toString() const
            {
                return T::toString(descriptor);
            }

            template<typename T>
            void RZResourceEntry::RZTypeResource<T>::destroy(void *allocator)
            {
                resource.destroy(descriptor, allocator);
            }

            template<typename T>
            void RZResourceEntry::RZTypeResource<T>::create(void *allocator)
            {
                resource.create(descriptor, allocator);
            }

            template<typename T>
            RZResourceEntry::RZTypeResource<T>::RZTypeResource(typename T::CreateDesc &&desc, T &&obj)
                : descriptor(std::move(desc)), resource(std::move(obj))
            {
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix