// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphBuffer.h"

#include "Razix/Graphics/Resources/RZTransientResources.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            void RZFrameGraphBuffer::create(const Desc& desc, void* transientAllocator)
            {
                m_Buffer = static_cast<FrameGraph::RZTransientResources*>(transientAllocator)->acquireBuffer(desc);
            }

            void RZFrameGraphBuffer::destroy(const Desc& desc, void* transientAllocator)
            {
                static_cast<FrameGraph::RZTransientResources*>(transientAllocator)->releaseBuffer(desc, m_Buffer);
            }

            std::string RZFrameGraphBuffer::toString(const Desc& desc)
            {
                return "size : " + std::to_string(desc.size) + " bytes";
            }

            void RZFrameGraphBuffer::preRead(const Desc& desc, uint32_t flags)
            {
            }

            void RZFrameGraphBuffer::preWrite(const Desc& desc, uint32_t flags)
            {
            }

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
