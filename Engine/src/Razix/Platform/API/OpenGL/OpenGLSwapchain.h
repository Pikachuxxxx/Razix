#pragma once

#include "Razix/Graphics/API/Swapchain.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {
    
        class OpenGLSwapchain : public Swapchain
        {
        public:
            OpenGLSwapchain(uint32_t width, uint32_t height);

            void Init() override;
            void Destroy() override;

            Texture* GetCurrentImage() override;
            Texture * GetImage(uint32_t index) override;
            uint32_t GetCurrentImageIndex() override;
            size_t GetSwapchainImageCount() override;
            CommandBuffer* GetCurrentCommandBuffer() override;

        private:
            uint32_t m_Width;
            uint32_t m_Height;
            uint32_t m_CurrentImageIndex = 0;

        };
    }
}
#endif