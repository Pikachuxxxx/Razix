// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphSemaphore.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            void RZFrameGraphSemaphore::create(const Desc& desc, void* allocator)
            {
                m_SemaphoreHandle = Graphics::RZSemaphore::Create(desc.name);
            }

            void RZFrameGraphSemaphore::destroy(const Desc& desc, void* allocator)
            {
                //m_SemaphoreHandle->Destroy();
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
