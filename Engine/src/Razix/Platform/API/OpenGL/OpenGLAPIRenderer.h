#pragma once
#include "Razix/Graphics/API/RZAPIRenderer.h"

#include "Razix/Platform/API/OpenGL/OpenGLContext.h"

namespace Razix {
    namespace Graphics {

        class OpenGLAPIRenderer : public RZAPIRenderer
        {
        public:
        public:
            OpenGLAPIRenderer(uint32_t width, uint32_t height);
            ~OpenGLAPIRenderer();

            static OpenGLAPIRenderer* GetGLRenderer() { return static_cast<OpenGLAPIRenderer*>(s_APIInstance); }

        protected:
            void InitAPIImpl() override;
            void AcquireImageAPIImpl() override {}
            void BeginAPIImpl(RZCommandBuffer* cmdBuffer) override;
            void SubmitImpl(RZCommandBuffer* cmdBuffer) override;
            void SubmitWorkImpl() override;
            void PresentAPIImpl() override;
            void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets) override;
            void DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType = DataType::UNSIGNED_INT) override;
            void DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) override;
            void DestroyAPIImpl() override;

            void OnResizeAPIImpl(uint32_t width, uint32_t height) override;

            RZSwapchain* GetSwapchainImpl() override;

            void BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant) override;
            void SetDepthBiasImpl(RZCommandBuffer* cmdBuffer) override;
            void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, uint32_t totalSets) override;
            void SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, uint32_t width, uint32_t height) override;

        private:
            OpenGLContext* m_Context; /* Reference to the opengl context, we store it to avoid multiple calls */
        };
    }    // namespace Graphics
}    // namespace Razix
