#pragma once

namespace Razix {
    namespace Gfx {

        /**
         * Memory Barriers 
         * 1. Image
         * 2. Buffer
         */

        /**
         * Pipeline Barriers
         */

        /* The order of enums is how the GPU execution takes place */
        enum PipelineStage : u32
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
            kBottomOfPipe,
            PipelineStage_COUNT
        };

        enum ImageLayout : u32
        {
            kNewlyCreated,
            kGeneric,
            kSwapchain,
            kColorRenderTarget,
            kDepthRenderTarget,
            kDepthStencilRenderTarget,
            kDepthStencilReadOnly,
            kShaderAttachment,
            kAttachment,
            kTransferSource,
            kTransferDestination,
            ImageLayout_COUNT
        };

        enum MemoryAccessMask : u32
        {
            kMemoryAccessNone = 0,
            kIndirectCommandReadBit,
            kIndexBufferDataReadBit,
            kVertexAttributeReadBit,
            kUniformReadBit,
            kInputAttachmentReadBit,
            kShaderReadBit,
            kShaderWriteBit,
            kColorAttachmentReadBit,
            kColorAttachmentWriteBit,
            kColorAttachmentReadWriteBit,
            kDepthStencilAttachmentReadBit,
            kDepthStencilAttachmentWriteBit,
            kTransferReadBit,
            kTransferWriteBit,
            kHostReadBit,
            kHostWriteBit,
            kMemoryReadBit,
            kMemoryWriteBit,
            MemoryAccessMask_COUNT
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
    }    // namespace Gfx
}    // namespace Razix
