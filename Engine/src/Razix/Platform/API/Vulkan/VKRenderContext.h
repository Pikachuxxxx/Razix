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

            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, OnImGui)

        protected:
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, InitAPIImpl)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, AcquireImageAPIImpl, RZSemaphore* signalSemaphore)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, SubmitWorkImpl, std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, BeginAPIImpl, RZCommandBuffer* cmdBuffer)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, SubmitImpl, RZCommandBuffer* cmdBuffer)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, PresentAPIImpl, RZSemaphore* waitSemaphore)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, BindPipelineImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, BindDescriptorSetAPImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, BindUserDescriptorSetsAPImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, DrawAPIImpl, RZCommandBuffer* cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, DrawIndexedAPIImpl, RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, DestroyAPIImpl)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, OnResizeAPIImpl, u32 width, u32 height)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, BindPushConstantsAPIImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, SetViewportImpl, RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, SetDepthBiasImpl, RZCommandBuffer* cmdBuffer)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, BindUserDescriptorSetsAPImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, SetScissorRectImpl, RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, EnableBindlessTexturesImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, BindPushDescriptorsImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, BeginRenderingImpl, RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, EndRenderingImpl, RZCommandBuffer* cmdBuffer)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, InsertImageMemoryBarrierImpl, RZCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, InsertBufferMemoryBarrierImpl, RZCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(void, CopyTextureResourceImpl, RZCommandBuffer* cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle)
            RAZIX_VIRTUAL_OVERRIDE_FINAL(RZSwapchain*, GetSwapchainImpl)

        private:
            VKContext*      m_Context; /* Reference to the Vulkan context, we store it to avoid multiple calls */
            VkDescriptorSet m_DescriptorSetPool[kMAX_DESCRIPTORS_BINDABLE_PER_FRAME];

        private:
            void SetCmdCheckpointImpl(RZCommandBuffer* cmdbuffer, void* markerData);
        };
    }    // namespace Graphics
}    // namespace Razix
