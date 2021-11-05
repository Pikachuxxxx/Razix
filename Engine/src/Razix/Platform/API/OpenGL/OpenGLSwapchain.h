#pragma once

#include "Razix/Graphics/API/Swapchain.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {
    
        /* OpenGL Implementation of the swapchain */
        class OpenGLSwapchain : public RZSwapchain
        {
        public:
            OpenGLSwapchain(uint32_t width, uint32_t height);

            void Init() override{}
            void Destroy() override {}
            void Flip() override;

            RZTexture* GetCurrentImage() override { return nullptr; }
            RZTexture * GetImage(uint32_t index) override {return nullptr;}
            size_t GetSwapchainImageCount() override { return 0; }
            RZCommandBuffer* GetCurrentCommandBuffer() override {return nullptr;}

        private:
            uint32_t m_Width;
            uint32_t m_Height;
            uint32_t m_CurrentImageIndex = 0;

        };
    }
}
#endif