// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Context.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
    #include <wrl.h>
using namespace Microsoft::WRL;

namespace Razix {
    namespace Gfx {

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

        std::string DriverVersionToString(LARGE_INTEGER driverVersion)
        {
            char driverVersionString[128];
            snprintf(driverVersionString, sizeof(driverVersionString), "%u.%u.%u.%u", HIWORD(driverVersion.HighPart), LOWORD(driverVersion.HighPart), HIWORD(driverVersion.LowPart), LOWORD(driverVersion.LowPart));
            return std::string(driverVersionString);
        }

        std::string WCharToString(const WCHAR* wcharStr)
        {
            int         bufferSize = WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, nullptr, 0, nullptr, nullptr);
            std::string str(bufferSize, 0);
            WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, &str[0], bufferSize, nullptr, nullptr);
            return str;
        }

        //--------------------------------------------------------------------------------------

        DX12Context::DX12Context(RZWindow* windowHandle)
            : m_Window(windowHandle)
        {
            RAZIX_CORE_ASSERT(windowHandle, "[D3D12] Window Handle is NULL!");
        }

        void DX12Context::Init()
        {
            //https://learn.microsoft.com/bs-latn-ba/windows/win32/api/dxgi1_6/nf-dxgi1_6-dxgideclareadapterremovalsupport
            // helps with adapter lost events
            DXGIDeclareAdapterRemovalSupport();

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
            auto m_PhysicalGPUAdapter = GetAdapter(g_UseWarp);
            D3D12CreateDevice(GetAdapter(g_UseWarp).Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_Device));

            // Device Features and Support
            {
                // Checks for SM6 and WaveIntrinsics
                // query shader model level, we ask for min of this level
                D3D12_FEATURE_DATA_SHADER_MODEL queryShaderModel = {D3D_SHADER_MODEL_6_0};
                CHECK_HRESULT(m_Device->CheckFeatureSupport((D3D12_FEATURE) D3D12_FEATURE_SHADER_MODEL, &queryShaderModel, sizeof(queryShaderModel)));
                // Query the level of support of Wave Intrinsics
                // https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_feature_data_d3d12_options1
                CHECK_HRESULT(m_Device->CheckFeatureSupport((D3D12_FEATURE) D3D12_FEATURE_D3D12_OPTIONS1, &m_WaveIntrinsicsSupport, sizeof(m_WaveIntrinsicsSupport)));

                if (queryShaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_0)
                    g_GraphicsFeatures.SupportsShaderModel6 = true;
                else
                    RAZIX_CORE_ERROR("[D3D12] Doesn't support Shader Model 6.0, some features will not work as expected and engine may crash!");
                if (m_WaveIntrinsicsSupport.WaveOps == 1) {
                    g_GraphicsFeatures.SupportsWaveIntrinsics = true;
                    g_GraphicsFeatures.MinLaneWidth           = m_WaveIntrinsicsSupport.WaveLaneCountMin;
                    g_GraphicsFeatures.MaxLaneWidth           = m_WaveIntrinsicsSupport.WaveLaneCountMax;
                } else
                    RAZIX_CORE_ERROR("[D3D12] Doesn't support Wave Instrinsics, some features will not work as expected and engine may crash!");
            }

            // Print the GPU adapter details
            DXGI_ADAPTER_DESC1 desc;
            m_PhysicalGPUAdapter->GetDesc1(&desc);

            RAZIX_CORE_INFO("[D3D12] GPU Name: {0}", WCharToString(desc.Description));
            RAZIX_CORE_INFO("[D3D12] Vendor ID: {0}", desc.VendorId);
            RAZIX_CORE_INFO("[D3D12] Device ID: {0}", desc.DeviceId);
            RAZIX_CORE_INFO("[D3D12] Subsystem ID: {0}", desc.SubSysId);
            RAZIX_CORE_INFO("[D3D12] Revision: {0}", desc.Revision);
            RAZIX_CORE_INFO("[D3D12] Dedicated Video Memory: {0}", desc.DedicatedVideoMemory);
            RAZIX_CORE_INFO("[D3D12] Dedicated System Memory: {0}", desc.DedicatedSystemMemory);
            RAZIX_CORE_INFO("[D3D12] Shared System Memory: {0}", desc.SharedSystemMemory);
            LARGE_INTEGER driverVersion;
            if (CHECK_HRESULT(m_PhysicalGPUAdapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &driverVersion))) {
                std::string driverVersionStr = DriverVersionToString(driverVersion);
                RAZIX_CORE_INFO("[D3D12] Driver Version: {0}", driverVersionStr);
            }
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
            D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
            CommandQueueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;    // Direct = Graphics
            CommandQueueDesc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            CommandQueueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
            CommandQueueDesc.NodeMask                 = 0;

            CHECK_HRESULT(m_Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&m_GraphicsQueue)));
            D3D12_TAG_OBJECT(m_GraphicsQueue, L"Graphics Queue");

            CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            CHECK_HRESULT(m_Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&m_CopyQueue)));
            D3D12_TAG_OBJECT(m_CopyQueue, L"Copy Queue");

            CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            CHECK_HRESULT(m_Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&m_SingleTimeGraphicsQueue)));
            D3D12_TAG_OBJECT(m_SingleTimeGraphicsQueue, L"Single TIme Graphics Queue");

            if (g_GraphicsFeatures.EnableVSync) {
            }

            // Create the swapchain
            m_Swapchain = rzstl::CreateUniqueRef<DX12Swapchain>(m_Window->getWidth(), m_Window->getHeight());

            D3D12_TAG_OBJECT(m_Device, L"Device");
        }

        void DX12Context::Destroy()
        {
    #if RAZIX_DEBUG
            if (m_DXGIDebug) {
                RAZIX_CORE_WARN("[D3D12] Reporting Live Objects...");
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

    }    // namespace Gfx
}    // namespace Razix
#endif