#pragma once

#include "Razix/Graphics/RHI/RHI.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Context.h"

namespace Razix {
    namespace Graphics {

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
            void         SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores) override;
            void         BeginAPIImpl(RZCommandBuffer* cmdBuffer) override;
            void         SubmitImpl(RZCommandBuffer* cmdBuffer) override;
            void         PresentAPIImpl(RZSemaphore* waitSemaphore) override;
            void         BindPipelineImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer) override;
            void         BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx) override;
            void         BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx) override;
            void         DrawAPIImpl(RZCommandBuffer* cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT) override;
            void         DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0) override;
            void         DestroyAPIImpl() override;
            void         OnResizeAPIImpl(u32 width, u32 height) override;
            void         BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant) override;
            void         SetViewportImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;
            void         SetDepthBiasImpl(RZCommandBuffer* cmdBuffer) override;
            void         BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx) override;
            void         SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;
            void         EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer) override;
            void         BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors) override;
            void         BeginRenderingImpl(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo) override;
            void         EndRenderingImpl(RZCommandBuffer* cmdBuffer) override;
            void         InsertImageMemoryBarrierImpl(RZCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo) override;
            void         InsertBufferMemoryBarrierImpl(RZCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo) override;
            void         CopyTextureResourceImpl(RZCommandBuffer* cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle) override;
            RZSwapchain* GetSwapchainImpl() override;

        private:
            DX12Context* m_Context = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix

#endif