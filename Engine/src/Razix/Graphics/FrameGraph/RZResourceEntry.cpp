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
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix