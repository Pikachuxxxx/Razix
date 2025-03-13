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
                RZTexture* textureResource = RZResourceManager::Get().getPool<RZTexture>().get(m_TextureHandle);
                RZTextureDesc textureDesc = CAST_TO_FG_TEX_DESC desc;
                   
                   // Determine the appropriate layout for reading based on texture type and flags
                   ImageLayout newLayout = ImageLayout::kShaderRead;
                   
                   // Special cases for different texture types
                if (textureDesc.format == TextureFormat::DEPTH32F ||
                    textureDesc.format == TextureFormat::DEPTH_STENCIL ||
                    textureDesc.format == TextureFormat::DEPTH16_UNORM)
                   {
                       // Depth textures should use depth read-only layout when read
                       newLayout = ImageLayout::kDepthStencilReadOnly;
                   }
                else if (textureDesc.initResourceViewHints == kTransferSrc)
                   {
                       // When being read as a transfer source
                       newLayout = ImageLayout::kTransferSource;
                   }
                   
                // Get current layout from the resource and transition if needed
                ImageLayout oldLayout = textureResource->getCurrentLayout();
                RZDrawCommandBufferHandle cmdBuffer = RHI::Get().GetCurrentCommandBuffer();
                RHI::InsertImageMemoryBarrier(cmdBuffer, m_TextureHandle, ImageLayout::kColorRenderTarget, ImageLayout::kShaderRead);
                
            }

            void RZFrameGraphTexture::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                
//                RZTexture* textureResource = RZResourceManager::Get().getPool<RZTexture>().get(m_TextureHandle);
//                RZTextureDesc textureDesc = CAST_TO_FG_TEX_DESC desc;
//                
//                // Determine the appropriate layout for writing based on texture type and flags
//                    ImageLayout newLayout = ImageLayout::kShaderWrite;
//                    
//                    // Handle special cases based on texture type and format
//                    if (textureDesc.format == TextureFormat::D24S8 ||
//                        textureDesc.format == TextureFormat::D32F ||
//                        textureDesc.format == TextureFormat::D32FS8)
//                    {
//                        if (flags & WRITE_FLAG_DEPTH_STENCIL)
//                        {
//                            // When writing to depth-stencil
//                            newLayout = ImageLayout::kDepthStencilRenderTarget;
//                        }
//                        else if (flags & WRITE_FLAG_DEPTH_ONLY)
//                        {
//                            // When writing to depth only
//                            newLayout = ImageLayout::kDepthRenderTarget;
//                        }
//                    }
//                    else if (flags & WRITE_FLAG_COLOR_ATTACHMENT)
//                    {
//                        // When writing as color attachment
//                        newLayout = ImageLayout::kColorRenderTarget;
//                    }
//                    else if (flags & WRITE_FLAG_TRANSFER_DST)
//                    {
//                        // When being written as a transfer destination
//                        newLayout = ImageLayout::kTransferDestination;
//                    }
//                    else if (textureDesc.type == TextureType::kSwapchain)
//                    {
//                        // Swapchain textures use special layout
//                        newLayout = ImageLayout::kSwapchain;
//                    }
//                    
//                    // Get current layout from the resource and transition if needed
//                    ImageLayout oldLayout = textureResource->getCurrentLayout();
//                        RZDrawCommandBufferHandle cmdBuffer = RHI::Get().GetCurrentCommandBuffer();
//                RHI::InsertImageMemoryBarrier(cmdBuffer, m_TextureHandle, ImageLayout::kShaderRead, ImageLayout::kColorRenderTarget);

            }
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix
