// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"

#include "Razix/Graphics/Resources/RZTransientResources.h"

#include "Razix/Graphics/RHI/API/RZAPICreateStructs.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            // TODO: Use a TransientResourcePool to allocate transient resources

            void RZFrameGraphTexture::create(const Desc& desc, void* transientAllocator)
            {
                if (!m_TextureHandle.isValid())
                    m_TextureHandle = RZResourceManager::Get().createTexture(desc);    //static_cast<FrameGraph::RZTransientResources*>(allocator)->acquireTexture(desc);
            }

            void RZFrameGraphTexture::destroy(const Desc& desc, void* transientAllocator)
            {
                //static_cast<FrameGraph::RZTransientResources*>(allocator)->releaseTexture(desc, m_Texture);
                //RZResourceManager::Get().releaseTexture(m_TextureHandle);
            }

            std::string RZFrameGraphTexture::toString(const Desc& desc)
            {
                // Size, Format
                if (desc.layers > 1)
                    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ", " + std::to_string(desc.layers) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
                else
                    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
            }

            void RZFrameGraphTexture::preRead(const Desc& desc, uint32_t flags)
            {
            }

            void RZFrameGraphTexture::preWrite(const Desc& desc, uint32_t flags)
            {
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix