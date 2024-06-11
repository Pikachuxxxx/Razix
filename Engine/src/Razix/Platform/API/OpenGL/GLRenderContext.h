#pragma once
#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Platform/API/OpenGL/OpenGLContext.h"

namespace Razix {
    namespace Graphics {

        // TODO: FIX THIS!!!

        class GLRenderContext : public RHI
        {
        public:
        public:
            GLRenderContext(u32 width, u32 height);
            ~GLRenderContext();

            static GLRenderContext* GetGLRenderer() { return static_cast<GLRenderContext*>(s_APIInstance); }

            void OnImGui() override {}

        protected:
            void InitAPIImpl() override;
            void AcquireImageAPIImpl(RZSemaphore* signalSemaphore) override {}
            void BeginAPIImpl(RZDrawCommandBuffer* cmdBuffer) override;
            void SubmitImpl(RZDrawCommandBuffer* cmdBuffer) override;
            void SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores) override;
            void PresentAPIImpl(RZSemaphore* waitSemaphore) override;
            void BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer) override;
            void BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx) override;
            void DrawAPIImpl(RZDrawCommandBuffer* cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT) override;
            void DrawIndexedAPIImpl(RZDrawCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0) override;
            void DestroyAPIImpl() override;

            void OnResizeAPIImpl(u32 width, u32 height) override;

            RZSwapchain* GetSwapchainImpl() override;

            void BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, RZPushConstant pushConstant) override;
            void SetDepthBiasImpl(RZDrawCommandBuffer* cmdBuffer) override;
            void BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx) override;
            void SetScissorRectImpl(RZDrawCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;

            void BeginRenderingImpl(RZDrawCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo) override;

            void EndRenderingImpl(RZDrawCommandBuffer* cmdBuffer) override;

            void EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer) override;

            void BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx) override;

            void BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors) override;

            void InsertImageMemoryBarrierImpl(RZDrawCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo) override;

            void InsertBufferMemoryBarrierImpl(RZDrawCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo) override;

            void SetViewportImpl(RZDrawCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;

            void CopyTextureResourceImpl(RZDrawCommandBuffer* cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle) override;

        private:
            OpenGLContext* m_Context; /* Reference to the opengl context, we store it to avoid multiple calls */
        };
    }    // namespace Graphics
}    // namespace Razix
