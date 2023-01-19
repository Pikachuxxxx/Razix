#pragma once

#include "Razix/Graphics/RHI/API/RZRenderPass.h"

namespace Razix {
    namespace Graphics {

        class OpenGLRenderPass : public RZRenderPass
        {
        public:
            OpenGLRenderPass(const RenderPassInfo& renderPassInfo);
            ~OpenGLRenderPass();

            void BeginRenderPass(RZCommandBuffer* commandBuffer, glm::vec4 clearColor, RZFramebuffer* framebuffer, SubPassContents subpass, uint32_t width, uint32_t height) override;
            void EndRenderPass(RZCommandBuffer* commandBuffer) override;
            void AddAttachment() override;
            void Destroy() override;

            bool shouldClear = true;
        };
    }    // namespace Graphics
}    // namespace Razix
