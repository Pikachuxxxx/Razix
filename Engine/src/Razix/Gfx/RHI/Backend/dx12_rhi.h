#ifndef DX12_RHI_H
#define DX12_RHI_H
#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <stdbool.h>
    #include <stdint.h>

    #define COBJMACROS    // C Object Macros for DX12

    #include <d3d12.h>
    #include <d3d12sdklayers.h>
    #include <d3dcompiler.h>
    #include <dxgi1_6.h>
    #ifdef RAZIX_DEBUG
        #include <dxgidebug.h>
    #endif

typedef struct D3D12FeatureCache
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS  options;
    D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1;
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5;

    D3D12_FEATURE_DATA_ARCHITECTURE1  architecture;
    D3D12_FEATURE_DATA_SHADER_MODEL   shaderModel;
    D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSig;

    D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT vaSupport;

    uint32_t nodeCount;
    bool     isUMA;
    bool     isCacheCoherentUMA;
} D3D12FeatureCache;

typedef struct dx12_ctx
{
    IDXGIFactory7*      factory7;
    IDXGIAdapter4*      adapter4;
    ID3D12Device9*      device9;
    D3D_FEATURE_LEVEL   featureLevel;
    D3D12FeatureCache   features;
    ID3D12CommandQueue* directQ;
    #ifdef RAZIX_DEBUG
    ID3D12Debug3*    d3dDebug3;
    ID3D12InfoQueue* d3dInfoQ;
    IDXGIInfoQueue*  dxgiInfoQ;
    IDXGIDebug*      dxgiDebug;
    #endif
} dx12_ctx;

typedef struct dx12_syncobj
{
    ID3D12Fence* fence;
    HANDLE       eventHandle;
} dx12_syncobj;

typedef struct dx12_descriptor_handles
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu;
} dx12_descriptor_handles;

typedef struct dx12_texture
{
    ID3D12Resource*       resource;
    D3D12_RESOURCE_STATES state;
    union
    {
        dx12_descriptor_handles srv;
        dx12_descriptor_handles uav;
        dx12_descriptor_handles rtv;
        dx12_descriptor_handles dsv;
    } resView;
} dx12_texture;

typedef struct dx12_swapchain
{
    IDXGISwapChain4*        swapchain4;
    uint32_t                imageCount;
    HWND                    window;
    ID3D12DescriptorHeap*   rtvHeap;
    dx12_descriptor_handles rtvHeapStart;
} dx12_swapchain;

#endif    // RAZIX_RENDER_API_DIRECTX12
#endif    // DX12_RHI_H