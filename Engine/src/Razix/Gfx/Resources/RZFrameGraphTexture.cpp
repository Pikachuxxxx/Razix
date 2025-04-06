// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"

#include "Razix/Core/RZEngine.h"

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
                if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                    RAZIX_CORE_INFO("[ReadBarrier::Texture] resource name: {0} | old layout: {1} | new layout: {2}", textureDesc.name, ImageLayoutNames[(u32) oldLayout], ImageLayoutNames[(u32) newLayout]);
                RHI::InsertImageMemoryBarrier(RHI::Get().GetCurrentCommandBuffer(), m_TextureHandle, oldLayout, newLayout);
            }

            void RZFrameGraphTexture::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                RZTexture*    textureResource = RZResourceManager::Get().getPool<RZTexture>().get(m_TextureHandle);
                RZTextureDesc textureDesc     = CAST_TO_FG_TEX_DESC desc;

                ImageLayout newLayout = ImageLayout::kColorRenderTarget;

                if (textureDesc.format == TextureFormat::DEPTH32F ||
                    textureDesc.format == TextureFormat::DEPTH16_UNORM) {
                    newLayout = ImageLayout::kDepthStencilRenderTarget;
                } else if (textureDesc.format == TextureFormat::DEPTH_STENCIL) {
                    newLayout = ImageLayout::kDepthRenderTarget;
                } else if ((textureDesc.initResourceViewHints & kUAV) == kUAV) {
                    newLayout = ImageLayout::kShaderWrite;
                } else if ((textureDesc.initResourceViewHints & kTransferDst) == kTransferDst) {
                    newLayout = ImageLayout::kTransferDestination;
                } else if (textureDesc.format == TextureFormat::SCREEN) {
                    newLayout = ImageLayout::kSwapchain;
                }

                ImageLayout oldLayout = textureResource->getCurrentLayout();
                if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                    RAZIX_CORE_INFO("[WriteBarrier::Texture] resource name: {0} | old layout: {1} | new layout: {2}", textureDesc.name, ImageLayoutNames[(u32) oldLayout], ImageLayoutNames[(u32) newLayout]);
                RHI::InsertImageMemoryBarrier(RHI::Get().GetCurrentCommandBuffer(), m_TextureHandle, oldLayout, newLayout);
            }

            void RZFrameGraphTexture::resize(u32 width, u32 height)
            {
                RZTexture* textureResource = RZResourceManager::Get().getPool<RZTexture>().get(m_TextureHandle);
                textureResource->Resize(width, height);
            }

            std::string RZFrameGraphTexture::toString(const Desc& desc)
            {
                if (desc.layers > 1)
                    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ", " + std::to_string(desc.layers) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
                else
                    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
            }

        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix
