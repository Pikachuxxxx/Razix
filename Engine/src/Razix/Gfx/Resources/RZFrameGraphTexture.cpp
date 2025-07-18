// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {

        void RZFrameGraphTexture::create(const Desc& desc, u32 id, const void* transientAllocator)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //if (transientAllocator)
            //    m_TextureHandle = TRANSIENT_ALLOCATOR_CAST(transientAllocator)->acquireTransientTexture(desc, id);
            //else {
            //    // If no transient allocator is provided, we create a imported persistent resource only ONCE!
            //    if (!m_TextureHandle.isValid())
            //        m_TextureHandle = RZResourceManager::Get().create_texture(desc);
            //}
        }

        void RZFrameGraphTexture::destroy(u32 id, const void* transientAllocator)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            //if (transientAllocator)
            //    TRANSIENT_ALLOCATOR_CAST(transientAllocator)->releaseTransientTexture(m_TextureHandle, id);
            //else {
            //    if (m_TextureHandle.isValid())
            //        RZResourceManager::Get().destroy_texture(m_TextureHandle);
            //}
        }

        void RZFrameGraphTexture::preRead(const Desc& desc, uint32_t flags)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            #if 0
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
#ifndef RAZIX_GOLD_MASTER
            if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                RAZIX_CORE_INFO("[ReadBarrier::Texture] resource name: {0} | old layout: {1} | new layout: {2}", textureDesc.name, ImageLayoutNames[(u32) oldLayout], ImageLayoutNames[(u32) newLayout]);
#endif
            //RHI::InsertImageMemoryBarrier(RHI::Get().GetCurrentCommandBuffer(), m_TextureHandle, oldLayout, newLayout);
      #endif
        }

        void RZFrameGraphTexture::preWrite(const Desc& desc, uint32_t flags)
        {
#if 0

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
#ifndef RAZIX_GOLD_MASTER
            if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                RAZIX_CORE_INFO("[WriteBarrier::Texture] resource name: {0} | old layout: {1} | new layout: {2}", textureDesc.name, ImageLayoutNames[(u32) oldLayout], ImageLayoutNames[(u32) newLayout]);
#endif
            RHI::InsertImageMemoryBarrier(RHI::Get().GetCurrentCommandBuffer(), m_TextureHandle, oldLayout, newLayout);
            #endif
        }

        void RZFrameGraphTexture::resize(u32 width, u32 height)
        {
            //RZTexture* textureResource = RZResourceManager::Get().getPool<RZTexture>().get(m_TextureHandle);
            //textureResource->Resize(width, height);
        }

        std::string RZFrameGraphTexture::toString(const Desc& desc)
        {
            //if (desc.layers > 1)
            //    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ", " + std::to_string(desc.layers) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
            //else
            //    return "(" + std::to_string(int(desc.width)) + ", " + std::to_string(int(desc.height)) + ") - " + RZTextureDesc::FormatToString(desc.format) + " [" + RZTextureDesc::TypeToString(desc.type) + "]";
        }

    }    // namespace Gfx
}    // namespace Razix
