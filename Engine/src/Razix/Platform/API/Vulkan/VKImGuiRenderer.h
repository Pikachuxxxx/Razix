#pragma once

#include "Razix/Graphics/Renderers/RZImGuiRenderer.h"

#include "Razix/Platform/API/Vulkan/VKRenderPass.h"
#include "Razix/Platform/API/Vulkan/VKFramebuffer.h"
#include "Razix/Platform/API/Vulkan/VKTexture.h"

struct ImGui_ImplVulkanH_Window;

namespace Razix {
    namespace Graphics {
        
        class VKImGuiRenderer : public RZImGuiRenderer
        {
        public:
            VKImGuiRenderer(uint32_t width, uint32_t height);
            ~VKImGuiRenderer() {}

            void Init() override;
            void NewFrame() override;
            void Render(RZCommandBuffer* commandBuffer) override;
            void EndFrame() override;
            void OnResize(uint32_t width, uint32_t height) override;
            void RebuildFontTexture() override;

        private:
            void* m_WindowHandle;
            uint32_t m_Width;
            uint32_t m_Height;
            VKFramebuffer* m_Framebuffers[3];
            VKRenderPass* m_Renderpass;
            VKTexture2D* m_FontTexture;

        private:
            void FrameRender(ImGui_ImplVulkanH_Window* wd);
            void setupVulkanWindowData(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
        };
    }
}