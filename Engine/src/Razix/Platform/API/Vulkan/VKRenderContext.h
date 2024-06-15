#pragma once

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

/**
 * Now what is happening is when we acquire a image for the first renderer it will  
 */

namespace Razix {
    namespace Graphics {

        const u32 kMAX_DESCRIPTORS_BINDABLE_PER_FRAME = 16;

        class VKRenderContext final : public RHI
        {
        public:
            VKRenderContext(u32 width, u32 height);
            ~VKRenderContext() {}

            static VKRenderContext* GetVKRenderer() { return static_cast<VKRenderContext*>(s_APIInstance); }

            void OnImGui() override;

        protected:
            void         InitAPIImpl() override;
            void         AcquireImageAPIImpl(RZSemaphore* signalSemaphore) override;
            void         SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores) override;
            void         BeginAPIImpl(RZDrawCommandBuffer* cmdBuffer) override;
            void         SubmitImpl(RZDrawCommandBuffer* cmdBuffer) override;
            void         PresentAPIImpl(RZSemaphore* waitSemaphore) override;
            void         BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer) override;
            void         BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx) override;
            void         BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx) override;
            void         DrawAPIImpl(RZDrawCommandBuffer* cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT) override;
            void         DrawIndexedAPIImpl(RZDrawCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0) override;
            void         DestroyAPIImpl() override;
            void         OnResizeAPIImpl(u32 width, u32 height) override;
            void         BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, RZPushConstant pushConstant) override;
            void         SetViewportImpl(RZDrawCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;
            void         SetDepthBiasImpl(RZDrawCommandBuffer* cmdBuffer) override;
            void         BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx) override;
            void         SetScissorRectImpl(RZDrawCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;
            void         EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer) override;
            void         BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors) override;
            void         BeginRenderingImpl(RZDrawCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo) override;
            void         EndRenderingImpl(RZDrawCommandBuffer* cmdBuffer) override;
            void         InsertImageMemoryBarrierImpl(RZDrawCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo) override;
            void         InsertBufferMemoryBarrierImpl(RZDrawCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo) override;
            void         CopyTextureResourceImpl(RZDrawCommandBuffer* cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle) override;
            RZSwapchain* GetSwapchainImpl() override;

        private:
            VKContext*      m_Context; /* Reference to the Vulkan context, we store it to avoid multiple calls */
            VkDescriptorSet m_DescriptorSetPool[kMAX_DESCRIPTORS_BINDABLE_PER_FRAME];

        private:
            void SetCmdCheckpointImpl(RZDrawCommandBuffer* cmdbuffer, void* markerData);
        };
    }    // namespace Graphics
}    // namespace Razix
