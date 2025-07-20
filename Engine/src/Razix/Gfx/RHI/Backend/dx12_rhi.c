#include "dx12_rhi.h"

#include "Razix/Gfx/RHI/RHI.h"

#include <d3d12shader.h>
#include <d3dcompiler.h>

// TYpe friendly defines
#define DX12Context g_GfxCtx.dx12
#define DX12Device  g_GfxCtx.dx12.device10
#define CHECK_HR(x) dx12_util_check_hresult((x), __func__, __FILE__, __LINE__)
#ifndef RAZIX_GOLD_MASTER
    #define TAG_OBJECT(resource, name)                                              \
        if (resource) {                                                             \
            resource->lpVtbl->SetName(resource, dx12_util_string_to_lpcwstr(name)); \
        }
#else
TAG_OBJECT(resource, name)
#endif

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

static D3D12_COMMAND_LIST_TYPE dx12_util_rz_cmdpool_to_cmd_list_type(rz_gfx_cmdpool_type type)
{
    switch (type) {
        case RZ_GFX_CMDPOOL_TYPE_GRAPHICS: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case RZ_GFX_CMDPOOL_TYPE_COMPUTE: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        case RZ_GFX_CMDPOOL_TYPE_TRANSFER: return D3D12_COMMAND_LIST_TYPE_COPY;
        default: return D3D12_COMMAND_LIST_TYPE_DIRECT;    // Fallback to graphics
    }
}

static D3D12_RESOURCE_STATES dx12_util_res_state_translate(rz_gfx_resource_state state)
{
    switch (state) {
        case RZ_GFX_RESOURCE_STATE_RENDER_TARGET: return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case RZ_GFX_RESOURCE_STATE_SHADER_READ: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        case RZ_GFX_RESOURCE_STATE_COPY_SRC: return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case RZ_GFX_RESOURCE_STATE_COPY_DST: return D3D12_RESOURCE_STATE_COPY_DEST;
        case RZ_GFX_RESOURCE_STATE_PRESENT: return D3D12_RESOURCE_STATE_PRESENT;
        case RZ_GFX_RESOURCE_STATE_DEPTH_WRITE: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case RZ_GFX_RESOURCE_STATE_GENERAL: return D3D12_RESOURCE_STATE_COMMON;
        default: return D3D12_RESOURCE_STATE_COMMON;
    }
}

//---------------------------------------------------------------------------------------------
// Helper functions

static IDXGIAdapter4* dx12_select_best_adapter(IDXGIFactory7* factory, D3D_FEATURE_LEVEL min_feat_level)
{
    IDXGIAdapter4* best_adapter     = NULL;
    size_t         maxDedicatedVRAM = 0;

    for (uint32_t i = 0;; ++i) {
        IDXGIAdapter1* adapter1 = NULL;
        HRESULT        hr       = IDXGIFactory7_EnumAdapters1(factory, i, &adapter1);
        if (hr == DXGI_ERROR_NOT_FOUND)
            break;
        if (FAILED(hr))
            continue;

        IDXGIAdapter4* adapter4 = NULL;
        hr                      = IDXGIAdapter1_QueryInterface(adapter1, &IID_IDXGIAdapter4, &adapter4);
        IDXGIAdapter1_Release(adapter1);

        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[D3D12] Failed to query IDXGIAdapter4 (HRESULT = 0x%08X)", (unsigned int) hr);
            continue;
        }

        DXGI_ADAPTER_DESC3 desc = {0};
        hr                      = IDXGIAdapter4_GetDesc3(adapter4, &desc);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[D3D12] Failed to get adapter description (HRESULT = 0x%08X)", (unsigned int) hr);
            IDXGIAdapter4_Release(adapter4);
            continue;
        }

        if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
            IDXGIAdapter4_Release(adapter4);
            continue;    // Skip software adapters (e.g., WARP)
        }

        if (desc.DedicatedVideoMemory > maxDedicatedVRAM) {
            if (best_adapter)
                IDXGIAdapter4_Release(best_adapter);

            best_adapter     = adapter4;
            maxDedicatedVRAM = desc.DedicatedVideoMemory;
        } else {
            IDXGIAdapter4_Release(adapter4);
        }
    }

    if (!best_adapter) {
        RAZIX_RHI_LOG_ERROR("[D3D12] No suitable GPU found for D3D12");
        return NULL;
    }

    DXGI_ADAPTER_DESC3 adapterDesc = {0};
    IDXGIAdapter4_GetDesc3(best_adapter, &adapterDesc);
    RAZIX_RHI_LOG_INFO("Selected Adapter Info:");
    RAZIX_RHI_LOG_INFO("\t -> Name                  : %ls", adapterDesc.Description);
    RAZIX_RHI_LOG_INFO("\t -> VendorID              : %u", adapterDesc.VendorId);
    RAZIX_RHI_LOG_INFO("\t -> DeviceId              : %u", adapterDesc.DeviceId);
    RAZIX_RHI_LOG_INFO("\t -> SubSysId              : %u", adapterDesc.SubSysId);
    RAZIX_RHI_LOG_INFO("\t -> Revision              : %u", adapterDesc.Revision);
    RAZIX_RHI_LOG_INFO("\t -> DedicatedVideoMemory  : %zu", adapterDesc.DedicatedVideoMemory);
    RAZIX_RHI_LOG_INFO("\t -> DedicatedSystemMemory : %zu", adapterDesc.DedicatedSystemMemory);
    RAZIX_RHI_LOG_INFO("\t -> SharedSystemMemory    : %zu", adapterDesc.SharedSystemMemory);

    return best_adapter;
}

static void dx12_query_features(dx12_ctx* ctx)
{
    ID3D12Device10*    device = ctx->device10;
    D3D12FeatureCache* f      = &ctx->features;

    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS, &f->options, sizeof(f->options));
    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS1, &f->options1, sizeof(f->options1));
    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS5, &f->options5, sizeof(f->options5));

    f->architecture.NodeIndex = 0;
    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_ARCHITECTURE1, &f->architecture, sizeof(f->architecture));
    f->isUMA              = f->architecture.UMA;
    f->isCacheCoherentUMA = f->architecture.CacheCoherentUMA;

    f->shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_7;
    if (FAILED(ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_SHADER_MODEL, &f->shaderModel, sizeof(f->shaderModel)))) {
        f->shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;
    }

    f->rootSig.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_ROOT_SIGNATURE, &f->rootSig, sizeof(f->rootSig)))) {
        f->rootSig.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &f->vaSupport, sizeof(f->vaSupport));

    f->nodeCount = ID3D12Device10_GetNodeCount(device);
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
    if (!ctx->device10) {
        RAZIX_RHI_LOG_ERROR("[D3D12] D3D12 device is NULL; can't register info queue.");
        return;
    }

    // This increases the device refcount.
    if (CHECK_HR(ID3D12InfoQueue_QueryInterface(ctx->device10, &IID_ID3D12InfoQueue, (void**) &ctx->d3dInfoQ))) {
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
        rtvHandle.ptr += (size_t) i * ID3D12Device10_GetDescriptorHandleIncrementSize(DX12Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        ID3D12Device10_CreateRenderTargetView(DX12Device, d3dresource, NULL, rtvHandle);
        ID3D12Resource_SetName(d3dresource, dx12_util_string_to_lpcwstr("Swapchain Image"));

        // This is the only place where a RZ_RESOURCE is manually created, instead of using the RZResourceManager
        rz_gfx_texture texture     = {0};
        dx12_texture   dxtexture   = {0};
        texture.resource.pName     = "$SWAPCHAIN_IMAGE$";
        texture.resource.handle    = (rz_handle){i, i};
        texture.resource.viewHints = RZ_GFX_RESOURCE_VIEW_FLAG_RTV;
        dxtexture.resource         = d3dresource;
        dxtexture.state            = D3D12_RESOURCE_STATE_PRESENT;
        dxtexture.resView.rtv.cpu  = rtvHandle;
        texture.dx12               = dxtexture;
        texture.desc.height        = sc->height;
        texture.desc.width         = sc->width;
        texture.desc.arraySize     = 1;
        texture.desc.mipLevels     = 1;
        texture.desc.format        = RAZIX_SWAPCHAIN_FORMAT;
        texture.desc.textureType   = RZ_GFX_TEXTURE_TYPE_2D;
        sc->backbuffers[i]         = texture;

        TAG_OBJECT(d3dresource, "Swapchain Backbuffer Resource");
    }
}

static void dx12_create_backbuffers(rz_gfx_swapchain* sc)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {0};
    rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors             = RAZIX_MAX_SWAP_IMAGES_COUNT;
    rtvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = ID3D12Device10_CreateDescriptorHeap(DX12Device, &rtvHeapDesc, &IID_ID3D12DescriptorHeap, (void**) &sc->dx12.rtvHeap);
    if (FAILED(hr) || sc->dx12.rtvHeap == NULL) {
        RAZIX_RHI_LOG_ERROR("[D3D12] Failed to create RTV descriptor heap (HRESULT = 0x%08X)", (unsigned int) hr);
        if (sc->dx12.swapchain4)
            IDXGISwapChain4_Release(sc->dx12.swapchain4);
        memset(sc, 0, sizeof(dx12_swapchain));
        return;
    }
    TAG_OBJECT(sc->dx12.rtvHeap, "Swapchain Heap");

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

static void dx12_GlobalCtxInit(void)
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
    CHECK_HR(D3D12CreateDevice((IUnknown*) DX12Context.adapter4, DX12Context.featureLevel, &IID_ID3D12Device10, (void**) &DX12Context.device10));
    if (DX12Context.device10 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Device");
        return;
    }

#ifdef RAZIX_DEBUG
    // TODO: Use engine setting to enable/disable debugging
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
    CHECK_HR(ID3D12Device10_CreateCommandQueue(DX12Device, &desc, &IID_ID3D12CommandQueue, &DX12Context.directQ));
    if (DX12Context.directQ == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Command Queue");
        return;
    }
    RAZIX_RHI_LOG_INFO("Created Global Direct Command Q");
    TAG_OBJECT(DX12Context.directQ, "Direct Command Q");

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

static void dx12_GlobalCtxDestroy(void)
{
    if (DX12Context.directQ) {
        ID3D12CommandQueue_Release(DX12Context.directQ);
        DX12Context.directQ = NULL;
    }

    if (DX12Context.device10) {
        ID3D12Device10_Release(DX12Context.device10);
        DX12Context.device10 = NULL;
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

static void dx12_CreateSyncobjFn(void* where, rz_gfx_syncobj_type type)
{
    rz_gfx_syncobj* syncobj = (rz_gfx_syncobj*) where;

    // Create a fence for synchronization
    CHECK_HR(ID3D12Device10_CreateFence(DX12Device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**) &syncobj->dx12.fence));
    if (syncobj->dx12.fence == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Fence");
        return;
    }

    if (type == RZ_GFX_SYNCOBJ_TYPE_CPU) {
        // Create an event handle for the fence
        syncobj->dx12.eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (syncobj->dx12.eventHandle == NULL) {
            RAZIX_RHI_LOG_ERROR("Failed to create event handle for D3D12 Fence");
            ID3D12Fence_Release(syncobj->dx12.fence);
            syncobj->dx12.fence = NULL;
        }
    }

    TAG_OBJECT(DX12Context.directQ, "Syncobj");
}

static void dx12_DestroySyncobjFn(rz_gfx_syncobj* syncobj)
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
    // we typically get a GLFWwinsow*
    if (nativeWindowHandle == NULL) {
        RAZIX_RHI_LOG_ERROR("Native window handle is NULL, cannot create swapchain");
        return;
    }
    if (width == 0 || height == 0) {
        RAZIX_RHI_LOG_ERROR("Invalid swapchain dimensions: %ux%u", width, height);
        return;
    }

    rz_gfx_swapchain* swapchain = (rz_gfx_swapchain*) where;
    memset(swapchain, 0, sizeof(rz_gfx_swapchain));
    swapchain->width       = width;
    swapchain->height      = height;
    swapchain->dx12.window = *(HWND*) nativeWindowHandle;
    swapchain->imageCount  = RAZIX_MAX_SWAP_IMAGES_COUNT;

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
    CHECK_HR(IDXGIFactory7_CreateSwapChainForHwnd(DX12Context.factory7, (IUnknown*) DX12Context.directQ, (HWND) swapchain->dx12.window, &desc, NULL, NULL, &swapchain1));
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

    sc->dx12.imageCount   = 0;
    sc->currBackBufferIdx = 0;
}

static void dx12_CreateCmdPool(void* where)
{
    rz_gfx_cmdpool* cmdPool = (rz_gfx_cmdpool*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&cmdPool->resource.handle), "Invalid cmd pool handle, who is allocating this? ResourceManager should create a valid handle");

    CHECK_HR(ID3D12Device10_CreateCommandAllocator(DX12Device, dx12_util_rz_cmdpool_to_cmd_list_type(cmdPool->resource.desc.cmdpoolDesc.poolType), &IID_ID3D12CommandAllocator, (void**) &cmdPool->dx12.cmdAlloc));
    if (cmdPool->dx12.cmdAlloc == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Command Allocator");
        return;
    }
    cmdPool->type = cmdPool->resource.desc.cmdpoolDesc.poolType;
    TAG_OBJECT(cmdPool->dx12.cmdAlloc, cmdPool->resource.pName);
}

static void dx12_DestroyCmdPool(void* cmdPool)
{
    RAZIX_RHI_ASSERT(cmdPool != NULL, "Command pool is NULL, cannot destroy");
    rz_gfx_cmdpool* cmdPoolPtr = (rz_gfx_cmdpool*) cmdPool;
    if (cmdPoolPtr->dx12.cmdAlloc) {
        ID3D12CommandAllocator_Release(cmdPoolPtr->dx12.cmdAlloc);
        cmdPoolPtr->dx12.cmdAlloc = NULL;
    }
}

static void dx12_CreateCmdBuf(void* where)
{
    rz_gfx_cmdbuf* cmdBuf = (rz_gfx_cmdbuf*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&cmdBuf->resource.handle), "Invalid command buffer handle, who is allocating this? ResourceManager should create a valid handle");
    const rz_gfx_cmdpool* cmdPool = cmdBuf->resource.desc.cmdbufDesc.pool;
    RAZIX_RHI_ASSERT(cmdPool != NULL, "Command buffer must have a valid command pool");
    cmdBuf->dx12.cmdAlloc = cmdBuf->resource.desc.cmdbufDesc.pool->dx12.cmdAlloc;
    RAZIX_RHI_ASSERT(cmdBuf->dx12.cmdAlloc != NULL, "Command buffer must have a valid command allocator");

    CHECK_HR(ID3D12Device10_CreateCommandList(DX12Device, 0, dx12_util_rz_cmdpool_to_cmd_list_type(cmdPool->type), cmdPool->dx12.cmdAlloc, NULL, &IID_ID3D12GraphicsCommandList, (void**) &cmdBuf->dx12.cmdList));
    if (cmdBuf->dx12.cmdList == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Command List");
        return;
    }
    // Immediately close it so that the first use can Reset() safely
    CHECK_HR(ID3D12GraphicsCommandList_Close(cmdBuf->dx12.cmdList));
    TAG_OBJECT(cmdBuf->dx12.cmdList, cmdBuf->resource.pName);
}

static void dx12_DestroyCmdBuf(void* cmdBuf)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer is NULL, cannot destroy");
    rz_gfx_cmdbuf* cmdBufPtr = (rz_gfx_cmdbuf*) cmdBuf;
    if (cmdBufPtr->dx12.cmdList) {
        ID3D12GraphicsCommandList_Release(cmdBufPtr->dx12.cmdList);
        cmdBufPtr->dx12.cmdList = NULL;
    }
}

static void dx12_CreateShader(void* where)
{
    rz_gfx_shader* shader = (rz_gfx_shader*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&shader->resource.handle), "Invalid shader handle, who is allocating this? ResourceManager should create a valid handle");

    // In Direct3D 12, shaders are not standalone objects like in Vulkan.
    // There's no need to create an intermediate shader module.
    // The raw shader bytecode (e.g., .cso or .dxil) is passed directly to the pipeline state during creation.
    // Therefore, this function intentionally does nothing. Actual shader usage happens during PSO creation.

    rz_gfx_shader_desc* desc = &shader->resource.desc.shaderDesc;

    switch (desc->pipelineType) {
        case RZ_GFX_PIPELINE_TYPE_GRAPHICS:
            if (desc->raster.vs.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_VERTEX;
            if (desc->raster.ps.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_PIXEL;
            if (desc->raster.gs.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_GEOMETRY;
            if (desc->raster.tcs.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_TESSELLATION_CONTROL;
            if (desc->raster.tes.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_TESSELLATION_EVALUATION;
            if (desc->mesh.task.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_TASK;
            if (desc->mesh.mesh.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_MESH;
            if (desc->mesh.ps.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_PIXEL;
            break;
        case RZ_GFX_PIPELINE_TYPE_COMPUTE:
            if (desc->compute.cs.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_COMPUTE;
            break;
        case RZ_GFX_PIPELINE_TYPE_RAYTRACING:
            if (desc->raytracing.rgen.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_GEN;
            if (desc->raytracing.miss.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_MISS;
            if (desc->raytracing.chit.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_CLOSEST_HIT;
            if (desc->raytracing.ahit.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_ANY_HIT;
            if (desc->raytracing.callable.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_CALLABLE;
        default:
            RAZIX_RHI_ASSERT(false, "Invalid pipeline type for shader!");
            break;
    }
}

static void dx12_DestroyShader(void* shader)
{
    RAZIX_RHI_ASSERT(shader != NULL, "Shader is NULL, cannot destroy");
    rz_gfx_shader* shaderPtr = (rz_gfx_shader*) shader;
    (void) shaderPtr;

    shaderPtr->shaderStageMask = 0;
    rz_gfx_shader_desc* desc   = &shaderPtr->resource.desc.shaderDesc;

    switch (desc->pipelineType) {
        case RZ_GFX_PIPELINE_TYPE_GRAPHICS: {
            if (desc->raster.vs.bytecode) {
                RAZIX_RHI_ASSERT(desc->raster.vs.size > 0, "VS bytecode has invalid size");
                free((void*) desc->raster.vs.bytecode);
                desc->raster.vs.bytecode = NULL;
            }
            if (desc->raster.ps.bytecode) {
                RAZIX_RHI_ASSERT(desc->raster.ps.size > 0, "PS bytecode has invalid size");
                free((void*) desc->raster.ps.bytecode);
                desc->raster.ps.bytecode = NULL;
            }
            if (desc->raster.gs.bytecode) {
                RAZIX_RHI_ASSERT(desc->raster.gs.size > 0, "GS bytecode has invalid size");
                free((void*) desc->raster.gs.bytecode);
                desc->raster.gs.bytecode = NULL;
            }
            if (desc->raster.tcs.bytecode) {
                RAZIX_RHI_ASSERT(desc->raster.tcs.size > 0, "TCS bytecode has invalid size");
                free((void*) desc->raster.tcs.bytecode);
                desc->raster.tcs.bytecode = NULL;
            }
            if (desc->raster.tes.bytecode) {
                RAZIX_RHI_ASSERT(desc->raster.tes.size > 0, "TES bytecode has invalid size");
                free((void*) desc->raster.tes.bytecode);
                desc->raster.tes.bytecode = NULL;
            }
            if (desc->mesh.task.bytecode) {
                RAZIX_RHI_ASSERT(desc->mesh.task.size > 0, "Task shader bytecode has invalid size");
                free((void*) desc->mesh.task.bytecode);
                desc->mesh.task.bytecode = NULL;
            }
            if (desc->mesh.mesh.bytecode) {
                RAZIX_RHI_ASSERT(desc->mesh.mesh.size > 0, "Mesh shader bytecode has invalid size");
                free((void*) desc->mesh.mesh.bytecode);
                desc->mesh.mesh.bytecode = NULL;
            }
            if (desc->mesh.ps.bytecode) {
                RAZIX_RHI_ASSERT(desc->mesh.ps.size > 0, "PS bytecode has invalid size");
                free((void*) desc->mesh.ps.bytecode);
                desc->mesh.ps.bytecode = NULL;
            }
            break;
            break;
        }

        case RZ_GFX_PIPELINE_TYPE_COMPUTE: {
            if (desc->compute.cs.bytecode) {
                RAZIX_RHI_ASSERT(desc->compute.cs.size > 0, "CS bytecode has invalid size");
                free((void*) desc->compute.cs.bytecode);
                desc->compute.cs.bytecode = NULL;
            }
            break;
        }

        case RZ_GFX_PIPELINE_TYPE_RAYTRACING: {
            if (desc->raytracing.rgen.bytecode) {
                RAZIX_RHI_ASSERT(desc->raytracing.rgen.size > 0, "RGEN bytecode has invalid size");
                free((void*) desc->raytracing.rgen.bytecode);
                desc->raytracing.rgen.bytecode = NULL;
            }
            if (desc->raytracing.miss.bytecode) {
                RAZIX_RHI_ASSERT(desc->raytracing.miss.size > 0, "MISS bytecode has invalid size");
                free((void*) desc->raytracing.miss.bytecode);
                desc->raytracing.miss.bytecode = NULL;
            }
            if (desc->raytracing.chit.bytecode) {
                RAZIX_RHI_ASSERT(desc->raytracing.chit.size > 0, "CHIT bytecode has invalid size");
                free((void*) desc->raytracing.chit.bytecode);
                desc->raytracing.chit.bytecode = NULL;
            }
            if (desc->raytracing.ahit.bytecode) {
                RAZIX_RHI_ASSERT(desc->raytracing.ahit.size > 0, "AHIT bytecode has invalid size");
                free((void*) desc->raytracing.ahit.bytecode);
                desc->raytracing.ahit.bytecode = NULL;
            }
            if (desc->raytracing.callable.bytecode) {
                RAZIX_RHI_ASSERT(desc->raytracing.callable.size > 0, "CALLABLE bytecode has invalid size");
                free((void*) desc->raytracing.callable.bytecode);
                desc->raytracing.callable.bytecode = NULL;
            }
            break;
        }
        default:
            RAZIX_RHI_LOG_WARN("[D3D12 Shader] Invalid or unhandled pipeline type during destruction.");
            break;
    }
}

static void dx12_CreateRootSignature(void* where)
{
    rz_gfx_root_signature* rootSig = (rz_gfx_root_signature*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&rootSig->resource.handle), "Invalid rootsignaure handle, who is allocating this? ResourceManager should create a valid handle");

    rz_gfx_root_signature_desc* desc = &rootSig->resource.desc.rootSignatureDesc;

    for (uint32_t i = 0; i < desc->descriptorTableCount; i++) {

    }

}

static void dx12_DestroyRootSignature(void* ptr)
{
}

//---------------------------------------------------------------------------------------------
// RHI

static void dx12_AcquireImage(rz_gfx_swapchain* sc)
{
    sc->currBackBufferIdx = IDXGISwapChain4_GetCurrentBackBufferIndex(sc->dx12.swapchain4);
}

static void dx12_WaitOnPrevCmds(const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint waitSyncPoint)
{
    rz_gfx_syncpoint completed = ID3D12Fence_GetCompletedValue(frameSyncobj->dx12.fence);

    if (completed < waitSyncPoint) {
        // Set the fence event and check for failure
        HRESULT hr = ID3D12Fence_SetEventOnCompletion(frameSyncobj->dx12.fence, waitSyncPoint, frameSyncobj->dx12.eventHandle);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] SetEventOnCompletion(%llu) failed -> 0x%08X", waitSyncPoint, hr);
        }

        // Wait for the event and log errors only
        DWORD result = WaitForSingleObject(frameSyncobj->dx12.eventHandle, INFINITE);
        if (result != WAIT_OBJECT_0) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] WaitForSingleObject -> %s",
                result == WAIT_TIMEOUT ? "WAIT_TIMEOUT" : "WAIT_FAILED");
        }

        // Verify fence advanced
        rz_gfx_syncpoint new_completed = ID3D12Fence_GetCompletedValue(frameSyncobj->dx12.fence);
        if (new_completed < waitSyncPoint) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] fence did not advance: completed=%llu, expected>=%llu", new_completed, waitSyncPoint);
        }
    }
}

static void dx12_Present(const rz_gfx_swapchain* sc)
{
    CHECK_HR(IDXGISwapChain4_Present(sc->dx12.swapchain4, 0, DXGI_PRESENT_ALLOW_TEARING));
}

static void dx12_BeginCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    CHECK_HR(ID3D12CommandAllocator_Reset(cmdBuf->dx12.cmdAlloc));
    CHECK_HR(ID3D12GraphicsCommandList_Reset(cmdBuf->dx12.cmdList, cmdBuf->dx12.cmdAlloc, NULL));
}

static void dx12_EndCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    CHECK_HR(ID3D12GraphicsCommandList_Close(cmdBuf->dx12.cmdList));
}

static void dx12_SubmitCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    ID3D12GraphicsCommandList* cmdLists[] = {cmdBuf->dx12.cmdList};
    ID3D12CommandQueue_ExecuteCommandLists(DX12Context.directQ, 1, (ID3D12CommandList**) cmdLists);
}

static void dx12_BeginRenderPass(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_renderpass renderPass)
{
    ID3D12GraphicsCommandList* cmdList = cmdBuf->dx12.cmdList;

    // Set Scissor and Rects
    D3D12_VIEWPORT vp = {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width    = (FLOAT) RAZIX_X(renderPass.extents),
        .Height   = (FLOAT) RAZIX_Y(renderPass.extents),
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f};
    ID3D12GraphicsCommandList_RSSetViewports(cmdBuf->dx12.cmdList, 1, &vp);

    D3D12_RECT scissor = {
        .left   = 0,
        .top    = 0,
        .right  = (LONG) RAZIX_X(renderPass.extents),
        .bottom = (LONG) RAZIX_Y(renderPass.extents)};
    ID3D12GraphicsCommandList_RSSetScissorRects(cmdBuf->dx12.cmdList, 1, &scissor);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[RAZIX_MAX_RENDER_TARGETS] = {0};
    uint32_t                    rtvCount                             = renderPass.colorAttachmentsCount;

    for (uint32_t i = 0; i < rtvCount; ++i)
        rtvHandles[i] = renderPass.colorAttachments[i].pTexture->dx12.resView.rtv.cpu;

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {0};
    bool                        hasDepth  = renderPass.depthAttachment.pTexture != NULL;

    if (hasDepth) {
        dsvHandle = renderPass.depthAttachment.pTexture->dx12.resView.dsv.cpu;
    }

    for (uint32_t i = 0; i < rtvCount; ++i) {
        if (renderPass.colorAttachments[i].clear) {
            float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            memcpy(clearColor, renderPass.colorAttachments[i].clearColor.raw, sizeof(float) * 4);
            ID3D12GraphicsCommandList_ClearRenderTargetView(cmdList, rtvHandles[i], clearColor, 0, NULL);
        }
    }

    if (hasDepth && renderPass.depthAttachment.clear) {
        ID3D12GraphicsCommandList_ClearDepthStencilView(cmdList, dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
    }

    ID3D12GraphicsCommandList_OMSetRenderTargets(cmdList, rtvCount, rtvHandles, FALSE, hasDepth ? &dsvHandle : NULL);
}

static void dx12_EndRenderPass(const rz_gfx_cmdbuf* cmdBuf)
{
    // In DX12 there's no explicit EndRenderPass unless using newer RenderPass APIs.
    // For basic OMSetRenderTargets path, this is a no-op.
    (void) cmdBuf;
}

static void dx12_SetViewport(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_viewport* viewport)
{
    D3D12_VIEWPORT vp = {
        .TopLeftX = (FLOAT) viewport->x,
        .TopLeftY = (FLOAT) viewport->y,
        .Width    = (FLOAT) viewport->width,
        .Height   = (FLOAT) viewport->height,
        .MinDepth = (FLOAT) viewport->minDepth,
        .MaxDepth = (FLOAT) viewport->maxDepth};
    ID3D12GraphicsCommandList_RSSetViewports(cmdBuf->dx12.cmdList, 1, &vp);
}

static void dx12_SetScissorRect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_rect* rect)
{
    D3D12_RECT scissor = {
        .left   = (LONG) rect->x,
        .top    = (LONG) rect->y,
        .right  = (LONG) rect->x + rect->width,
        .bottom = (LONG) rect->y + rect->height};
    ID3D12GraphicsCommandList_RSSetScissorRects(cmdBuf->dx12.cmdList, 1, &scissor);
}

static void dx12_InsertImageBarrier(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* texture, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    D3D12_RESOURCE_BARRIER barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = texture->dx12.resource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,    // TODO: use mip/layer combo
            .StateBefore = dx12_util_res_state_translate(beforeState),
            .StateAfter  = dx12_util_res_state_translate(afterState),
        }};
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 1, &barrier);
}

//---------------------------------------------------------------------------------------------
static rz_gfx_syncpoint dx12_Signal(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint* globalSyncPoint)
{
    if (syncobj && syncobj->dx12.fence) {
        ID3D12Fence*     fence           = syncobj->dx12.fence;
        rz_gfx_syncpoint signalSyncpoint = ++(*globalSyncPoint);
        CHECK_HR(ID3D12CommandQueue_Signal(DX12Context.directQ, fence, signalSyncpoint));
        return signalSyncpoint;
    }
    return (rz_gfx_syncpoint) -1;
}

static void dx12_FlushGPUWork(const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* globalSyncpoint)
{
    rz_gfx_syncpoint signalValue = dx12_Signal(frameSyncobj, globalSyncpoint);
    dx12_WaitOnPrevCmds(frameSyncobj, signalValue);
}

static void dx12_ResizeSwapchain(rz_gfx_swapchain* sc, uint32_t width, uint32_t height)
{
    sc->width = width;
    sc->width = height;

    for (uint32_t i = 0; i < sc->imageCount; ++i) {
        if (sc->backbuffers[i].dx12.resource) {
            ID3D12Resource_Release(sc->backbuffers[i].dx12.resource);
            sc->backbuffers[i].dx12.resource = NULL;
        }
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
    IDXGISwapChain4*     sc4           = sc->dx12.swapchain4;
    IDXGISwapChain4_GetDesc(sc4, &swapChainDesc);
    CHECK_HR(IDXGISwapChain4_ResizeBuffers(sc->dx12.swapchain4, sc->imageCount, sc->width, sc->height, dx12_util_rz_gfx_format_to_dxgi_format(RAZIX_SWAPCHAIN_FORMAT), swapChainDesc.Flags));

    dx12_update_swapchain_rtvs(sc);
}

//---------------------------------------------------------------------------------------------

static void dx12_BeginFrame(rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    (void) (globalSyncPoint);
    // This is reverse to what Vulkan does, we first wait for previous work to be done
    // and then acquire a new back buffer, because acquire is a GPU operation in vulkan,
    // here we just ask for index and wait on GPU until work is done and that back buffer is free to use
    dx12_AcquireImage(sc);
    uint32_t         frameIdx         = sc->currBackBufferIdx;
    rz_gfx_syncpoint currentSyncPoint = frameSyncPoints[frameIdx];
    dx12_WaitOnPrevCmds(frameSyncobj, currentSyncPoint);
}

static void dx12_EndFrame(const rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    dx12_Present(sc);
    rz_gfx_syncpoint wait_value = dx12_Signal(frameSyncobj, globalSyncPoint);
    uint32_t         frame_idx  = sc->currBackBufferIdx;
    frameSyncPoints[frame_idx]  = wait_value;
}

//---------------------------------------------------------------------------------------------
// Jump table

rz_rhi_api dx12_rhi = {
    .GlobalCtxInit    = dx12_GlobalCtxInit,       // GlobalCtxInit
    .GlobalCtxDestroy = dx12_GlobalCtxDestroy,    // GlobalCtxDestroy
    .CreateSyncobj    = dx12_CreateSyncobjFn,     // CreateSyncobj
    .DestroySyncobj   = dx12_DestroySyncobjFn,    // DestroySyncobj
    .CreateSwapchain  = dx12_CreateSwapchain,     // CreateSwapchain
    .DestroySwapchain = dx12_DestroySwapchain,    // DestroySwapchain
    .CreateCmdPool    = dx12_CreateCmdPool,       // CreateCmdPool
    .DestroyCmdPool   = dx12_DestroyCmdPool,      // DestroyCmdPool
    .CreateCmdBuf     = dx12_CreateCmdBuf,        // CreateCmdBuf
    .DestroyCmdBuf    = dx12_DestroyCmdBuf,       // DestroyCmdBuf

    .AcquireImage       = dx12_AcquireImage,          // AcquireImage
    .WaitOnPrevCmds     = dx12_WaitOnPrevCmds,        // WaitOnPrevCmds
    .Present            = dx12_Present,               // Present
    .BeginCmdBuf        = dx12_BeginCmdBuf,           // BeginCmdBuf
    .EndCmdBuf          = dx12_EndCmdBuf,             // EndCmdBuf
    .SubmitCmdBuf       = dx12_SubmitCmdBuf,          // SubmitCmdBuf
    .BeginRenderPass    = dx12_BeginRenderPass,       // BeginRenderPass
    .EndRenderPass      = dx12_EndRenderPass,         // EndRenderPass
    .SetViewport        = dx12_SetViewport,           // SetViewport
    .SetScissorRect     = dx12_SetScissorRect,        // SetScissorRect
    .InsertImageBarrier = dx12_InsertImageBarrier,    // InsertImageBarrier

    .SignalGPU       = dx12_Signal,             // Signal
    .FlushGPUWork    = dx12_FlushGPUWork,       // FlushGPUWork
    .ResizeSwapchain = dx12_ResizeSwapchain,    // ResizeSwapchain
    .BeginFrame      = dx12_BeginFrame,         // BeginFrame
    .EndFrame        = dx12_EndFrame,           // EndFrame
};