#pragma once

#include "Razix/Core/RZSTL/smart_pointers.h"

#include "Razix/Gfx/RHI/API/RZSwapchain.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/RHI.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>
    #include <dxgi1_5.h>

    #include "Razix/Platform/API/DirectX12/DX12Fence.h"

namespace Razix {
    namespace Gfx {

        // VSYNC = OFF => To achieve maximum frame rates while rendering with vsync-off, the DXGI_SWAP_EFFECT_FLIP_DISCARD flip model should be used.
        // VSYNC = ON  => When using the DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL presentation model, the DXGI runtime will place the presented frame at the end of the presentation queue.

        class DX12Swapchain : public RZSwapchain
        {
        public:
            DX12Swapchain(u32 width, u32 height);
            ~DX12Swapchain() {}

            void  Init(u32 width, u32 height) override;
            void  Destroy() override;
            void  DestroyBackBufferImages() override;
            void  OnResize(u32 width, u32 height) override;
            void* GetAPIHandle() override { return &m_Swapchain; }

            u32                         acquireBackBuffer();
            void                        present();
            void                        clearWithColor(ID3D12GraphicsCommandList2* commandList, float4 color);
            void                        prepareAsRenderTarget(ID3D12GraphicsCommandList2* commandList);
            ID3D12Resource*             getCurrentD3DBackbufferResource() { return m_SwapchainD3DHandles[m_AcquiredBackBufferImageIndex]; }
            D3D12_CPU_DESCRIPTOR_HANDLE getCurrentBackBufferRTVHandle();

            RZTextureHandle GetImage(u32 index) override { return m_SwapchainImageTextures[index]; }
            RZTextureHandle GetCurrentBackBufferImage() override { return m_SwapchainImageTextures[m_AcquiredBackBufferImageIndex]; }
            sz              GetSwapchainImageCount() override { return m_SwapchainImageCount; }

        private:
            IDXGISwapChain4*             m_Swapchain                                        = nullptr;
            u32                          m_SwapchainImageCount                              = 0;
            std::vector<RZTextureHandle> m_SwapchainImageTextures                           = {};
            ID3D12Resource*              m_SwapchainD3DHandles[RAZIX_MAX_SWAP_IMAGES_COUNT] = {};
            HWND                         m_HWNDHandle                                       = NULL;
            ID3D12DescriptorHeap*        m_SwapchainRTVHeap                                 = nullptr;
            u32                          m_RTVDescriptorSize                                = 0;
            const u32                    m_BackbuffersCount                                 = RAZIX_MAX_SWAP_IMAGES_COUNT;
        };

    }    // namespace Gfx
}    // namespace Razix
#endif