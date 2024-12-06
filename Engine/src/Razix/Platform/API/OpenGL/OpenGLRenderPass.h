#pragma once

#include "Razix/Graphics/RHI/API/RZRenderPass.h"

namespace Razix {
    namespace Graphics {

        class OpenGLRenderPass : public RZRenderPass
        {
        public:
            OpenGLRenderPass(const RenderPassInfo& renderPassInfo);
            ~OpenGLRenderPass();

            void BeginRenderPass(RZDrawCommandBufferHandle cmdBuffer, glm::vec4 clearColor, RZFramebuffer* framebuffer, SubPassContents subpass, u32 width, u32 height) override;
            void EndRenderPass(RZDrawCommandBufferHandle cmdBuffer) override;
            void AddAttachment() override;
            void Destroy() override;

            bool shouldClear = true;
        };
    }    // namespace Graphics
}    // namespace Razix
