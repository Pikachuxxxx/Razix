// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX11Swapchain.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

namespace Razix {
    namespace Graphics {

        void DX11Swapchain::Init(uint32_t width, uint32_t height)
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX11Swapchain::Destroy()
        {
            m_Target.Reset();
            m_Swapchain.Reset();
        }

        void DX11Swapchain::Flip()
        {
            m_Swapchain->Present(1u, 0u);
        }

        Razix::Graphics::RZTexture* DX11Swapchain::GetImage(uint32_t index)
        {
            RAZIX_UNIMPLEMENTED_METHOD
                return nullptr;
        }

        Razix::Graphics::RZTexture* DX11Swapchain::GetCurrentImage()
        {
            RAZIX_UNIMPLEMENTED_METHOD
                return nullptr;
        }

        size_t DX11Swapchain::GetSwapchainImageCount()
        {
            RAZIX_UNIMPLEMENTED_METHOD
                return NULL;
        }

        RZCommandBuffer* DX11Swapchain::getCurrentCommandBuffer()
        {
            RAZIX_UNIMPLEMENTED_METHOD
                return nullptr;
        }

        uint32_t DX11Swapchain::getCurrentImageIndex()
        {
            RAZIX_UNIMPLEMENTED_METHOD
                return 0;
        }

    }
}
#endif