#pragma once
#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Platform/API/OpenGL/OpenGLContext.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {

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
            void BeginAPIImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void SubmitImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores) override;
            void PresentAPIImpl(RZSemaphore* waitSemaphore) override;
            void BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer) override;
            void BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx) override;
            void DrawAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT) override;
            void DrawIndexedAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0) override;
            void DestroyAPIImpl() override;

            void OnResizeAPIImpl(u32 width, u32 height) override;

            RZSwapchain* GetSwapchainImpl() override;

            void BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZPushConstant pushConstant) override;
            void SetDepthBiasImpl(RZDrawCommandBufferHandle cmdBuffer) override;
            void BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx) override;
            void SetScissorRectImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;

            void BeginRenderingImpl(RZDrawCommandBufferHandle cmdBuffer, const RenderingInfo& renderingInfo) override;

            void EndRenderingImpl(RZDrawCommandBufferHandle cmdBuffer) override;

            void EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer) override;

            void BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx) override;

            void BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptor>& descriptors) override;

            void InsertImageMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo) override;

            void InsertBufferMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo) override;

            void SetViewportImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) override;

            void CopyTextureResourceImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle) override;

        private:
            OpenGLContext* m_Context; /* Reference to the opengl context, we store it to avoid multiple calls */
        };
    }    // namespace Graphics
}    // namespace Razix

#endif