#include "rzxpch.h"
#include "OpenGLSwapchain.h"

#ifdef RAZIX_RENDER_API_OPENGL

namespace Razix {
    namespace Graphics {

        OpenGLSwapchain::OpenGLSwapchain(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {

        }

        void OpenGLSwapchain::Init() {

        }

        void OpenGLSwapchain::Destroy() {

        }

        Texture* OpenGLSwapchain::GetCurrentImage() {
            return nullptr;
        }

        Texture* OpenGLSwapchain::GetImage(uint32_t index) {
            return nullptr;
        }

        uint32_t OpenGLSwapchain::GetCurrentImageIndex() {
            return 0;
        }

        size_t OpenGLSwapchain::GetSwapchainImageCount() {
            return 0;
        }

        CommandBuffer* OpenGLSwapchain::GetCurrentCommandBuffer() {
            return nullptr;
        }

    }
}
#endif