// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Context.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/D3D12Utilities.h"

    #include <dxgi1_6.h>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
    #include <wrl.h>
using namespace Microsoft::WRL;

namespace Razix {
    namespace Graphics {

        // https://www.3dgep.com/learning-directx-12-1/
        //--------------------------------------------------------------------------------------
        static ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp)
        {
            ComPtr<IDXGIFactory4> dxgiFactory;
            UINT                  createFactoryFlags = 0;
    #if defined(RAZIX_DEBUG)
            createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
    #endif

            CHECK_HRESULT(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

            ComPtr<IDXGIAdapter1> dxgiAdapter1;
            ComPtr<IDXGIAdapter4> dxgiAdapter4;

            if (useWarp) {
                CHECK_HRESULT(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
                CHECK_HRESULT(dxgiAdapter1.As(&dxgiAdapter4));
            } else {
                SIZE_T maxDedicatedVideoMemory = 0;
                for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i) {
                    DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
                    dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

                    // Check to see if the adapter can create a D3D12 device without actually creating it.
                    // The adapter with the largest dedicated video memory and min feature level 12 is favored.
                    if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                        SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)) &&
                        dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory) {
                        maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                        CHECK_HRESULT(dxgiAdapter1.As(&dxgiAdapter4));
                    }
                }
            }

            return dxgiAdapter4;
        }

        //--------------------------------------------------------------------------------------

        DX12Context::DX12Context(RZWindow* windowHandle)
            : m_Window(windowHandle)
        {
            RAZIX_CORE_ASSERT(windowHandle, "[D3D12] Window Handle is NULL!");
        }

        void DX12Context::Init()
        {
    #if RAZIX_DEBUG

            // Init the Debug layer stuff
            if (CHECK_HRESULT(D3D12GetDebugInterface(IID_PPV_ARGS(&m_D3D12Debug)))) {
                m_D3D12Debug->EnableDebugLayer();

                // Init DXGI debug
                if (CHECK_HRESULT(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_DXGIDebug)))) {
                    m_DXGIDebug->EnableLeakTrackingForThread();
                }

            } else
                RAZIX_CORE_WARN("[D3D12] D3D12 Debug interface not available! Debug Layer will be disabled.");
    #endif

            // Get all the GPUs and choose the Discrete one + DirectX >= 12.2.
            D3D12CreateDevice(GetAdapter(g_UseWarp).Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_Device));

    #if RAZIX_DEBUG
            // Create validation layer (Message filtering and severity breakpoint, similar to VK)
            if (CHECK_HRESULT(m_Device->QueryInterface(IID_PPV_ARGS(&m_DebugValidation)))) {
                m_DebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
                m_DebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
                m_DebugValidation->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

                // Suppress whole categories of messages
                //D3D12_MESSAGE_CATEGORY Categories[] = {};

                // Suppress messages based on their severity level
                // Since D3D12_MESSAGE_SEVERITY_INFO message severity is for information only, info messages are suppressed.
                D3D12_MESSAGE_SEVERITY Severities[] = {
                    D3D12_MESSAGE_SEVERITY_INFO,
                };

                // Suppress individual messages by their ID
                D3D12_MESSAGE_ID DenyIds[] = {
                    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,    // I'm really not sure how to avoid this message.
                    D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                          // This warning occurs when using capture frame while graphics debugging.
                    D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                        // This warning occurs when using capture frame while graphics debugging.
                };

                D3D12_INFO_QUEUE_FILTER NewFilter = {};
                //NewFilter.DenyList.NumCategories = _countof(Categories);
                //NewFilter.DenyList.pCategoryList = Categories;
                NewFilter.DenyList.NumSeverities = _countof(Severities);
                NewFilter.DenyList.pSeverityList = Severities;
                NewFilter.DenyList.NumIDs        = _countof(DenyIds);
                NewFilter.DenyList.pIDList       = DenyIds;

                CHECK_HRESULT(m_DebugValidation->PushStorageFilter(&NewFilter));
            } else
                RAZIX_CORE_WARN("[D3D12] Debug Validation is disabled!");
    #endif

            // Create the Graphics Command Queue
            D3D12_COMMAND_QUEUE_DESC graphicsCommandQueueDesc = {};
            graphicsCommandQueueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;    // Direct = Graphics
            graphicsCommandQueueDesc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            graphicsCommandQueueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
            graphicsCommandQueueDesc.NodeMask                 = 0;

            CHECK_HRESULT(m_Device->CreateCommandQueue(&graphicsCommandQueueDesc, IID_PPV_ARGS(&m_GraphicsQueue)));

            if (g_GraphicsFeaturesSettings.EnableVSync) {
            }

            // Create the swapchain
            m_Swapchain = rzstl::CreateUniqueRef<DX12Swapchain>(m_Window->getWidth(), m_Window->getHeight());
        }

        void DX12Context::Destroy()
        {
    #if RAZIX_DEBUG
            if (m_DXGIDebug) {
                RAZIX_CORE_WARN("[D3D12] Reporting Live Objects...");
                OutputDebugStringW(L"[Razix] [D3D12] Reporting Live Objects...");
                m_DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
            }

            if (m_DXGIDebug)
                delete m_DXGIDebug;
            if (m_D3D12Debug)
                delete m_D3D12Debug;
            m_DXGIDebug  = nullptr;
            m_D3D12Debug = nullptr;

            if (m_DebugValidation)
                m_DebugValidation->Release();
    #endif

            // Destroy the device
            m_Device->Release();
        }

        void DX12Context::ClearWithColor(f32 r, f32 g, f32 b)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }    // namespace Graphics
}    // namespace Razix
#endif