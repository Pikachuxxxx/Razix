// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphBuffer.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/RHI.h"

    namespace Razix
{
    namespace Gfx {
        namespace FrameGraph {

            void RZFrameGraphBuffer::create(const Desc& desc, void* transientAllocator)
            {
                if (!m_BufferHandle.isValid())
                    m_BufferHandle = RZResourceManager::Get().createUniformBuffer(desc);
            }

            void RZFrameGraphBuffer::destroy(const Desc& desc, void* transientAllocator)
            {
                RZResourceManager::Get().destroyUniformBuffer(m_BufferHandle);
            }
        
            // TODO: use a combination of BufferUsage and resourceViewHints to deduce, we don't have ImageLayout and needs to do more intelligent tracking to deducte barrier types

            void RZFrameGraphBuffer::preRead(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                RZBufferDesc bufferDesc         = CAST_TO_FG_BUF_DESC desc;

                BufferBarrierType barrierType = BufferBarrierType::ShaderWriteToShaderRead;

                if ((bufferDesc.initResourceViewHints & kSRV) == kSRV) {
                    barrierType = BufferBarrierType::ShaderWriteToShaderRead;
                } else if ((bufferDesc.initResourceViewHints & kCBV) == kCBV) {
                    barrierType = BufferBarrierType::CPUToGPU;
                } else if ((bufferDesc.initResourceViewHints & kTransferSrc) == kTransferSrc) {
                    barrierType = BufferBarrierType::TransferDstToShaderRead;
                }

                if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                    RAZIX_CORE_INFO("[ReadBarrier::Buffer] resource name: {0} | barrier type: {1}", bufferDesc.name, BufferBarrierTypeNames[(u32) barrierType]);

                RHI::InsertBufferMemoryBarrier(RHI::Get().GetCurrentCommandBuffer(), m_BufferHandle, barrierType);
            }

            void RZFrameGraphBuffer::preWrite(const Desc& desc, uint32_t flags)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                RZBufferDesc bufferDesc         = CAST_TO_FG_BUF_DESC desc;

                BufferBarrierType barrierType = BufferBarrierType::ShaderReadToShaderWrite;

                if ((bufferDesc.initResourceViewHints & kUAV) == kUAV) {
                    barrierType = BufferBarrierType::ShaderReadToShaderWrite;
                } else if ((bufferDesc.initResourceViewHints & kTransferDst) == kTransferDst) {
                    barrierType = BufferBarrierType::CPUToGPU;
                } else if ((bufferDesc.initResourceViewHints & kCBV) == kCBV) {
                    barrierType = BufferBarrierType::CPUToGPU;
                }

                if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                    RAZIX_CORE_INFO("[WriteBarrier::Buffer] resource name: {0} | barrier type: {1}", bufferDesc.name, BufferBarrierTypeNames[(u32) barrierType]);

                RHI::InsertBufferMemoryBarrier(RHI::Get().GetCurrentCommandBuffer(), m_BufferHandle, barrierType);
            }

            std::string RZFrameGraphBuffer::toString(const Desc& desc)
            {
                return "size : " + std::to_string(desc.size) + " bytes";
            }
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix
