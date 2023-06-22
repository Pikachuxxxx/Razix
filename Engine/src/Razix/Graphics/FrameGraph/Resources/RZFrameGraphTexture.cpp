// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"

#include "Razix/Graphics/FrameGraph/Resources/RZTransientResources.h"

#include "Razix/Graphics/RHI/API/RZAPICreateStructs.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            void RZFrameGraphTexture::create(const Desc& desc, void* allocator)
            {
                m_Texture = static_cast<FrameGraph::RZTransientResources*>(allocator)->acquireTexture(desc);
            }

            void RZFrameGraphTexture::destroy(const Desc& desc, void* allocator)
            {
                static_cast<FrameGraph::RZTransientResources*>(allocator)->releaseTexture(desc, m_Texture);
            }

            std::string RZFrameGraphTexture::toString(const Desc& desc)
            {
                // Size, Format
                if (desc.layers > 1)
                    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ", " + std::to_string(desc.layers) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
                else
                    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix