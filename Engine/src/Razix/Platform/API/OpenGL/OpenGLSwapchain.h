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
            OpenGLSwapchain(u32 width, u32 height);

            void  Init(u32 width, u32 height) override {}
            void  Destroy() override {}
            void  Flip() override;
            void  OnResize(u32 width, u32 height) override {}
            void* GetAPIHandle() override { return nullptr; }

            RZTextureHandle GetCurrentImage() override { return {}; }
            RZTextureHandle GetImage(u32 index) override { return {}; }
            sz              GetSwapchainImageCount() override { return 1; }

            u32 getCurrentImageIndex() override { return 0; }

        private:
            u32                  m_Width;
            u32                  m_Height;
            u32                  m_CurrentImageIndex = 0;
            OpenGLCommandBuffer* m_DummyCmdBuffer;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif