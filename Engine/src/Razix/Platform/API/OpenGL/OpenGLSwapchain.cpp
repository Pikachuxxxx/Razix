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

        }

        Texture* OpenGLSwapchain::GetImage(uint32_t index) {

        }

        uint32_t OpenGLSwapchain::GetCurrentImageIndex() {

        }

        size_t OpenGLSwapchain::GetSwapchainImageCount() {

        }

        CommandBuffer* OpenGLSwapchain::GetCurrentCommandBuffer() {

        }

    }
}
#endif