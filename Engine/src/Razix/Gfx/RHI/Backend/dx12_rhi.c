#include "dx12_rhi.h"

#include "Razix/Gfx/RHI/RHI.h"

// TYpe friendly defines
#define DX12Context g_GfxCtx.dx12
#define DX12Device  g_GfxCtx.dx12.device9
#define CHECK_HR(x) dx12_util_check_hresult((x), __func__, __FILE__, __LINE__)

//---------------------------------------------------------------------------------------------
// Internal types

typedef struct
{
    HRESULT     code;
    const char* description;
} HRESULTDescriptionEntry;

static HRESULTDescriptionEntry hresult_errors[] = {
    {S_OK, "Operation successful"},
    {E_ABORT, "Operation aborted"},
    {E_ACCESSDENIED, "General access denied error"},
    {E_FAIL, "Unspecified failure"},
    {E_HANDLE, "Handle that is not valid"},
    {E_INVALIDARG, "One or more arguments are not valid"},
    {E_NOINTERFACE, "No such interface supported"},
    {E_NOTIMPL, "Not implemented"},
    {E_OUTOFMEMORY, "Failed to allocate necessary memory"},
    {E_POINTER, "Pointer that is not valid"},
    {E_UNEXPECTED, "Unexpected failure"},
};

//---------------------------------------------------------------------------------------------
// Util functions

static const char* dx12_util_hresult_to_string(HRESULT hr)
{
    size_t count = sizeof(hresult_errors) / sizeof(hresult_errors[0]);
    for (size_t i = 0; i < count; ++i) {
        if (hresult_errors[i].code == hr)
            return hresult_errors[i].description;
    }
    return "Unknown HRESULT error";
}

static bool dx12_util_check_hresult(HRESULT hr, const char* func, const char* file, int line)
{
    if (hr != S_OK) {
        const char* desc = dx12_util_hresult_to_string(hr);
        RAZIX_RHI_LOG_ERROR("[D3D12] HRESULT Error :: %s\n -> In function %s (%s:%d)\n", desc, func, file, line);
        return false;
    }
    return true;
}

static LPCWSTR dx12_util_string_to_lpcwstr(const char* input)
{
    if (!input) return NULL;

    int wide_len = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
    if (wide_len == 0) return NULL;

    wchar_t* wide_str = (wchar_t*) malloc(wide_len * sizeof(wchar_t));
    if (!wide_str) return NULL;

    if (MultiByteToWideChar(CP_UTF8, 0, input, -1, wide_str, wide_len) == 0) {
        free(wide_str);
        return NULL;
    }
    return wide_str;
}

static DXGI_FORMAT dx12_util_rz_gfx_format_to_dxgi_format(rz_gfx_format format)
{
    switch (format) {
        // 8-bit per channel formats
        case RZ_GFX_FORMAT_R8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case RZ_GFX_FORMAT_R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
        case RZ_GFX_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case RZ_GFX_FORMAT_B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM;

        // sRGB formats
        case RZ_GFX_FORMAT_R8G8B8A8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case RZ_GFX_FORMAT_B8G8R8A8_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

        // 16-bit float formats
        case RZ_GFX_FORMAT_R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
        case RZ_GFX_FORMAT_R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
        case RZ_GFX_FORMAT_R16G16B16A16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;

        // 32-bit float formats
        case RZ_GFX_FORMAT_R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
        case RZ_GFX_FORMAT_R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
        case RZ_GFX_FORMAT_R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
        case RZ_GFX_FORMAT_R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;

        // Depth-Stencil formats
        case RZ_GFX_FORMAT_D16_UNORM: return DXGI_FORMAT_D16_UNORM;
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case RZ_GFX_FORMAT_D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

        // Compressed formats
        case RZ_GFX_FORMAT_BC1_RGBA_UNORM: return DXGI_FORMAT_BC1_UNORM;
        case RZ_GFX_FORMAT_BC3_RGBA_UNORM: return DXGI_FORMAT_BC3_UNORM;
        case RZ_GFX_FORMAT_BC7_UNORM: return DXGI_FORMAT_BC7_UNORM;
        case RZ_GFX_FORMAT_BC7_SRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;

        // Default fallback
        case RZ_GFX_FORMAT_UNDEFINED:
        default: return DXGI_FORMAT_UNKNOWN;
    }
}

//---------------------------------------------------------------------------------------------
// Helper functions

static IDXGIAdapter4* dx12_select_best_adapter(IDXGIFactory7* factory, D3D_FEATURE_LEVEL min_feat_level)
{
    // We prefer the first one if we cannot find any Discrete GPUs
    IDXGIAdapter4* best_adapter     = NULL;
    size_t         maxDedicatedVRAM = 0;
    for (uint32_t i = 0;; ++i) {
        // DXGI factory returns IDXGIAdapter1* via EnumAdapters1.
        // To access modern features (like GetDesc3), we must QueryInterface
        // the IDXGIAdapter1 to IDXGIAdapter4, which exposes DXGI_ADAPTER_DESC3.
        // We use that to inspect the adapter and decide if it's suitable.
        IDXGIAdapter1* adapter1 = NULL;

        HRESULT hr = IDXGIFactory7_EnumAdapters1(factory, i, &adapter1);
        if (hr == DXGI_ERROR_NOT_FOUND)
            break;
        if (FAILED(hr))
            continue;

        IDXGIAdapter4* adapter4 = NULL;
        hr                      = IDXGIAdapter1_QueryInterface(adapter1, &IID_IDXGIAdapter4, &adapter4);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[D3D12] [DX12] Failed to query IDXGIAdapter4 from IDXGIAdapter1 (HRESULT = 0x%08X)", (unsigned int) hr);
            IDXGIAdapter1_Release(adapter1);
            continue;
        }
        IDXGIAdapter1_Release(adapter1);

        DXGI_ADAPTER_DESC3 desc = {0};
        hr                      = IDXGIAdapter4_GetDesc3(adapter4, &desc);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[D3D12] [DX12] Failed to get DXGI_ADAPTER_DESC3 from IDXGIAdapter4 (HRESULT = 0x%08X)", (unsigned int) hr);
            IDXGIAdapter4_Release(adapter4);
            continue;
        }

        if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
            IDXGIAdapter4_Release(adapter4);
            continue;
        }

        if (desc.DedicatedVideoMemory > maxDedicatedVRAM) {
            if (best_adapter)
                IDXGIAdapter4_Release(best_adapter);

            maxDedicatedVRAM = desc.DedicatedVideoMemory;
            best_adapter     = adapter4;
            break;
        }

        if (!best_adapter) {
            best_adapter = adapter4;
        } else {
            IDXGIAdapter4_Release(adapter4);
        }
    }

    if (!best_adapter) {
        RAZIX_RHI_LOG_ERROR("[D3D12] [DX12] No suitable GPU found for D3D12");
        return NULL;
    } else {
        RAZIX_RHI_LOG_INFO("Selected Adapter Info: ");
        DXGI_ADAPTER_DESC3 adapterDesc = {0};
        IDXGIAdapter4_GetDesc3(best_adapter, &adapterDesc);
        RAZIX_RHI_LOG_INFO("\t -> Name                    : %ls", adapterDesc.Description);
        RAZIX_RHI_LOG_INFO("\t -> VendorID                : %u", adapterDesc.VendorId);
        RAZIX_RHI_LOG_INFO("\t -> DeviceId                : %u", adapterDesc.DeviceId);
        RAZIX_RHI_LOG_INFO("\t -> SubSysId                : %u", adapterDesc.SubSysId);
        RAZIX_RHI_LOG_INFO("\t -> Revision                : %u", adapterDesc.Revision);
        RAZIX_RHI_LOG_INFO("\t -> DedicatedVideoMemory    : %zu", adapterDesc.DedicatedVideoMemory);
        RAZIX_RHI_LOG_INFO("\t -> DedicatedSystemMemory   : %zu", adapterDesc.DedicatedSystemMemory);
        RAZIX_RHI_LOG_INFO("\t -> SharedSystemMemory      : %zu", adapterDesc.SharedSystemMemory);
        return best_adapter;
    }
}

static void dx12_query_features(dx12_ctx* ctx)
{
    ID3D12Device9*     device = ctx->device9;
    D3D12FeatureCache* f      = &ctx->features;

    ID3D12Device9_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS, &f->options, sizeof(f->options));
    ID3D12Device9_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS1, &f->options1, sizeof(f->options1));
    ID3D12Device9_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS5, &f->options5, sizeof(f->options5));

    f->architecture.NodeIndex = 0;
    ID3D12Device9_CheckFeatureSupport(device, D3D12_FEATURE_ARCHITECTURE1, &f->architecture, sizeof(f->architecture));
    f->isUMA              = f->architecture.UMA;
    f->isCacheCoherentUMA = f->architecture.CacheCoherentUMA;

    f->shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_7;
    if (FAILED(ID3D12Device9_CheckFeatureSupport(device, D3D12_FEATURE_SHADER_MODEL, &f->shaderModel, sizeof(f->shaderModel)))) {
        f->shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;
    }

    f->rootSig.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(ID3D12Device9_CheckFeatureSupport(device, D3D12_FEATURE_ROOT_SIGNATURE, &f->rootSig, sizeof(f->rootSig)))) {
        f->rootSig.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    ID3D12Device9_CheckFeatureSupport(device, D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &f->vaSupport, sizeof(f->vaSupport));

    f->nodeCount = ID3D12Device9_GetNodeCount(device);
}

static void dx12_print_features(const D3D12FeatureCache* f)
{
    RAZIX_RHI_LOG_INFO("============ D3D12 Device Feature Info ============");

    RAZIX_RHI_LOG_INFO("Shader Model          : %u.%u",
        (f->shaderModel.HighestShaderModel >> 4) & 0xF,
        f->shaderModel.HighestShaderModel & 0xF);

    RAZIX_RHI_LOG_INFO("Root Signature        : v1.%d",
        f->rootSig.HighestVersion == D3D_ROOT_SIGNATURE_VERSION_1_1 ? 1 : 0);

    RAZIX_RHI_LOG_INFO("GPU Nodes             : %u", f->nodeCount);
    RAZIX_RHI_LOG_INFO("UMA                   : %s", f->isUMA ? "Yes" : "No");
    RAZIX_RHI_LOG_INFO("Cache-Coherent UMA    : %s", f->isCacheCoherentUMA ? "Yes" : "No");

    // D3D12_OPTIONS
    RAZIX_RHI_LOG_INFO("Conservative Raster   : %s", f->options.ConservativeRasterizationTier != D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED ? "Yes" : "No");
    RAZIX_RHI_LOG_INFO("Standard Swizzle      : %s", f->options.StandardSwizzle64KBSupported ? "Yes" : "No");
    RAZIX_RHI_LOG_INFO("Typed UAV Load ADDR64 : %s", f->options.TypedUAVLoadAdditionalFormats ? "Yes" : "No");

    // D3D12_OPTIONS1
    RAZIX_RHI_LOG_INFO("Wave Ops              : %s", f->options1.WaveOps ? "Yes" : "No");
    RAZIX_RHI_LOG_INFO("Wave Lane Count       : Min %u / Max %u", f->options1.WaveLaneCountMin, f->options1.WaveLaneCountMax);
    RAZIX_RHI_LOG_INFO("Int64 Shader Ops      : %s", f->options1.Int64ShaderOps ? "Yes" : "No");

    // D3D12_OPTIONS5
    RAZIX_RHI_LOG_INFO("Raytracing Tier       : Tier %d", f->options5.RaytracingTier);

    // VA support
    RAZIX_RHI_LOG_INFO("VA 64-bit Support     : %s", f->vaSupport.MaxGPUVirtualAddressBitsPerResource >= 44 ? "Yes" : "Partial/No");

    RAZIX_RHI_LOG_INFO("===================================================");
}

#ifdef _DEBUG
// Before Device
static void dx12_register_debug_interface(dx12_ctx* ctx)
{
    if (SUCCEEDED(D3D12GetDebugInterface(&IID_ID3D12Debug3, (void**) &ctx->d3dDebug3))) {
        ID3D12Debug3_EnableDebugLayer(ctx->d3dDebug3);
        // THIS IS SLOW AF! DISABLE IT ON NVIDIA GPUs
        //ID3D12Debug3_SetEnableGPUBasedValidation(backend->d3d12_debug, TRUE);
        RAZIX_RHI_LOG_INFO("D3D12 debug layer and GPU-based validation enabled");
    } else {
        RAZIX_RHI_LOG_WARN("D3D12 debug interface not available. Debug layer not enabled.");
    }
}

// After Device
static void dx12_d3d12_register_info_queue(dx12_ctx* ctx)
{
    if (!ctx->device9) {
        RAZIX_RHI_LOG_ERROR("[D3D12] D3D12 device is NULL; can't register info queue.");
        return;
    }

    // This increases the device refcount.
    if (CHECK_HR(ID3D12InfoQueue_QueryInterface(ctx->device9, &IID_ID3D12InfoQueue, (void**) &ctx->d3dInfoQ))) {
        ID3D12InfoQueue* q = ctx->d3dInfoQ;

        ID3D12InfoQueue_SetBreakOnSeverity(q, D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        ID3D12InfoQueue_SetBreakOnSeverity(q, D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);

        RAZIX_RHI_LOG_INFO("D3D12 info queue registered and debug message filters installed");
    } else {
        RAZIX_RHI_LOG_WARN("Failed to query ID3D12InfoQueue interface. Validation messages will not be captured.");
    }
}

static void dx12_dxgi_register_info_queue(dx12_ctx* ctx)
{
    if (SUCCEEDED(DXGIGetDebugInterface1(0, &IID_IDXGIInfoQueue, (void**) &ctx->dxgiInfoQ))) {
        IDXGIInfoQueue* q = ctx->dxgiInfoQ;

        IDXGIInfoQueue_SetBreakOnSeverity(q, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
        IDXGIInfoQueue_SetBreakOnSeverity(q, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);

        RAZIX_RHI_LOG_INFO("DXGI debug info queue registered");
    } else {
        RAZIX_RHI_LOG_WARN("DXGI info queue not available. No message filtering.");
    }
}

static void dx12_track_dxgi_liveobjects(dx12_ctx* ctx)
{
    RAZIX_RHI_LOG_WARN("Tracking live DXGI objects. This will report all live objects at the end of the program.");
    if (SUCCEEDED(DXGIGetDebugInterface1(0, &IID_IDXGIDebug, (void**) &ctx->dxgiDebug))) {
        IDXGIDebug_ReportLiveObjects(
            ctx->dxgiDebug,
            DXGI_DEBUG_ALL,
            (DXGI_DEBUG_RLO_FLAGS) (DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        RAZIX_RHI_LOG_INFO("DXGI live object report completed");
        IDXGIDebug_Release(ctx->dxgiDebug);
    }
}

static void dx12_destroy_debug_handles(dx12_ctx* ctx)
{
    if (ctx->d3dDebug3) {
        ID3D12Debug3_Release(ctx->d3dDebug3);
        ctx->d3dDebug3 = NULL;
    }
    if (ctx->d3dInfoQ) {
        ID3D12InfoQueue_Release(ctx->d3dInfoQ);
        ctx->d3dInfoQ = NULL;
    }
    if (ctx->dxgiInfoQ) {
        IDXGIInfoQueue_Release(ctx->dxgiInfoQ);
        ctx->dxgiInfoQ = NULL;
    }
    // TODO: Done after device destroy
    if (ctx->dxgiDebug) {
        IDXGIDebug_Release(ctx->dxgiDebug);
        ctx->dxgiDebug = NULL;
    }
}
#endif

static void dx12_update_swapchain_rtvs(rz_gfx_swapchain* sc)
{
    sc->imageCount = RAZIX_MAX_SWAP_IMAGES_COUNT;
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(sc->dx12.rtvHeap, &sc->dx12.rtvHeapStart.cpu);
    ID3D12DescriptorHeap_GetGPUDescriptorHandleForHeapStart(sc->dx12.rtvHeap, &sc->dx12.rtvHeapStart.gpu);

    for (uint32_t i = 0; i < sc->imageCount; ++i) {
        ID3D12Resource* d3dresource = NULL;
        HRESULT         hr          = IDXGISwapChain4_GetBuffer(sc->dx12.swapchain4, i, &IID_ID3D12Resource, (void**) &d3dresource);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[D3D12] Failed to get backbuffer %u from swapchain (HRESULT = 0x%08X)", i, (unsigned int) hr);

            for (uint32_t j = 0; j < i; ++j) {
                if (sc->backbuffers[j].dx12.resource) {
                    ID3D12Resource_Release(sc->backbuffers[j].dx12.resource);
                    sc->backbuffers[j].dx12.resource = NULL;
                }
            }

            ID3D12DescriptorHeap_Release(sc->dx12.rtvHeap);
            IDXGISwapChain4_Release(sc->dx12.swapchain4);
            memset(sc, 0, sizeof(dx12_swapchain));
            return;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = sc->dx12.rtvHeapStart.cpu;
        rtvHandle.ptr += (size_t) i * ID3D12Device9_GetDescriptorHandleIncrementSize(DX12Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        ID3D12Device9_CreateRenderTargetView(DX12Device, d3dresource, NULL, rtvHandle);
        ID3D12Resource_SetName(d3dresource, dx12_util_string_to_lpcwstr("Swapchain Image"));

        // This is the only place where a RZ_RESOURCE is manually created, instead of using the RZResourceManager
        rz_gfx_texture texture     = {0};
        dx12_texture   dxtexture   = {0};
        texture.resource.name      = "$SWAPCHAIN_IMAGE$";
        texture.resource.handle    = (rz_handle){i, i};
        texture.resource.viewHints = RZ_GFX_RESOURCE_VIEW_FLAG_RTV;
        dxtexture.resource         = d3dresource;
        dxtexture.state            = D3D12_RESOURCE_STATE_PRESENT;
        dxtexture.resView.rtv.cpu  = rtvHandle;
        texture.dx12               = dxtexture;
        texture.height             = sc->height;
        texture.width              = sc->width;
        texture.arraySize          = 1;
        texture.mipLevels          = 1;
        texture.format             = RAZIX_SWAPCHAIN_FORMAT;
        texture.textureType        = RZ_GFX_TEXTURE_TYPE_2D;
        sc->backbuffers[i]         = texture;
    }
}

static void dx12_create_backbuffers(rz_gfx_swapchain* sc)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {0};
    rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors             = RAZIX_MAX_SWAP_IMAGES_COUNT;
    rtvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = ID3D12Device9_CreateDescriptorHeap(DX12Device, &rtvHeapDesc, &IID_ID3D12DescriptorHeap, (void**) &sc->dx12.rtvHeap);
    if (FAILED(hr) || sc->dx12.rtvHeap == NULL) {
        RAZIX_RHI_LOG_ERROR("[D3D12] Failed to create RTV descriptor heap (HRESULT = 0x%08X)", (unsigned int) hr);
        if (sc->dx12.swapchain4)
            IDXGISwapChain4_Release(sc->dx12.swapchain4);
        memset(sc, 0, sizeof(dx12_swapchain));
        return;
    }

    dx12_update_swapchain_rtvs(sc);
}

static void dx12_destroy_backbuffers(rz_gfx_swapchain* sc)
{
    for (uint32_t i = 0; i < sc->imageCount; ++i) {
        if (sc->backbuffers[i].dx12.resource) {
            ID3D12Resource_Release(sc->backbuffers[i].dx12.resource);
            sc->backbuffers[i].dx12.resource = NULL;
        }
    }

    if (sc->dx12.rtvHeap) {
        ID3D12DescriptorHeap_Release(sc->dx12.rtvHeap);
        sc->dx12.rtvHeap = NULL;
    }
}

//---------------------------------------------------------------------------------------------
// Public API functions

static void dx_GlobalCtxInit(void)
{
    RAZIX_RHI_LOG_INFO("Creating DXGI factory");

    UINT createFactoryFlags = 0;
#if defined(RAZIX_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    CHECK_HR(CreateDXGIFactory2(createFactoryFlags, &IID_IDXGIFactory7, (void**) &DX12Context.factory7));
    if (DX12Context.factory7 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create DXGI Factory7");
        return;
    }

    // Set the minimum requested feature level
    DX12Context.featureLevel = D3D_FEATURE_LEVEL_12_0;

    DX12Context.adapter4 = dx12_select_best_adapter(DX12Context.factory7, DX12Context.featureLevel);
    if (DX12Context.adapter4 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to select a suitable D3D12 adapter");
        return;
    }

#ifdef RAZIX_DEBUG
    // We register D3D12Debug interface before device create
    dx12_register_debug_interface(&DX12Context);
#endif

    // Create the device
    RAZIX_RHI_LOG_INFO("Creating D3D12 Device");
    CHECK_HR(D3D12CreateDevice((IUnknown*) DX12Context.adapter4, DX12Context.featureLevel, &IID_ID3D12Device9, (void**) &DX12Context.device9));
    if (DX12Context.device9 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Device");
        return;
    }

#ifdef RAZIX_DEBUG
    // Register the D3D12 info queue after device creation
    dx12_d3d12_register_info_queue(&DX12Context);
    // Register the DXGI info queue
    dx12_dxgi_register_info_queue(&DX12Context);
    // Print the D3D12 features
    dx12_query_features(&DX12Context);
    dx12_print_features(&DX12Context.features);
    RAZIX_RHI_LOG_INFO("D3D12 Device created successfully");
#else
    RAZIX_RHI_LOG_INFO("D3D12 Device created successfully without debug features");
#endif

    // Create global command queue
    D3D12_COMMAND_QUEUE_DESC desc = {0};
    desc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask                 = 0;
    CHECK_HR(ID3D12Device9_CreateCommandQueue(DX12Device, &desc, &IID_ID3D12CommandQueue, &DX12Context.directQ));
    if (DX12Context.directQ == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Command Queue");
        return;
    }
    RAZIX_RHI_LOG_INFO("Created Global Direct Command Q");

    g_GraphicsFeatures.EnableVSync                  = false;
    g_GraphicsFeatures.TesselateTerrain             = false;
    g_GraphicsFeatures.SupportsBindless             = DX12Context.features.options.ResourceBindingTier >= D3D12_RESOURCE_BINDING_TIER_3;
    g_GraphicsFeatures.SupportsWaveIntrinsics       = true;
    g_GraphicsFeatures.SupportsShaderModel6         = DX12Context.features.shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_0;
    g_GraphicsFeatures.SupportsNullIndexDescriptors = DX12Context.features.options5.SRVOnlyTiledResourceTier3;
    g_GraphicsFeatures.SupportsTimelineSemaphores   = true;
    g_GraphicsFeatures.MaxBindlessTextures          = 4096;
    g_GraphicsFeatures.MinLaneWidth                 = DX12Context.features.options1.WaveLaneCountMin;
    g_GraphicsFeatures.MaxLaneWidth                 = DX12Context.features.options1.WaveLaneCountMax;
}

static void dx_GlobalCtxDestroy(void)
{
    if (DX12Context.device9) {
        ID3D12Device9_Release(DX12Context.device9);
        DX12Context.device9 = NULL;
    }

    if (DX12Context.adapter4) {
        IDXGIAdapter4_Release(DX12Context.adapter4);
        DX12Context.adapter4 = NULL;
    }

    if (DX12Context.factory7) {
        IDXGIFactory7_Release(DX12Context.factory7);
        DX12Context.factory7 = NULL;
    }

#ifdef RAZIX_DEBUG
    dx12_destroy_debug_handles(&DX12Context);
    dx12_track_dxgi_liveobjects(&DX12Context);
#endif
}

static void dx_CreateSyncobjFn(void* where, rz_gfx_syncobj_type type)
{
    dx12_syncobj* syncobj = (dx12_syncobj*) where;

    // Create a fence for synchronization
    CHECK_HR(ID3D12Device9_CreateFence(DX12Device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**) &syncobj->fence));
    if (syncobj->fence == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Fence");
        return;
    }

    if (type == RZ_GFX_SYNCOBJ_TYPE_CPU) {
        // Create an event handle for the fence
        syncobj->eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (syncobj->eventHandle == NULL) {
            RAZIX_RHI_LOG_ERROR("Failed to create event handle for D3D12 Fence");
            ID3D12Fence_Release(syncobj->fence);
            syncobj->fence = NULL;
        }
    }
}

static void dx_DestroySyncobjFn(rz_gfx_syncobj* syncobj)
{
    if (syncobj->dx12.fence) {
        ID3D12Fence_Release(syncobj->dx12.fence);
        syncobj->dx12.fence = NULL;
    }

    if (syncobj->dx12.eventHandle) {
        CloseHandle(syncobj->dx12.eventHandle);
        syncobj->dx12.eventHandle = NULL;
    }
}

static void dx12_CreateSwapchain(void* where, void* nativeWindowHandle, uint32_t width, uint32_t height)
{
    rz_gfx_swapchain* swapchain = (rz_gfx_swapchain*) where;
    memset(swapchain, 0, sizeof(rz_gfx_swapchain));
    swapchain->width       = width;
    swapchain->height      = height;
    swapchain->dx12.window = *(HWND*) nativeWindowHandle;

    DXGI_SWAP_CHAIN_DESC1 desc = {0};
    desc.Width                 = width;
    desc.Height                = height;
    desc.Format                = dx12_util_rz_gfx_format_to_dxgi_format(RAZIX_SWAPCHAIN_FORMAT);
    desc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount           = RAZIX_MAX_SWAP_IMAGES_COUNT;
    desc.Scaling               = DXGI_SCALING_STRETCH;
    desc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count      = 1;    // No MSAA
    desc.SampleDesc.Quality    = 0;
    desc.Flags                 = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    IDXGISwapChain1* swapchain1 = NULL;
    CHECK_HR(IDXGIFactory7_CreateSwapChainForHwnd(DX12Context.factory7, (IUnknown*) DX12Context.directQ, (HWND) nativeWindowHandle, &desc, NULL, NULL, &swapchain1));
    if (swapchain1 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Swapchain");
        return;
    }

    CHECK_HR(IDXGISwapChain1_QueryInterface(swapchain1, &IID_IDXGISwapChain4, (void**) &swapchain->dx12.swapchain4));
    IDXGISwapChain1_Release(swapchain1);

    dx12_create_backbuffers(swapchain);
}

static void dx12_DestroySwapchain(rz_gfx_swapchain* sc)
{
    dx12_destroy_backbuffers(sc);

    if (sc->dx12.swapchain4) {
        IDXGISwapChain4_Release(sc->dx12.swapchain4);
        sc->dx12.swapchain4 = NULL;
    }

    sc->dx12.imageCount         = 0;
    sc->dx12.currentBufferIndex = 0;
}

//---------------------------------------------------------------------------------------------
// Jump table

rz_rhi_api dx12_rhi = {
    .GlobalCtxInit    = dx_GlobalCtxInit,         // GlobalCtxInit
    .GlobalCtxDestroy = dx_GlobalCtxDestroy,      // GlobalCtxDestroy
    .CreateSyncobj    = dx_CreateSyncobjFn,       // CreateSyncobj
    .DestroySyncobj   = dx_DestroySyncobjFn,      // DestroySyncobj
    .CreateSwapchain  = dx12_CreateSwapchain,     // CreateSwapchain
    .DestroySwapchain = dx12_DestroySwapchain,    // DestroySwapchain
    NULL,                                         // AcquireImage
};