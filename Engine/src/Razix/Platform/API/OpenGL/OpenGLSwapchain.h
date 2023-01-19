#pragma once

#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Platform/API/OpenGL/OpenGLCommandBuffer.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {

        /* OpenGL Implementation of the swapchain */
        class OpenGLSwapchain : public RZSwapchain
        {
        public:
            OpenGLSwapchain(uint32_t width, uint32_t height);

            void  Init(uint32_t width, uint32_t height) override {}
            void  Destroy() override {}
            void  Flip() override;
            void  OnResize(uint32_t width, uint32_t height) override {}
            void* GetAPIHandle() override { return nullptr; }

            RZTexture*       GetCurrentImage() override { return nullptr; }
            RZTexture*       GetImage(uint32_t index) override { return nullptr; }
            size_t           GetSwapchainImageCount() override { return 1; }

            uint32_t getCurrentImageIndex() override { return 0; }

        private:
            uint32_t m_Width;
            uint32_t m_Height;
            uint32_t m_CurrentImageIndex = 0;
            OpenGLCommandBuffer* m_DummyCmdBuffer;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif