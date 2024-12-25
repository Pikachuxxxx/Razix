#pragma once

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

/**
 * Now what is happening is when we acquire a image for the first renderer it will  
 */

namespace Razix {
    namespace Gfx {

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
            void         BeginAPIImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void         SubmitImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void         PresentAPIImpl(RZSemaphore* waitSemaphore) override;
            void         BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer) override;
            void         BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZDescriptorSetHandle descriptorSet, u32 setIdx) override;
            void         BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptorSetHandle>& descriptorSets, u32 startSetIdx) override;
            void         DrawAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT) override;
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
            VKContext*      m_Context; /* Reference to the Vulkan context, we store it to avoid multiple calls */
            VkDescriptorSet m_DescriptorSetCachePool[kMAX_DESCRIPTORS_BINDABLE_PER_FRAME];

        private:
            void SetCmdCheckpointImpl(RZDrawCommandBufferHandle cmdBuffer, void* markerData);
        };
    }    // namespace Gfx
}    // namespace Razix
