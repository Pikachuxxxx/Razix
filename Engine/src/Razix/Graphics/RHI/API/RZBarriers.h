#pragma once

namespace Razix {
    namespace Graphics {

        /**
         * Memory Barriers 
         * 1. Image
         * 2. Buffer
         */

        /**
         * Pipeline Barriers
         */

        enum class PipelineStage : u32
        {
            kTopOfPipe = 0,
            kDrawIndirect,
            kDraw,
            kVertexInput,
            kVertexShader,
            kTessellationControlShader,
            kTessellationEvaluationShader,
            kGeometryShader,
            kFragmentShader,
            kEarlyFragmentTests,
            kLateFragmentTests,
            kEarlyOrLateTests,
            kColorAttachmentOutput,
            kComputeShader,
            kTransfer,
            kMeshShader,
            kTaskShader,
            kBottomOfPipe
        };

        enum class ImageLayout : u32
        {
            kUndefined = 0,
            kGeneral,
            kColorAttachmentOptimal,
            kDepthStencilAttachmentOptimal,
            kDepthStencilReadOnlyOptimal,
            kShaderReadOnlyOptimal,
            kTransferSrcOptimal,
            kTransferDstOptimal,
            kPresentationEngine
        };

        enum class MemoryAccessMask : u32
        {
            kIndirectCommandReadBit = 0,
            kIndexBufferDataReadBit,
            kVertexAttributeReadBit,
            kUniformReadBit,
            kInputAttachmentReadBit,
            kShaderReadBit,
            kShaderWriteBit,
            kColorAttachmentReadBit,
            kColorAttachmentWriteBit,
            kDepthStencilAttachmentReadBit,
            kDepthStencilAttachmentWriteBit,
            kTransferReadBit,
            kTransferWriteBit,
            kHostReadBit,
            kHostWriteBit,
            kMemoryReadBit,
            kMemoryWriteBit
        };

        struct PipelineBarrierInfo
        {
            PipelineStage startExecutionStage;
            PipelineStage endExecutionStage;
        };

        struct ImageMemoryBarrierInfo
        {
            MemoryAccessMask srcAccess;
            MemoryAccessMask dstAccess;
            ImageLayout      srcLayout;
            ImageLayout      dstLayout;
        };

        struct BufferMemoryBarrierInfo
        {
            MemoryAccessMask srcAccess;
            MemoryAccessMask dstAccess;
        };
    }    // namespace Graphics
}    // namespace Razix
