#ifndef RHI_H
#define RHI_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

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

    typedef uint64_t rz_gfx_timestamp;

    typedef struct rz_gfx_syncobj
    {
        rz_gfx_timestamp waitTimestamp;
#ifdef RAZIX_RENDER_API_VULKAN
            //vk_gfx_ctx vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            //dx12_gfx_synocobj dx12;
#endif
    } rz_gfx_syncobj;

    typedef struct rz_gfx_swapchain
    {
        uint32_t width;
        uint32_t height;
        uint32_t imageCount;
        uint32_t currBackBufferIdx;
#ifdef RAZIX_RENDER_API_VULKAN
            //vk_gfx_swapchain vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            //dx12_gfx_swapchain dx12;
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
            vk_gfx_ctx vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            dx12_gfx_ctx dx12;
#endif
        };

    } rz_gfx_context;

    //---------------------------------------------------------------------------------------------
    // Gfx API

    void rzGfxCtx_StartUp();
    void rzGfxCtx_ShutDown();

    rz_render_api rzGfxCtx_GetRenderAPI();
    void          rzGfxCtx_SetRenderAPI(rz_render_api api);

    //---------------------------------------------------------------------------------------------
    // RHI Jump Table

    /**
     * Rendering API initialization like Instance, Device Creation etc. will happen here! one master place to start it all up!
     */
    typedef void (*rzRHI_GlobalCtxInitFn)(void);

    /**
     * RHI API
     */
    typedef void (*rzRHI_AcquireImageFn)(rz_gfx_syncobj*);

    typedef struct
    {
        rzRHI_GlobalCtxInitFn GlobalCtxInit;
        rzRHI_AcquireImageFn  AcquireImage;
    } rz_rhi_api;

    //---------------------------------------------------------------------------------------------
    // Globals
    //---------------------------------
    extern rz_gfx_context g_GfxCtx;    // Global Graphics Context singleton instance
    //---------------------------------
    extern rz_render_api g_RenderAPI;
    //---------------------------------
    extern rz_rhi_api g_RHI;

    void rzGfxCtx_GlobalCtxInit();

#ifdef __cplusplus
}
#endif    // __cplusplus
#endif    // RHI_H
