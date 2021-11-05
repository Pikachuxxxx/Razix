#include "rzxpch.h"
#include "DX11Swapchain.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

namespace Razix {
    namespace Graphics {

        void DX11Swapchain::Init()
        {
            throw std::logic_error("The method or operation is not implemented.");
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
            throw std::logic_error("The method or operation is not implemented.");
        }

        Razix::Graphics::RZTexture* DX11Swapchain::GetCurrentImage()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        size_t DX11Swapchain::GetSwapchainImageCount()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        RZCommandBuffer* DX11Swapchain::GetCurrentCommandBuffer()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }
    }
}
#endif