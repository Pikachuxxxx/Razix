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

        protected:
            void InitAPIImpl() override;
            void AcquireImageAPIImpl(RZSemaphore* signalSemaphore) override {}
            void BeginAPIImpl(RZCommandBuffer* cmdBuffer) override;
            void SubmitImpl(RZCommandBuffer* cmdBuffer) override;
            void SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores) override;
            void PresentAPIImpl(RZSemaphore* waitSemaphore) override;
            void BindPipelineImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer) override;
            void BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets) override;
            void DrawAPIImpl(RZCommandBuffer* cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT) override;
            void DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0) override;
            void DestroyAPIImpl() override;

            void OnResizeAPIImpl(u32 width, u32 height) override;

            RZSwapchain* GetSwapchainImpl() override;

            void BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant) override;
            void SetDepthBiasImpl(RZCommandBuffer* cmdBuffer) override;
            void BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets) override;
            void SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;

            void BeginRenderingImpl(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo) override;

            void EndRenderingImpl(RZCommandBuffer* cmdBuffer) override;

            void EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer) override;

            void BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx) override;

            void BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors) override;

            void InsertImageMemoryBarrierImpl(RZCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo) override;

            void InsertBufferMemoryBarrierImpl(RZCommandBuffer* cmdBuffer, RZUniformBuffer* buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo) override;

            void SetViewportImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;

        private:
            OpenGLContext* m_Context; /* Reference to the opengl context, we store it to avoid multiple calls */
        };
    }    // namespace Graphics
}    // namespace Razix
