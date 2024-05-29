#pragma once

#include "RZSTL/smart_pointers.h"

#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/RHI.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>
    #include <dxgi1_5.h>

namespace Razix {
    namespace Graphics {

        // VSYNC = OFF => To achieve maximum frame rates while rendering with vsync-off, the DXGI_SWAP_EFFECT_FLIP_DISCARD flip model should be used.
        // VSYNC = ON  => When using the DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL presentation model, the DXGI runtime will place the presented frame at the end of the presentation queue.

        struct FrameSyncData_DX12
        {
            u32          fenceValue       = 0;
            HANDLE       renderFenceEvent = INVALID_HANDLE_VALUE;
            ID3D12Fence* renderFence      = nullptr;
        };

        class DX12Swapchain : public RZSwapchain
        {
        public:
            DX12Swapchain(u32 width, u32 height);
            ~DX12Swapchain() {}

            void  Init(u32 width, u32 height) override;
            void  Destroy() override;
            void  Flip() override;
            void  OnResize(u32 width, u32 height) override;
            void* GetAPIHandle() override { return &m_Swapchain; }

            RZTextureHandle GetImage(u32 index) override { return m_SwapchainImageTextures[index]; }
            RZTextureHandle GetCurrentImage() override { return m_SwapchainImageTextures[m_AcquiredBackBufferImageIndex]; }
            sz              GetSwapchainImageCount() override { return m_SwapchainImageCount; }

            const FrameSyncData_DX12& getCurrentFrameSyncDataVK() const
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                RAZIX_ASSERT(m_CurrentSubmittedBackBufferIndex < m_SwapchainImageCount, "[D3D12] Incorrect swapchain buffer index");
                return m_FramesSyncData[m_CurrentSubmittedBackBufferIndex];
            }

        private:
            IDXGISwapChain4*             m_Swapchain                                   = nullptr;
            u32                          m_SwapchainImageCount                         = 0;    /* Total number of swapchain images being used  */
            std::vector<RZTextureHandle> m_SwapchainImageTextures                      = {};   /* Swapchain images stored as engine 2D texture */
            FrameSyncData_DX12           m_FramesSyncData[RAZIX_MAX_SWAP_IMAGES_COUNT] = {};   /* Frame sync primitives                        */
            HWND                         m_HWNDHandle                                  = NULL; /* Windows Handle */

        private:
        };

    }    // namespace Graphics
}    // namespace Razix
#endif