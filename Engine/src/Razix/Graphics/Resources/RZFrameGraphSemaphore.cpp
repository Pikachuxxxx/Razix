// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphSemaphore.h"

#include "Razix/Graphics/Resources/RZTransientResources.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            void RZFrameGraphSemaphore::create(const Desc& desc, void* allocator)
            {
                m_SemaphoreHandle = static_cast<FrameGraph::RZTransientResources*>(allocator)->acquireSemaphore(desc);
            }

            void RZFrameGraphSemaphore::destroy(const Desc& desc, void* allocator)
            {
                static_cast<FrameGraph::RZTransientResources*>(allocator)->releaseSemaphore(desc, m_SemaphoreHandle);
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
