// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX11Swapchain.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

namespace Razix {
    namespace Graphics {

        void DX11Swapchain::Init(u32 width, u32 height)
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

        Razix::Graphics::RZTextureHandle DX11Swapchain::GetImage(u32 index)
        {
            RAZIX_UNIMPLEMENTED_METHOD
            return {};
        }

        Razix::Graphics::RZTextureHandle DX11Swapchain::GetCurrentImage()
        {
            RAZIX_UNIMPLEMENTED_METHOD
            return {};
        }

        sz DX11Swapchain::GetSwapchainImageCount()
        {
            RAZIX_UNIMPLEMENTED_METHOD
            return NULL;
        }

        u32 DX11Swapchain::getCurrentImageIndex()
        {
            RAZIX_UNIMPLEMENTED_METHOD
            return 0;
        }

    }    // namespace Graphics
}    // namespace Razix
#endif