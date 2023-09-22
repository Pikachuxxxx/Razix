// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"



#include "Razix/Graphics/RHI/API/RZAPIDesc.h"

#include "Razix/Graphics/RHI/API/RZBindingInfoAccessViews.h"

#include "Razix/Graphics/RHI/RHI.h"

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
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // Get the Biding info from the flags
                DescriptorBindingInfo info = Graphics::DecodeDescriptorBindingInfo(flags);

                // Note: Disabled cause validation layers are not complaining

                //Graphics::RHI::InsertImageMemoryBarrier(Graphics::RHI::GetCurrentCommandBuffer(), m_TextureHandle, {.startExecutionStage = PipelineStage::kColorAttachmentOutput, .endExecutionStage = PipelineStage::kFragmentShader}, {.srcAccess = MemoryAccessMask::kColorAttachmentReadWriteBit, .dstAccess = MemoryAccessMask::kShaderReadBit, .srcLayout = ImageLayout::kColorAttachmentOptimal, .dstLayout = ImageLayout::kShaderReadOnlyOptimal});
            }

            void RZFrameGraphTexture::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                /**
                 * We use RenderingInfo struct to build the render targets + attach info and we don't have to build them
                 * in the preWrite cause RHI doesn't have functions like RHI::SetDepthRenderTarget or RHI::SetRenderTarget(handle, slotIdx)
                 * 
                 * Instead we use the rendering info struct and pass it to RHI::BeginRendering, now this is done inside the exectue function
                 * but I think we can have the frame graph execute function handle this struct and call the RHI internally
                 * 
                 */

                // Note: Disabled cause validation layers are not complaining

                //Graphics::RHI::InsertImageMemoryBarrier(Graphics::RHI::GetCurrentCommandBuffer(), m_TextureHandle, {.startExecutionStage = PipelineStage::kTopOfPipe, .endExecutionStage = PipelineStage::kColorAttachmentOutput}, {.srcAccess = MemoryAccessMask::kNone, .dstAccess = MemoryAccessMask::kColorAttachmentReadWriteBit, .srcLayout = ImageLayout::kUndefined, .dstLayout = ImageLayout::kColorAttachmentOptimal});
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix