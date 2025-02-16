// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {
        namespace FrameGraph {

            // TODO: Use a TransientResourcePool to allocate transient resources

            void RZFrameGraphTexture::create(const Desc& desc, void* transientAllocator)
            {
                if (!m_TextureHandle.isValid())
                    m_TextureHandle = RZResourceManager::Get().createTexture(desc);
            }

            void RZFrameGraphTexture::destroy(const Desc& desc, void* transientAllocator)
            {
                RZResourceManager::Get().destroyTexture(m_TextureHandle);
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
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // TODO: Handle other types of layout based using automatic deduction inside frame graph
            }

            void RZFrameGraphTexture::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            }
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix
