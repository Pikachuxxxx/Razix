#pragma once

#include "RZSTL/smart_pointers.h"

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
            void  Flip() override;
            void  OnResize(u32 width, u32 height) override;
            void* GetAPIHandle() override { return &m_Swapchain; }

            u32                         acquireBackBuffer();
            void                        present();
            void                        clearWithColor(ID3D12GraphicsCommandList2* commandList, glm::vec4 color);
            void                        prepareAsRenderTarget(ID3D12GraphicsCommandList2* commandList);
            D3D12_CPU_DESCRIPTOR_HANDLE getCurrentBackBufferRTVHandle();
            ID3D12Resource*             getCurrentD3DBackbufferResource() { return m_SwapchainD3DHandles[m_AcquiredBackBufferImageIndex]; }

            RZTextureHandle GetImage(u32 index) override { return m_SwapchainImageTextures[index]; }
            RZTextureHandle GetCurrentImage() override { return m_SwapchainImageTextures[m_AcquiredBackBufferImageIndex]; }
            sz              GetSwapchainImageCount() override { return m_SwapchainImageCount; }

        private:
            IDXGISwapChain4*             m_Swapchain                                        = nullptr;                     /* Handle to DXGI swapchain */
            u32                          m_SwapchainImageCount                              = 0;                           /* Total number of swapchain images being used  */
            std::vector<RZTextureHandle> m_SwapchainImageTextures                           = {};                          /* Swapchain images stored as engine 2D texture */
            ID3D12Resource*              m_SwapchainD3DHandles[RAZIX_MAX_SWAP_IMAGES_COUNT] = {};                          /* Swapchain images stored as d3d resources */
            HWND                         m_HWNDHandle                                       = NULL;                        /* Windows Handle */
            ID3D12DescriptorHeap*        m_SwapchainRTVHeap                                 = nullptr;                     /* Descriptor Heap from which the swapchain back buffers are allocated from */
            u32                          m_RTVDescriptorSize                                = 0;                           /* Size of the descriptor in the heap */
            const u32                    m_BackbuffersCount                                 = RAZIX_MAX_SWAP_IMAGES_COUNT; /* Number of swapchain back buffers  */
        };

    }    // namespace Gfx
}    // namespace Razix
#endif