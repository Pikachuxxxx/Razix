#ifndef RHI_H
#define RHI_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifndef RAZIX_API

    #if defined(RAZIX_PLATFORM_WINDOWS)
        #if defined(RAZIX_EXPORT_SYMBOLS)
            #define RAZIX_API __declspec(dllexport)
        #else
            #define RAZIX_API __declspec(dllimport)
        #endif
    #else
        // GCC, Clang: use visibility attribute
        #if __GNUC__ >= 4
            #define RAZIX_API __attribute__((visibility("default")))
        #else
            #define RAZIX_API
        #endif
    #endif

#endif    //  RAZIX_API

#include "Razix/Core/RZHandle.h"

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Gfx/RHI/Backend/dx12_rhi.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Gfx/RHI/Backend/vk_rhi.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

#define ENABLE_SYNC_LOGGING 0

// ANSI color codes
#define ANSI_COLOR_RESET  "\x1b[0m"
#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_CYAN   "\x1b[36m"
#define ANSI_COLOR_GRAY   "\x1b[90m"

    static inline const char* _rhi_log_timestamp()
    {
        static char buffer[64];
        time_t      now = time(NULL);
        struct tm   tm;

#ifdef _WIN32
        localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &tm);
        return buffer;
    }

#ifdef RAZIX_GOLD_MASTER
    #define RAZIX_RHI_LOG_INFO(...)
    #define RAZIX_RHI_LOG_WARN(...)
    #define RAZIX_RHI_LOG_ERROR(...)
    #define RAZIX_RHI_LOG_TRACE(...)
#else
    #define RAZIX_RHI_LOG_INFO(fmt, ...) \
        printf(ANSI_COLOR_GREEN "[%s] [RHI/INFO]  " ANSI_COLOR_RESET fmt "\n", _rhi_log_timestamp(), ##__VA_ARGS__)

    #define RAZIX_RHI_LOG_WARN(fmt, ...) \
        printf(ANSI_COLOR_YELLOW "[%s] [RHI/WARN]  " ANSI_COLOR_RESET fmt "\n", _rhi_log_timestamp(), ##__VA_ARGS__)

    #define RAZIX_RHI_LOG_ERROR(fmt, ...) \
        printf(ANSI_COLOR_RED "[%s] [RHI/ERROR] " ANSI_COLOR_RESET fmt "\n", _rhi_log_timestamp(), ##__VA_ARGS__)

    #define RAZIX_RHI_LOG_TRACE(fmt, ...) \
        printf(ANSI_COLOR_CYAN "[%s] [RHI/TRACE] " ANSI_COLOR_RESET fmt "\n", _rhi_log_timestamp(), ##__VA_ARGS__)
#endif    // RAZIX_GOLD_MASTER

/****************************************************************************************************
*                                         Graphics Settings                                        *
****************************************************************************************************/

/* Triple buffering is enabled by default */
#define RAZIX_ENABLE_TRIPLE_BUFFERING
/* The total number of images that the swapchain can render/present to, by default we use triple buffering, defaults to d32 buffering if disabled */
#ifdef RAZIX_ENABLE_TRIPLE_BUFFERING
    /* Frames in FLight defines the number of frames that will be rendered to while another frame is being presented (used for triple buffering)*/
    #define RAZIX_MAX_FRAMES_IN_FLIGHT  3
    #define RAZIX_MAX_SWAP_IMAGES_COUNT 3
    #define RAZIX_MAX_FRAMES            RAZIX_MAX_SWAP_IMAGES_COUNT
#elif
    #define RAZIX_MAX_SWAP_IMAGES_COUNT 2
#endif

/* Whether or not to use VMA as memory backend */
#ifdef RAZIX_PLATFORM_WINDOWS
    #define RAZIX_USE_VMA 1
#elif RAZIX_PLATFORM_MACOS
    #define RAZIX_USE_VMA 0    // Still porting WIP, so disabled idk if the SDK has it
#endif

/* Total No.Of Render Targets = typically a Max of 8 (as supported by most APIs) */
#define RAZIX_MAX_RENDER_TARGETS 8

/* Size of indices in Razix Engine, change here for global configuration */
#define RAZIX_INDICES_SIZE         sizeof(u32)    // we use 32-bit indices for now
#define RAZIX_INDICES_FORMAT       R32_UINT
#define RAZIX_INDICES_FORMAT_VK    VK_INDEX_TYPE_UINT32
#define RAZIX_INDICES_FORMAT_D3D12 DXGI_FORMAT_R32_UINT
#define RAZIX_INDICES_FORMAT_AGC   sce::Agc::IndexSize::k32
#define RAZIX_SWAPCHAIN_FORMAT     RZ_GFX_FORMAT_B8G8R8A8_UNORM

    //---------------------------------------------------------------------------------------------
    // GFX/RHI types

    typedef enum rz_render_api
    {
        RZ_RENDER_API_NONE = -1,
        RZ_RENDER_API_VULKAN,    // MacOS/Linux
        RZ_RENDER_API_D3D12,     // [WIP] // PC & XBOX
        RZ_RENDER_API_GXM,       // Not Supported yet! (PSVita)
        RZ_RENDER_API_GCM,       // Not Supported yet! (PS3)
        RZ_RENDER_API_AGC,       // Not Supported yet! (PlayStation 5)
    } rz_render_api;

    /**
     * Preset for Resource Bind views
     * For the initial stage of requirements we start off 
     * with a enum and expand to a POD struct in future
     */

    /**
     * Resource view hints provide a way to bind the resource views as needed
     * these hints can be created at initialization time, and during bind time 
     * to dynamically select the necessary view 
     * 
     * For ex. we can have a RWCubeMap viewed as Texture2DArray using the UAV hint 
     * when writing to via compute shader and as a CubeMap using the SRV hint 
     * while drawing a skybox, this is handled internally by the resource abstraction
     * 
     * get/setResourceViewHints are used during descriptor heap bind time to bind the apt
     * resource view and is exposed to client to select it explicitly, the shader reflection 
     * API will also provide it's own hints to make this automatic
     */
    typedef enum rz_gfx_resource_view_hints
    {
        RZ_GFX_RESOURCE_VIEW_FLAG_NONE         = 0,
        RZ_GFX_RESOURCE_VIEW_FLAG_SRV          = 1 << 0,
        RZ_GFX_RESOURCE_VIEW_FLAG_UAV          = 1 << 1,
        RZ_GFX_RESOURCE_VIEW_FLAG_RTV          = 1 << 2,
        RZ_GFX_RESOURCE_VIEW_FLAG_DSV          = 1 << 3,
        RZ_GFX_RESOURCE_VIEW_FLAG_CBV          = 1 << 4,
        RZ_GFX_RESOURCE_VIEW_FLAG_SAMPLER      = 1 << 5,
        RZ_GFX_RESOURCE_VIEW_FLAG_TRANSFER_SRC = 1 << 6,
        RZ_GFX_RESOURCE_VIEW_FLAG_TRANSFER_DST = 1 << 7,
    } rz_gfx_resource_view_hints;

    typedef enum rz_gfx_syncobj_type
    {
        RZ_GFX_SYNCOBJ_TYPE_CPU,
        RZ_GFX_SYNCOBJ_TYPE_GPU,
    } rz_gfx_syncobj_type;

    typedef enum rz_gfx_format
    {
        RZ_GFX_FORMAT_UNDEFINED = 0,

        // 8-bit per channel
        RZ_GFX_FORMAT_R8_UNORM,
        RZ_GFX_FORMAT_R8G8_UNORM,
        RZ_GFX_FORMAT_R8G8B8A8_UNORM,
        RZ_GFX_FORMAT_B8G8R8A8_UNORM,

        // sRGB
        RZ_GFX_FORMAT_R8G8B8A8_SRGB,
        RZ_GFX_FORMAT_B8G8R8A8_SRGB,

        // 16-bit float
        RZ_GFX_FORMAT_R16_FLOAT,
        RZ_GFX_FORMAT_R16G16_FLOAT,
        RZ_GFX_FORMAT_R16G16B16A16_FLOAT,

        // 32-bit float
        RZ_GFX_FORMAT_R32_FLOAT,
        RZ_GFX_FORMAT_R32G32_FLOAT,
        RZ_GFX_FORMAT_R32G32B32_FLOAT,
        RZ_GFX_FORMAT_R32G32B32A32_FLOAT,

        // Depth-Stencil
        RZ_GFX_FORMAT_D16_UNORM,
        RZ_GFX_FORMAT_D24_UNORM_S8_UINT,
        RZ_GFX_FORMAT_D32_FLOAT,
        RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT,

        // Packed/Compressed formats (optional depending on support)
        RZ_GFX_FORMAT_BC1_RGBA_UNORM,
        RZ_GFX_FORMAT_BC3_RGBA_UNORM,
        RZ_GFX_FORMAT_BC7_UNORM,
        RZ_GFX_FORMAT_BC7_SRGB
    } rz_gfx_format;

    typedef enum rz_gfx_texture_type
    {
        RZ_GFX_TEXTURE_TYPE_UNDEFINED = 0,
        RZ_GFX_TEXTURE_TYPE_1D,
        RZ_GFX_TEXTURE_TYPE_2D,
        RZ_GFX_TEXTURE_TYPE_3D,
        RZ_GFX_TEXTURE_TYPE_CUBE,
        RZ_GFX_TEXTURE_TYPE_1D_ARRAY,
        RZ_GFX_TEXTURE_TYPE_2D_ARRAY,
        RZ_GFX_TEXTURE_TYPE_CUBE_ARRAY
    } rz_gfx_texture_type;

    /**
      * Graphics Features as supported by the GPU, even though Engine supports them
      * the GPU can override certain setting and query run-time info like LaneWidth etc.
      */
    typedef struct rz_gfx_features
    {
        bool     EnableVSync;
        bool     TesselateTerrain;
        bool     SupportsBindless;
        bool     SupportsWaveIntrinsics;
        bool     SupportsShaderModel6;
        bool     SupportsNullIndexDescriptors;
        bool     SupportsTimelineSemaphores;
        uint32_t MaxBindlessTextures;
        uint32_t MinLaneWidth;
        uint32_t MaxLaneWidth;
    } rz_gfx_features;

    typedef struct rz_gfx_resource
    {
        const char*                name;
        rz_handle                  handle;
        rz_gfx_resource_view_hints viewHints;
    } rz_gfx_resource;

#define RAZIX_GFX_RESOURCE rz_gfx_resource resource

    typedef uint64_t rz_gfx_syncpoint;

    typedef struct rz_gfx_syncobj
    {
        RAZIX_GFX_RESOURCE;
        rz_gfx_syncpoint waitTimestamp;
#ifdef RAZIX_RENDER_API_VULKAN
            //vk_gfx_ctx vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_syncobj dx12;
#endif
    } rz_gfx_syncobj;

    typedef struct rz_gfx_texture
    {
        RAZIX_GFX_RESOURCE;
        uint32_t            width;
        uint32_t            height;
        uint32_t            depth;
        uint32_t            mipLevels;
        uint32_t            arraySize;
        rz_gfx_format       format;
        rz_gfx_texture_type textureType;
#ifdef RAZIX_RENDER_API_VULKAN
            //vk_syncobj vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_texture dx12;
#endif
    } rz_gfx_texture;

    typedef struct rz_gfx_swapchain
    {
        RAZIX_GFX_RESOURCE;
        uint32_t       width;
        uint32_t       height;
        uint32_t       imageCount;
        uint32_t       currBackBufferIdx;
        rz_gfx_texture backbuffers[RAZIX_MAX_SWAP_IMAGES_COUNT];
#ifdef RAZIX_RENDER_API_VULKAN
            //vk_swapchain vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_swapchain dx12;
#endif
    } rz_gfx_swapchain;

    typedef struct rz_gfx_context
    {
        uint32_t      width;
        uint32_t      height;
        uint32_t      frameIndex;
        rz_render_api renderAPI;
        // All global submission queues are managed within the internal contexts
        union
        {
#ifdef RAZIX_RENDER_API_VULKAN
            vk_ctx vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            dx12_ctx dx12;
#endif
        };

    } rz_gfx_context;

    //---------------------------------------------------------------------------------------------
    // Gfx API

    RAZIX_API void rzGfxCtx_StartUp();
    RAZIX_API void rzGfxCtx_ShutDown();

    RAZIX_API rz_render_api rzGfxCtx_GetRenderAPI();
    RAZIX_API void          rzGfxCtx_SetRenderAPI(rz_render_api api);
    RAZIX_API const char*   rzGfxCtx_GetRenderAPIString();

    //---------------------------------------------------------------------------------------------
    // RHI Jump Table

    /**
     * Rendering API initialization like Instance, Device Creation etc. will happen here! one master place to start it all up!
     */
    typedef void (*rzRHI_GlobalCtxInitFn)(void);
    typedef void (*rzRHI_GlobalCtxDestroyFn)(void);

    typedef void (*rzRHI_CreateSyncobjFn)(void* where, rz_gfx_syncobj_type);
    typedef void (*rzRHI_DestroySyncobjFn)(rz_gfx_syncobj*);

    typedef void (*rzRHI_CreateSwapchainFn)(void* where, void*, uint32_t, uint32_t);
    typedef void (*rzRHI_DestroySwapchainFn)(rz_gfx_swapchain*);

    /**
     * RHI API
     */
    typedef void (*rzRHI_BeginFrameFn)(rz_gfx_swapchain*, const rz_gfx_syncobj*, rz_gfx_syncpoint*, rz_gfx_syncpoint*);
    typedef void (*rzRHI_EndFrameFn)(const rz_gfx_swapchain*, const rz_gfx_syncobj*, rz_gfx_syncpoint*, rz_gfx_syncpoint*);

    typedef void (*rzRHI_AcquireImageFn)(rz_gfx_swapchain*);
    typedef void (*rzRHI_WaitOnPrevCmdsFn)(const rz_gfx_syncobj*, rz_gfx_syncpoint);
    typedef void (*rzRHI_PresentFn)(const rz_gfx_swapchain*);

    typedef rz_gfx_syncpoint (*rzRHI_SignalGPUFn)(const rz_gfx_syncobj*, rz_gfx_syncpoint*);

    typedef struct rz_rhi_api
    {
        rzRHI_GlobalCtxInitFn    GlobalCtxInit;
        rzRHI_GlobalCtxDestroyFn GlobalCtxDestroy;

        rzRHI_CreateSyncobjFn    CreateSyncobj;
        rzRHI_DestroySyncobjFn   DestroySyncobj;
        rzRHI_CreateSwapchainFn  CreateSwapchain;
        rzRHI_DestroySwapchainFn DestroySwapchain;

        rzRHI_BeginFrameFn BeginFrame;
        rzRHI_EndFrameFn   EndFrame;

        rzRHI_AcquireImageFn   AcquireImage;
        rzRHI_WaitOnPrevCmdsFn WaitOnPrevCmds;
        rzRHI_PresentFn        Present;
        rzRHI_SignalGPUFn      SignalGPU;
    } rz_rhi_api;

    //---------------------------------------------------------------------------------------------
    // Globals
    //---------------------------------
    RAZIX_API extern rz_gfx_context g_GfxCtx;    // Global Graphics Context singleton instance
    //---------------------------------
    RAZIX_API extern rz_render_api g_RenderAPI;
    //---------------------------------
    RAZIX_API extern rz_rhi_api g_RHI;
    //---------------------------------
    RAZIX_API extern rz_gfx_features g_GraphicsFeatures;
    //---------------------------------

#define rzGfxCtx_GlobalCtxInit    g_RHI.GlobalCtxInit
#define rzGfxCtx_GlobalCtxDestroy g_RHI.GlobalCtxDestroy

#define rzRHI_CreateSyncobj    g_RHI.CreateSyncobj
#define rzRHI_DestroySyncobj   g_RHI.DestroySyncobj
#define rzRHI_CreateSwapchain  g_RHI.CreateSwapchain
#define rzRHI_DestroySwapchain g_RHI.DestroySwapchain

#define rzRHI_BeginFrame g_RHI.BeginFrame
#define rzRHI_EndFrame   g_RHI.EndFrame

#define rzRHI_AcquireImage   g_RHI.AcquireImage
#define rzRHI_WaitOnPrevCmds g_RHI.WaitOnPrevCmds
#define rzRHI_Present        g_RHI.Present
#define rzRHI_SignalGPU      g_RHI.SignalGPU

#ifdef __cplusplus
}
#endif    // __cplusplus
#endif    // RHI_H
