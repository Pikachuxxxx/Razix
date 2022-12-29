// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphBuffer.h"

#include "Razix/Graphics/FrameGraph/Resources/RZTransientResources.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            void RZFrameGraphBuffer::create(const Desc& desc, void* allocator)
            {
                m_Buffer = static_cast<FrameGraph::RZTransientResources*>(allocator)->acquireBuffer(desc);
            }

            void RZFrameGraphBuffer::destroy(const Desc& desc, void* allocator)
            {
                static_cast<FrameGraph::RZTransientResources*>(allocator)->releaseBuffer(desc, m_Buffer);
            }

            std::string RZFrameGraphBuffer::toString(const Desc& desc)
            {
                return "size : " + std::to_string(desc.size) + " bytes";
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
