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
                if (desc.layers > 1)
                    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ", " + std::to_string(desc.layers) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
                else
                    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
            }

            void RZFrameGraphTexture::preRead(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                RZTexture*    textureResource = RZResourceManager::Get().getPool<RZTexture>().get(m_TextureHandle);
                RZTextureDesc textureDesc     = CAST_TO_FG_TEX_DESC desc;

                ImageLayout newLayout = ImageLayout::kShaderRead;

                if (textureDesc.format == TextureFormat::DEPTH32F ||
                    textureDesc.format == TextureFormat::DEPTH_STENCIL ||
                    textureDesc.format == TextureFormat::DEPTH16_UNORM) {
                    newLayout = ImageLayout::kDepthStencilReadOnly;
                } else if ((textureDesc.initResourceViewHints & kTransferSrc) == kTransferSrc) {
                    newLayout = ImageLayout::kTransferSource;
                }

                ImageLayout oldLayout = textureResource->getCurrentLayout();
                RHI::InsertImageMemoryBarrier(RHI::Get().GetCurrentCommandBuffer(), m_TextureHandle, oldLayout, newLayout);
            }

            void RZFrameGraphTexture::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                RZTexture*    textureResource = RZResourceManager::Get().getPool<RZTexture>().get(m_TextureHandle);
                RZTextureDesc textureDesc     = CAST_TO_FG_TEX_DESC desc;

                ImageLayout newLayout = ImageLayout::kShaderWrite;

                if (textureDesc.format == TextureFormat::DEPTH32F ||
                    textureDesc.format == TextureFormat::DEPTH16_UNORM) {
                    newLayout = ImageLayout::kDepthStencilRenderTarget;
                } else if (textureDesc.format == TextureFormat::DEPTH_STENCIL) {
                    newLayout = ImageLayout::kDepthRenderTarget;
                } else if ((textureDesc.initResourceViewHints & kRTV) == kRTV) {
                    newLayout = ImageLayout::kColorRenderTarget;
                } else if ((textureDesc.initResourceViewHints & kTransferDst) == kTransferDst) {
                    newLayout = ImageLayout::kTransferDestination;
                } else if (textureDesc.format == TextureFormat::SCREEN) {
                    newLayout = ImageLayout::kSwapchain;
                }

                ImageLayout oldLayout = textureResource->getCurrentLayout();
                RHI::InsertImageMemoryBarrier(RHI::Get().GetCurrentCommandBuffer(), m_TextureHandle, oldLayout, newLayout);
            }
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix
