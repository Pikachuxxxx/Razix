#pragma once

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

/**
 * Now what is happening is when we acquire a image for the first renderer it will  
 */

namespace Razix {
    namespace Graphics {

        const u32 kMAX_DESCRIPTORS_BINDABLE_PER_FRAME = 16;

        class VKRenderContext : public RHI
        {
        public:
            VKRenderContext(u32 width, u32 height);
            ~VKRenderContext();

            static VKRenderContext* GetVKRenderer() { return static_cast<VKRenderContext*>(s_APIInstance); }

        protected:
            void AcquireImageAPIImpl(RZSemaphore* signalSemaphore) override;
            void SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores) override;

            void InitAPIImpl() override;
            void BeginAPIImpl(RZCommandBuffer* cmdBuffer) override;
            void SubmitImpl(RZCommandBuffer* cmdBuffer) override;
            void PresentAPIImpl(RZSemaphore* waitSemaphore) override;
            void BindDescriptorSetAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx) override;
            void BindUserDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets) override;
            void DrawAPIImpl(RZCommandBuffer* cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT) override;
            void DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0) override;
            void DestroyAPIImpl() override;

            void OnResizeAPIImpl(u32 width, u32 height) override;

            RZSwapchain* GetSwapchainImpl() override;

            void BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant) override;
            void SetDepthBiasImpl(RZCommandBuffer* cmdBuffer) override;
            void BindUserDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets) override;
            void SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;
            void EnableBindlessTexturesImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer) override;
            void BindPushDescriptorsImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors) override;

            void BeginRenderingImpl(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo) override;
            void EndRenderingImpl(RZCommandBuffer* cmdBuffer) override;

            void SetCmdCheckpointImpl(RZCommandBuffer* cmdbuffer, void* markerData) override;

        private:
            VKContext*      m_Context; /* Reference to the Vulkan context, we store it to avoid multiple calls */
            VkDescriptorSet m_DescriptorSetPool[kMAX_DESCRIPTORS_BINDABLE_PER_FRAME];
        };
    }    // namespace Graphics
}    // namespace Razix
