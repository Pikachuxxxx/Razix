// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Swapchain.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Context.h"
    #include "Razix/Platform/API/DirectX12/DX12Texture.h"
    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"

    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>

    #include <dxgi1_6.h>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
    #include <wrl.h>
using namespace Microsoft::WRL;

namespace Razix {
    namespace Gfx {

        //--------------------------------------------------------------------------------------

        static bool CheckTearingSupport()
        {
            BOOL allowTearing = FALSE;

            // Rather than create the DXGI 1.5 factory interface directly, we create the
            // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the
            // graphics debugging tools which will not support the 1.5 factory interface
            // until a future update.
            ComPtr<IDXGIFactory4> factory4;
            if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4)))) {
                ComPtr<IDXGIFactory5> factory5;
                if (SUCCEEDED(factory4.As(&factory5))) {
                    if (FAILED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
                        allowTearing = FALSE;
                }
            }

            return allowTearing == TRUE;
        }

        //--------------------------------------------------------------------------------------

        DX12Swapchain::DX12Swapchain(u32 width, u32 height)
        {
            m_Width  = width;
            m_Height = height;

            // Initialize the swapchain
            Init(m_Width, m_Height);

            m_SwapchainImageCount = RAZIX_MAX_SWAP_IMAGES_COUNT;
        }

        void DX12Swapchain::Init(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            IDXGIFactory4* dxgiFactory4       = nullptr;
            UINT           createFactoryFlags = 0;
    #if defined(RAZIX_DEBUG)
            createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
    #endif

            CHECK_HRESULT(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.Width                 = width;
            swapChainDesc.Height                = height;
            swapChainDesc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.Stereo                = FALSE;
            swapChainDesc.SampleDesc            = {1, 0};
            swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount           = RAZIX_MAX_SWAP_IMAGES_COUNT;
            swapChainDesc.Scaling               = DXGI_SCALING_STRETCH;
            swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.AlphaMode             = DXGI_ALPHA_MODE_UNSPECIFIED;
            // It is recommended to always allow tearing if tearing support is available.
            swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

            m_HWNDHandle = glfwGetWin32Window((GLFWwindow*) DX12Context::Get()->getWindow()->GetNativeWindow());

            IDXGISwapChain1* swapchain1            = nullptr;
            auto             pGraphicsCommandQueue = DX12Context::Get()->getGraphicsQueue();
            CHECK_HRESULT(dxgiFactory4->CreateSwapChainForHwnd((ID3D12CommandQueue*) pGraphicsCommandQueue, m_HWNDHandle, &swapChainDesc, nullptr, nullptr, &swapchain1));
            RAZIX_CORE_ASSERT(swapchain1, "[D3D12] Failed to create IDXGISwapChain1.");

            // Disable the Alt+Enter full screen toggle feature. Switching to full screen
            // will be handled manually.
            CHECK_HRESULT(dxgiFactory4->MakeWindowAssociation(m_HWNDHandle, DXGI_MWA_NO_ALT_ENTER));

            CHECK_HRESULT(swapchain1->QueryInterface(IID_PPV_ARGS(&m_Swapchain)));
            RAZIX_CORE_ASSERT(m_Swapchain, "[D3D12] Failed to create IDXGISwapChain4.");

            // Clean up intermediate resources.
            swapchain1->Release();
            swapchain1 = nullptr;

            dxgiFactory4->Release();
            dxgiFactory4 = nullptr;

            // Create the RTV descriptor heap to point to the swapchain resources in GPU heap
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors             = RAZIX_MAX_SWAP_IMAGES_COUNT;
            desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            CHECK_HRESULT(DX12Context::Get()->getDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_SwapchainRTVHeap)));
            m_RTVDescriptorSize = DX12Context::Get()->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            // Extract the back buffers from the swapchain
            // To iterate the descriptors in a descriptor heap, a handle to the first descriptor in the heap is retrieved
            D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = m_SwapchainRTVHeap->GetCPUDescriptorHandleForHeapStart();

            for (u32 i = 0; i < m_BackbuffersCount; ++i) {
                ID3D12Resource* backBuffer = nullptr;
                // Get the resources from swapchain itself, the Heap memory for these swapchain textures are managed internally
                // When we call CreateSwapChainForHwnd that will take care of it.
                // ResizeBuffers will take care of re-allocating new heap and destroying old stuff etc.
                CHECK_HRESULT(m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

                // A NULL description is used to create a default descriptor for the resource
                // In this case, the resource's internal description is used to create the RTV
                // Since the Resource is allocated from swapchain and not by user it has the required internal desc
                DX12Context::Get()->getDevice()->CreateRenderTargetView(backBuffer, nullptr, rtvStartHandle);

                rtvStartHandle.ptr += m_RTVDescriptorSize;

                // Create engine resource with this handle
                RZHandle<RZTexture> handle;
                void*               where = RZResourceManager::Get().getPool<RZTexture>().obtain(handle);

                new (where) DX12Texture(backBuffer);
                IRZResource<RZTexture>* resource = (IRZResource<RZTexture>*) where;
                resource->setHandle(handle);
                resource->setName(RZ_SWAP_IMAGE_RES_NAME);

                m_SwapchainImageTextures.push_back(handle);
                m_SwapchainD3DHandles[i] = backBuffer;

                D3D12_TAG_OBJECT(backBuffer, L"$RazixSwapchainBackBuffer$");
            }
        }

        void DX12Swapchain::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            D3D_SAFE_RELEASE(m_Swapchain)

            for (u32 i = 0; i < m_SwapchainImageCount; i++)
                D3D_SAFE_RELEASE(m_SwapchainD3DHandles[i]);

            D3D_SAFE_RELEASE(m_SwapchainRTVHeap)
        }

        void DX12Swapchain::DestroyBackBufferImages()
        {
            for (u32 i = 0; i < m_SwapchainImageCount; i++)
                D3D_SAFE_RELEASE(m_SwapchainD3DHandles[i]);

            D3D_SAFE_RELEASE(m_SwapchainRTVHeap)
        }

        void DX12Swapchain::OnResize(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_Width == width && m_Height == height)
                return;

            m_IsResized  = true;
            m_IsResizing = true;

            m_Width  = width;
            m_Height = height;

            Destroy();

            for (u32 i = 0; i < m_SwapchainImageCount; i++)
                D3D_SAFE_RELEASE(m_SwapchainD3DHandles[i]);

            m_SwapchainImageTextures.clear();

            m_Swapchain = VK_NULL_HANDLE;

            Init(width, height);
            m_IsResizing = false;
        }

        u32 DX12Swapchain::acquireBackBuffer()
        {
            m_AcquiredBackBufferImageIndex = m_Swapchain->GetCurrentBackBufferIndex();
            return m_AcquiredBackBufferImageIndex;
        }

        void DX12Swapchain::present()
        {
            u32 syncInterval = g_GraphicsFeatures.EnableVSync ? 1 : 0;
            u32 presentFlags = !g_GraphicsFeatures.EnableVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
            CHECK_HRESULT(m_Swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

            m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_SwapchainImageCount;
        }

        void DX12Swapchain::clearWithColor(ID3D12GraphicsCommandList2* commandList, glm::vec4 color)
        {
            DX12Utilities::TransitionResource(commandList, m_SwapchainD3DHandles[m_AcquiredBackBufferImageIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

            auto rtv = getCurrentBackBufferRTVHandle();
            commandList->ClearRenderTargetView(rtv, &color[0], 0, nullptr);
        }

        void DX12Swapchain::prepareAsRenderTarget(ID3D12GraphicsCommandList2* commandList)
        {
            DX12Utilities::TransitionResource(commandList, m_SwapchainD3DHandles[m_AcquiredBackBufferImageIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        }

        D3D12_CPU_DESCRIPTOR_HANDLE DX12Swapchain::getCurrentBackBufferRTVHandle()
        {
            D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_SwapchainRTVHeap->GetCPUDescriptorHandleForHeapStart();
            // The handle is offset from the beginning of the descriptor heap based on the current back buffer index and the size of the descriptor
            DX12Utilities::GetCPUDescriptorOffsetHandle(rtv, m_AcquiredBackBufferImageIndex, m_RTVDescriptorSize);
            return rtv;
        }

        //--------------------------------------------------------------------------------------

    }    // namespace Gfx
}    // namespace Razix
#endif