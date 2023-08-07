#pragma once

#include "Razix/Graphics/RHI/API/RZSwapchain.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

    #include <d3d11.h>
    #include <wrl.h>

namespace Razix {
    namespace Graphics {
        class DX11Swapchain : public RZSwapchain
        {
        public:
            DX11Swapchain() {}
            ~DX11Swapchain() {}

            void  Init(u32 width, u32 height) override;
            void  Destroy() override;
            void  Flip() override;
            void  OnResize(u32 width, u32 height) override {}
            void* GetAPIHandle() { return m_Swapchain.Get(); }

            RZTextureHandle GetImage(u32 index) override;
            RZTextureHandle GetCurrentImage() override;
            sz              GetSwapchainImageCount() override;

            inline Microsoft::WRL::ComPtr<IDXGISwapChain>&         getSwapchain() { return m_Swapchain; }
            inline Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& getRenderTarget() { return m_Target; }

            u32 getCurrentImageIndex() override;

        private:
            /* DirectX handle to swapchain */
            Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain;
            /* The render target to which the swapchain presents/renders to */
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_Target;
        };
    }    // namespace Graphics
}    // namespace Razix

#endif