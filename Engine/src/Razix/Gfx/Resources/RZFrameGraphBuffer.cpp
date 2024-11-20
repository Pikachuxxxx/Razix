// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphBuffer.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

#include "Razix/Gfx/RHI/API/RZAPIDesc.h"
#include "Razix/Gfx/RHI/API/RZBindingInfoAccessViews.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {
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

                // Unlike images they are already in proper and fixed state

                //PipelineStage endStage;
                //if (info.stage == ShaderStage::Vertex)
                //    endStage = PipelineStage::kVertexShader;
                //else if (info.stage == ShaderStage::Pixel)
                //    endStage = PipelineStage::kFragmentShader;
                //else if (info.stage == ShaderStage::Compute)
                //    endStage = PipelineStage::kComputeShader;
                //
                //Graphics::RHI::InsertBufferMemoryBarrier(Graphics::RHI::GetCurrentCommandBuffer(), m_BufferHandle, {.startExecutionStage = PipelineStage::kTopOfPipe, .endExecutionStage = endStage}, {.srcAccess = MemoryAccessMask::})

                // Get the Biding info from the flags
                if (flags != FrameGraph::kFlagsNone)
                    DescriptorBindingInfo info = Gfx::DecodeDescriptorBindingInfo(flags);
                else
                    return;
            }

            void RZFrameGraphBuffer::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // Unlike images they are already in proper and fixed state

                // Get the Biding info from the flags
                if (flags != FrameGraph::kFlagsNone)
                    DescriptorBindingInfo info = Gfx::DecodeDescriptorBindingInfo(flags);
                else
                    return;
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
