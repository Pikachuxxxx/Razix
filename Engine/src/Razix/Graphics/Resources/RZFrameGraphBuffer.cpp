// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphBuffer.h"



#include "Razix/Graphics/RHI/API/RZBindingInfoAccessViews.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            void RZFrameGraphBuffer::create(const Desc& desc, void* transientAllocator)
            {
                if (!m_BufferHandle.isValid())
                    m_BufferHandle = RZResourceManager::Get().createUniformBuffer(desc); 
            }

            void RZFrameGraphBuffer::destroy(const Desc& desc, void* transientAllocator)
            {
                //static_cast<FrameGraph::RZTransientResources*>(transientAllocator)->releaseBuffer(desc, m_Buffer);
            }

            std::string RZFrameGraphBuffer::toString(const Desc& desc)
            {
                return "size : " + std::to_string(desc.size) + " bytes";
            }

            void RZFrameGraphBuffer::preRead(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // Get the Biding info from the flags
                DescriptorBindingInfo info = Graphics::DecodeDescriptorBindingInfo(flags);

            }

            void RZFrameGraphBuffer::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
