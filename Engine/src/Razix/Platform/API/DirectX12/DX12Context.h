#pragma once

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Swapchain.h"

    #include <d3d12.h>
    #include <dxgi1_6.h>

    // D3D Memory allocator - VMA equivalent
    #include <D3D12MemAlloc.h>

    #ifdef RAZIX_DEBUG
        #include <d3d12sdklayers.h>
        #include <dxgidebug.h>
    #endif

namespace Razix {
    namespace Gfx {

        // Use the Windows Advanced Rasterization Platform (WARP) for device creation.
        static constexpr bool g_UseWarp = false;    // Preferable with older GPUs

        class DX12Context : public RZGraphicsContext
        {
        public:
            DX12Context(RZWindow* windowHandle);
            ~DX12Context() {}

            void Init() override;
            void Destroy() override;
            void ClearWithColor(f32 r, f32 g, f32 b) override;

            void Wait() override {}

            static DX12Context* Get() { return static_cast<DX12Context*>(s_Context); }

            rzstl::UniqueRef<DX12Swapchain>& getSwapchain() { return m_Swapchain; }

            RAZIX_INLINE ID3D12CommandQueue* getGraphicsQueue() { return m_GraphicsQueue; }
            RAZIX_INLINE ID3D12CommandQueue* getCopyQueue() { return m_CopyQueue; }
            RAZIX_INLINE ID3D12CommandQueue* getSingleTimeGraphicsQueue() { return m_SingleTimeGraphicsQueue; }
            /*  Returns a const pointer to the window handle that the context renders to */
            RAZIX_INLINE const RZWindow* getWindow() const { return m_Window; }
            /* Gets the handle to the device */
            RAZIX_INLINE ID3D12Device10* getDevice() { return m_Device; }

        private:
            RZWindow*                         m_Window = nullptr;      /* The Window handle                 */
            ID3D12Device10*                   m_Device = nullptr;      /* D3D12 handle to the GPU device    */
            D3D12_FEATURE_DATA_D3D12_OPTIONS1 m_WaveIntrinsicsSupport; /* Device Features: Wave Intrinsics */
    #ifdef RAZIX_DEBUG
            ID3D12Debug6* m_D3D12Debug = nullptr; /* D3D12 error handle    */
            IDXGIDebug1*  m_DXGIDebug  = nullptr; /* DXGI debugging handle */
            // TODO: Replace with ID3D12InfoQueue1 and add callback function similar to Vulkan
            // https://microsoft.github.io/DirectX-Specs/d3d/MessageCallback.html
            ID3D12InfoQueue* m_DebugValidation = nullptr; /* Debug validation to break on severity and filter messages  */
    #endif
            ID3D12CommandQueue*             m_GraphicsQueue;           /* GPU queue to submit draw/graphics related command lists */
            ID3D12CommandQueue*             m_CopyQueue;               /* GPU queue to submit copy command lists, for temporary and persistent resource copies */
            ID3D12CommandQueue*             m_SingleTimeGraphicsQueue; /* Graphics queue to submit single time command lists, for copy/single frame operations etc */
            rzstl::UniqueRef<DX12Swapchain> m_Swapchain;               /* Handle to the Razix-DX12 swapchain abstraction     */
            ID3D12DescriptorHeap*           m_SamplerHeap = nullptr;
        };
    }    // namespace Gfx
}    // namespace Razix

#endif