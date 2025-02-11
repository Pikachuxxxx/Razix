#pragma once

#include "Razix/Gfx/RHI/RHI.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Context.h"

namespace Razix {
    namespace Gfx {

        class DX12RenderContext : public RHI
        {
        public:
            DX12RenderContext(u32 width, u32 height);
            ~DX12RenderContext() {}

            static DX12RenderContext* GetVKRenderer() { return static_cast<DX12RenderContext*>(s_APIInstance); }

            void OnImGui() override;

        protected:
            void         InitAPIImpl() override;
            void         AcquireImageAPIImpl(RZSemaphore* signalSemaphore) override;
            void         BeginAPIImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void         SubmitImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void         PresentAPIImpl(RZSemaphore* waitSemaphore) override;
            void         BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer) override;
            void         BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZDescriptorSetHandle descriptorSet, u32 setIdx) override;
            void         BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptorSetHandle>& descriptorSets, u32 startSetIdx) override;
            void         DrawAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 count, DrawDataType datayType = DrawDataType ::UNSIGNED_INT) override;
            void         DrawIndexedAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0) override;
            void         DispatchAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 groupX, u32 groupY, u32 groupZ) override;
            void         DestroyAPIImpl() override;
            void         OnResizeAPIImpl(u32 width, u32 height) override;
            void         BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZPushConstant pushConstant) override;
            void         SetViewportImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;
            void         SetDepthBiasImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void         SetScissorRectImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;
            void         EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer) override;
            void         BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptor>& descriptors) override;
            void         BeginRenderingImpl(RZDrawCommandBufferHandle cmdBuffer, const RenderingInfo& renderingInfo) override;
            void         EndRenderingImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void         InsertImageMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo) override;
            void         InsertBufferMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo) override;
            void         CopyTextureResourceImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle) override;
            RZSwapchain* GetSwapchainImpl() override;

        private:
            DX12Context*                m_Context = nullptr;
            rzstl::UniqueRef<DX12Fence> m_RenderReadyFence;
            u64                         m_InflightFramesFenceValues[RAZIX_MAX_FRAMES] = {0, 0, 0};
        };
    }    // namespace Gfx
}    // namespace Razix

#endif