#pragma once

#include "Razix/Graphics/API/RZSwapchain.h"

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

            void Init() override;
            void Destroy() override;
            void Flip() override;

            RZTexture* GetImage(uint32_t index) override;
            RZTexture* GetCurrentImage() override;
            size_t GetSwapchainImageCount() override;
            RZCommandBuffer* GetCurrentCommandBuffer() override;

            inline Microsoft::WRL::ComPtr<IDXGISwapChain>& getSwapchain() { return m_Swapchain; }
            inline Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& getRenderTarget() { return m_Target; }
        private:
            /* DirectX handle to swapchain */
            Microsoft::WRL::ComPtr<IDXGISwapChain>          m_Swapchain;
            /* The render target to which the swapchain presents/renders to */
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_Target;

        };
    }
}

#endif