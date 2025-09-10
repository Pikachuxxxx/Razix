#ifndef RHI_H
#define RHI_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifndef RAZIX_RHI_API
    #if defined(RAZIX_PLATFORM_WINDOWS)
        #if defined(RAZIX_EXPORT_SYMBOLS)
            #define RAZIX_RHI_API __declspec(dllexport)
        #else
            #define RAZIX_RHI_API __declspec(dllimport)
        #endif
    #else
        // GCC, Clang: use visibility attribute
        #if __GNUC__ >= 4
            #define RAZIX_RHI_API __attribute__((visibility("default")))
        #else
            #define RAZIX_RHI_API
        #endif
    #endif
#endif    //  RAZIX_RHI_API

#if defined(_MSC_VER)
    #define RAZIX_RHI_ALIGN_16 __declspec(align(16))
#elif defined(__GNUC__) || defined(__clang__)
    #define RAZIX_RHI_ALIGN_16 __attribute__((aligned(16)))
#else
    #error "Unsupported compiler for RAZIX_RHI_ALIGN_16"
#endif

#include "Razix/Core/RZHandle.h"

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Gfx/RHI/Backend/dx12_rhi.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Gfx/RHI/Backend/vk_rhi.h"
#endif

// Include profiling support for RHI operations

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

#include "Razix/Core/Profiling/RZProfiling.h"

#define ENABLE_SYNC_LOGGING 0

// ANSI color codes
// ANSI Color Codes
#define ANSI_COLOR_RESET     "\033[0m"
#define ANSI_COLOR_BOLD      "\033[1m"
#define ANSI_COLOR_DIM       "\033[2m"
#define ANSI_COLOR_UNDERLINE "\033[4m"

// Regular Colors
#define ANSI_COLOR_BLACK   "\033[30m"
#define ANSI_COLOR_RED     "\033[31m"
#define ANSI_COLOR_GREEN   "\033[32m"
#define ANSI_COLOR_YELLOW  "\033[33m"
#define ANSI_COLOR_BLUE    "\033[34m"
#define ANSI_COLOR_MAGENTA "\033[35m"
#define ANSI_COLOR_CYAN    "\033[36m"
#define ANSI_COLOR_WHITE   "\033[37m"
#define ANSI_COLOR_GRAY    "\033[90m"

// Bright Colors
#define ANSI_COLOR_BRIGHT_BLACK   "\033[90m"
#define ANSI_COLOR_BRIGHT_RED     "\033[91m"
#define ANSI_COLOR_BRIGHT_GREEN   "\033[92m"
#define ANSI_COLOR_BRIGHT_YELLOW  "\033[93m"
#define ANSI_COLOR_BRIGHT_BLUE    "\033[94m"
#define ANSI_COLOR_BRIGHT_MAGENTA "\033[95m"
#define ANSI_COLOR_BRIGHT_CYAN    "\033[96m"
#define ANSI_COLOR_BRIGHT_WHITE   "\033[97m"

// Background Colors
#define ANSI_BG_BLACK   "\033[40m"
#define ANSI_BG_RED     "\033[41m"
#define ANSI_BG_GREEN   "\033[42m"
#define ANSI_BG_YELLOW  "\033[43m"
#define ANSI_BG_BLUE    "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN    "\033[46m"
#define ANSI_BG_WHITE   "\033[47m"

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

#if defined(_MSC_VER)
    #define RAZIX_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define RAZIX_DEBUG_BREAK() __builtin_trap()
#else
    #define RAZIX_DEBUG_BREAK() ((void) 0)
#endif

#ifdef RAZIX_DEBUG
    #define RAZIX_RHI_ASSERT(cond, msg, ...)                                                             \
        do {                                                                                             \
            if (!(cond)) {                                                                               \
                RAZIX_RHI_LOG_ERROR("[RHI ASSERT] %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
                RAZIX_DEBUG_BREAK();                                                                     \
                abort();                                                                                 \
            }                                                                                            \
        } while (0)

    #define RAZIX_RHI_ABORT()                                                     \
        do {                                                                      \
            RAZIX_RHI_LOG_ERROR("RHI Abort called at %s:%d", __FILE__, __LINE__); \
            RAZIX_DEBUG_BREAK();                                                  \
            abort();                                                              \
        } while (0)

#else
    #define RAZIX_RHI_ASSERT(cond, msg, ...)
    #define RAZIX_RHI_ABORT()
#endif

#if defined(_MSC_VER)
    #include <intrin.h>
    #pragma intrinsic(_BitScanReverse)
    static inline unsigned int rz_clz32(unsigned int x)
    {
        unsigned long index;
        _BitScanReverse(&index, x);
        return 31 - index;
    }
#elif defined(__clang__) || defined(__GNUC__)
static inline unsigned int rz_clz32(unsigned int x)
{
    return __builtin_clz(x);
}
#else
    #error "Compiler not supported for RAZIX_RHI_BITS_FOR_ENUM"
#endif

#define RAZIX_RHI_BITS_FOR_ENUM(count) ((count) <= 1 ? 0 : (32 - rz_clz32((count) - 1)))

#define RAZIX_RHI_ALIGN(n, a) (((size_t) (n) + ((size_t) (a) - 1)) & ~(size_t) ((a) - 1))    // 'a' needs to be a power of 2

#define RAZIX_RHI_ASSERT_RES_TYPE(resPtr, expectedEnum, contextMsg) \
    RAZIX_RHI_ASSERT((resPtr) != NULL && (resPtr)->resource.type == (expectedEnum), "[DX12][TypeCheck] %s | Got=%d Expected=%d", (contextMsg), (resPtr) ? (resPtr)->resource.type : -1, (expectedEnum))

/****************************************************************************************************
*                                         Graphics Settings                                        *
****************************************************************************************************/

/* Backend API configurations */
#define RAZIX_VK_API_VERSION VK_API_VERSION_1_3 /* Min supported version by razix engine */

/* Frames in Flight defines the number of frames that will be rendered to while another frame is being presented, CPU can prepare frames ahead */
#define RAZIX_MAX_FRAMES_IN_FLIGHT  2
#define RAZIX_MAX_SWAP_IMAGES_COUNT 4    // Adreno GPUs might sometimes report upto 4 images in the swapchain for vulkan
#define RAZIX_MIN_SWAP_IMAGES_COUNT 3
#define RAZIX_MAX_FRAMES            RAZIX_MAX_SWAP_IMAGES_COUNT

/* Whether or not to use VMA as memory backend */
#ifdef RAZIX_PLATFORM_WINDOWS
    #define RAZIX_USE_VMA 1
#elif RAZIX_PLATFORM_MACOS
    #define RAZIX_USE_VMA 0    // Still porting WIP, so disabled idk if the SDK has it
#endif

/* Total No.Of Render Targets = typically a Max of 8 (as supported by most APIs) */
#define RAZIX_MAX_RENDER_TARGETS 8

/* Size of indices in Razix Engine, change here for global configuration */
#define RAZIX_INDICES_SIZE                       sizeof(u32)    // we use 32-bit indices for now
#define RAZIX_INDICES_FORMAT                     RZ_GFX_INDEX_TYPE_UINT32
#define RAZIX_INDICES_FORMAT_VK                  VK_INDEX_TYPE_UINT32
#define RAZIX_INDICES_FORMAT_D3D12               DXGI_FORMAT_R32_UINT
#define RAZIX_INDICES_FORMAT_AGC                 sce::Agc::IndexSize::k32
#define RAZIX_SWAPCHAIN_FORMAT                   RZ_GFX_FORMAT_B8G8R8A8_UNORM
#define RAZIX_SWAPCHAIN_FORMAT_VK                VK_FORMAT_B8G8R8A8_UNORM
#define RAZIX_SWAPCHAIN_FORMAT_DX12              DXGI_FORMAT_B8G8R8A8_UNORM
#define RAZIX_MAX_DESCRIPTOR_TABLES              8
#define RAZIX_MAX_DESCRIPTOR_RANGES              32
#define RAZIX_MAX_ROOT_CONSTANTS                 2
#define RAZIX_MAX_VERTEX_ATTRIBUTES              32
#define RAZIX_INITIAL_DESCRIPTOR_NUM_FREE_RANGES 16
#define RAZIX_CONSTANT_BUFFER_MIN_ALIGNMENT      256                                  // 256 bytes is the minimum alignment for constant buffers in Vulkan and D3D12
#define RAZIX_MAX_ALLOWED_HEAPS_TO_BIND          RZ_GFX_DESCRIPTOR_HEAP_TYPE_COUNT    // Maximum number of heaps that can be bound at once
#define RAZIX_MAX_ALLOWED_TABLES_TO_BIND         16                                   // Maximum number of descriptor tables that can be bound at once
#define RAZIX_RESOURCE_VIEW_DIMENSION_FULL       0xffffffff
#define RAZIX_MAX_VERTEX_BUFFERS_BOUND           16

#define RAZIX_PUSH_CONSTANT_REFLECTION_NAME_PREFIX "PushConstant"
#define RAZIX_PUSH_CONSTANT_REFLECTION_NAME_VK     RAZIX_PUSH_CONSTANT_REFLECTION_NAME_PREFIX
#define RAZIX_PUSH_CONSTANT_REFLECTION_NAME_DX12   "PushConstantBuffer"

#define RAZIX_EXTENTS_ELEM_COUNT 2
#define RAZIX_X(v)               ((v)[0])
#define RAZIX_Y(v)               ((v)[1])
#define RAZIX_Z(v)               ((v)[2])
#define RAZIX_W(v)               ((v)[3])

// Resource
#define RAZIX_GFX_RESOURCE rz_gfx_resource resource

    //---------------------------------------------------------------------------------------------
    // GFX/RHI types

    typedef enum rz_render_api
    {
        RZ_RENDER_API_NONE = -1,
        RZ_RENDER_API_VULKAN,       // Windows/MacOS/Linux
        RZ_RENDER_API_D3D12,        // [WIP] // PC/XBOX
        RZ_RENDER_API_GXM,          // Not Supported yet! (PSVita)
        RZ_RENDER_API_GCM,          // Not Supported yet! (PS3)
        RZ_RENDER_API_AGC,          // Not Supported yet! (PlayStation 5)
        RZ_RENDER_API_COUNT = 2,    // Only the supported ones
    } rz_render_api;

    /**
     * Resource view hints provide a way to add hints to the resource view creation as per backend requirements.
     * Actual resources can have multiple views, each with different hints.
     * Resource Views are manually managed and destroyed.
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
        RZ_GFX_RESOURCE_VIEW_FLAG_COUNT        = 8,
    } rz_gfx_resource_view_hints;

    typedef enum rz_gfx_resource_type
    {
        RZ_GFX_RESOURCE_TYPE_INVALID = 0,
        RZ_GFX_RESOURCE_TYPE_RESOURCE_VIEW,
        RZ_GFX_RESOURCE_TYPE_TEXTURE,
        RZ_GFX_RESOURCE_TYPE_BUFFER,
        RZ_GFX_RESOURCE_TYPE_SAMPLER,
        RZ_GFX_RESOURCE_TYPE_SHADER,
        RZ_GFX_RESOURCE_TYPE_DESCRIPTOR_HEAP,
        RZ_GFX_RESOURCE_TYPE_DESCRIPTOR_TABLE,
        RZ_GFX_RESOURCE_TYPE_ROOT_SIGNATURE,
        RZ_GFX_RESOURCE_TYPE_CMD_POOL,
        RZ_GFX_RESOURCE_TYPE_CMD_BUFFER,
        RZ_GFX_RESOURCE_TYPE_PIPELINE,
        RZ_GFX_RESOURCE_TYPE_COUNT
    } rz_gfx_resource_type;

    typedef enum rz_gfx_syncobj_type
    {
        RZ_GFX_SYNCOBJ_TYPE_CPU,
        RZ_GFX_SYNCOBJ_TYPE_GPU,
        RZ_GFX_SYNCOBJ_TYPE_TIMELINE,
        RZ_GFX_SYNCOBJ_TYPE_COUNT
    } rz_gfx_syncobj_type;

    typedef enum rz_gfx_format
    {
        RZ_GFX_FORMAT_UNDEFINED = 0,

        // Unsigned 8-bit
        RZ_GFX_FORMAT_R8_UNORM,
        RZ_GFX_FORMAT_R8_UINT,

        // 16-bit
        RZ_GFX_FORMAT_R16_UNORM,
        RZ_GFX_FORMAT_R16_FLOAT,
        RZ_GFX_FORMAT_R16G16_FLOAT,
        RZ_GFX_FORMAT_R16G16_UNORM,
        RZ_GFX_FORMAT_R16G16B16A16_UNORM,
        RZ_GFX_FORMAT_R16G16B16A16_FLOAT,

        // 32-bit
        RZ_GFX_FORMAT_R32_SINT,
        RZ_GFX_FORMAT_R32_UINT,
        RZ_GFX_FORMAT_R32_FLOAT,
        RZ_GFX_FORMAT_R32G32_SINT,
        RZ_GFX_FORMAT_R32G32_UINT,
        RZ_GFX_FORMAT_R32G32_FLOAT,
        RZ_GFX_FORMAT_R32G32B32_SINT,
        RZ_GFX_FORMAT_R32G32B32_UINT,
        RZ_GFX_FORMAT_R32G32B32_FLOAT,
        RZ_GFX_FORMAT_R32G32B32A32_SINT,
        RZ_GFX_FORMAT_R32G32B32A32_UINT,
        RZ_GFX_FORMAT_R32G32B32A32_FLOAT,

        // Packed formats
        RZ_GFX_FORMAT_R11G11B10_FLOAT,
        RZ_GFX_FORMAT_R11G11B10_UINT,

        // Color formats
        RZ_GFX_FORMAT_R8G8_UNORM,
        RZ_GFX_FORMAT_R8G8B8_UNORM,
        RZ_GFX_FORMAT_R8G8B8A8_UNORM,
        RZ_GFX_FORMAT_R8G8B8A8_SRGB,
        RZ_GFX_FORMAT_B8G8R8A8_UNORM,
        RZ_GFX_FORMAT_B8G8R8A8_SRGB,

        // Legacy / placeholder
        RZ_GFX_FORMAT_RGB8_UNORM,
        RZ_GFX_FORMAT_RGB16_UNORM,
        RZ_GFX_FORMAT_RGB32_UINT,
        RZ_GFX_FORMAT_RGBA,
        RZ_GFX_FORMAT_RGB,

        // Depth-stencil
        RZ_GFX_FORMAT_D16_UNORM,
        RZ_GFX_FORMAT_D24_UNORM_S8_UINT,
        RZ_GFX_FORMAT_D32_FLOAT,
        RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT,
        RZ_GFX_FORMAT_STENCIL8,

        // Pseudo format for swapchain screen output
        RZ_GFX_FORMAT_SCREEN,    // Basically B8G8R8A8_UNORM

        // Block formats
        RZ_GFX_FORMAT_BC1_RGBA_UNORM,
        RZ_GFX_FORMAT_BC3_RGBA_UNORM,
        RZ_GFX_FORMAT_BC6_UNORM,    // HDR
        RZ_GFX_FORMAT_BC7_UNORM,
        RZ_GFX_FORMAT_BC7_SRGB,

        RZ_GFX_FORMAT_COUNT
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
        RZ_GFX_TEXTURE_TYPE_CUBE_ARRAY,
        RZ_GFX_TEXTURE_TYPE_COUNT
    } rz_gfx_texture_type;

    typedef enum rz_gfx_cmdpool_type
    {
        RZ_GFX_CMDPOOL_TYPE_GRAPHICS,
        RZ_GFX_CMDPOOL_TYPE_COMPUTE,
        RZ_GFX_CMDPOOL_TYPE_TRANSFER,
        RZ_GFX_CMDPOOL_TYPE_COUNT
    } rz_gfx_cmdpool_type;

    typedef enum rz_gfx_resolution
    {
        RZ_GFX_RESOLUTION_1080p,       /* native HD resolution 1920x1080 rendering                            */
        RZ_GFX_RESOLUTION_1440p,       /* native 2K resolution 2560x1440 rendering                            */
        RZ_GFX_RESOLUTION_4K_UPSCALED, /* Upscaled using FSR/DLSS                                             */
        RZ_GFX_RESOLUTION_4K_NATIVE,   /* native 3840x2160 rendering                                          */
        RZ_GFX_RESOLUTION_WINDOW,      /* Selects the resolution dynamically based on the presentation window */
        RZ_GFX_RESOLUTION_CUSTOM,      /* Custom resolution for rendering                                     */
        RZ_GFX_RESOLUTION_COUNT
    } rz_gfx_resolution;

    typedef enum rz_gfx_resource_state
    {
        RZ_GFX_RESOURCE_STATE_UNDEFINED = 0,
        RZ_GFX_RESOURCE_STATE_COMMON,
        RZ_GFX_RESOURCE_STATE_GENERIC_READ,
        RZ_GFX_RESOURCE_STATE_RENDER_TARGET,
        RZ_GFX_RESOURCE_STATE_DEPTH_WRITE,
        RZ_GFX_RESOURCE_STATE_DEPTH_READ,
        RZ_GFX_RESOURCE_STATE_SHADER_READ,
        RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS,
        RZ_GFX_RESOURCE_STATE_COPY_SRC,
        RZ_GFX_RESOURCE_STATE_COPY_DST,
        RZ_GFX_RESOURCE_STATE_PRESENT,
        RZ_GFX_RESOURCE_STATE_VERTEX_BUFFER,
        RZ_GFX_RESOURCE_STATE_INDEX_BUFFER,
        RZ_GFX_RESOURCE_STATE_CONSTANT_BUFFER,
        RZ_GFX_RESOURCE_STATE_INDIRECT_ARGUMENT,
        RZ_GFX_RESOURCE_STATE_RESOLVE_SRC,
        RZ_GFX_RESOURCE_STATE_RESOLVE_DST,
        RZ_GFX_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
        RZ_GFX_RESOURCE_STATE_SHADING_RATE_SOURCE,
        RZ_GFX_RESOURCE_STATE_VIDEO_DECODE_READ,
        RZ_GFX_RESOURCE_STATE_VIDEO_DECODE_WRITE,
        RZ_GFX_RESOURCE_STATE_COUNT
    } rz_gfx_resource_state;

    typedef enum rz_gfx_clear_color_preset
    {
        RZ_GFX_CLEAR_COLOR_PRESET_OPAQUE_BLACK = 0,
        RZ_GFX_CLEAR_COLOR_PRESET_OPAQUE_WHITE,
        RZ_GFX_CLEAR_COLOR_PRESET_TRANSPARENT_BLACK,
        RZ_GFX_CLEAR_COLOR_PRESET_TRANSPARENT_WHITE,
        RZ_GFX_CLEAR_COLOR_PRESET_PINK,
        RZ_GFX_CLEAR_COLOR_PRESET_DEPTH_ZERO_TO_ONE,
        RZ_GFX_CLEAR_COLOR_PRESET_DEPTH_ONE_TO_ZERO,
        RZ_GFX_CLEAR_COLOR_PRESET_COUNT
    } rz_gfx_clear_color_preset;

    typedef enum rz_gfx_descriptor_type
    {
        RZ_GFX_DESCRIPTOR_TYPE_NONE            = 0xFFFFFFFF,
        RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER = 0,
        RZ_GFX_DESCRIPTOR_TYPE_PUSH_CONSTANT,
        RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED,    // (Vulkan-only, not recommended)
        RZ_GFX_DESCRIPTOR_TYPE_TEXTURE,
        RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE,
        RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE,
        RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE,
        RZ_GFX_DESCRIPTOR_TYPE_SAMPLER,
        RZ_GFX_DESCRIPTOR_TYPE_RW_TYPED,
        RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED,
        RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED,
        RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS,
        RZ_GFX_DESCRIPTOR_TYPE_RW_BYTE_ADDRESS,
        RZ_GFX_DESCRIPTOR_TYPE_APPEND_STRUCTURED,
        RZ_GFX_DESCRIPTOR_TYPE_CONSUME_STRUCTURED,
        RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED_COUNTER,
        RZ_GFX_DESCRIPTOR_TYPE_RT_ACCELERATION_STRUCTURE,
        RZ_GFX_DESCRIPTOR_TYPE_COUNT
    } rz_gfx_descriptor_type;

    typedef enum rz_gfx_descriptor_heap_type
    {
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV = 0,
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_SAMPLER,
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_RTV,
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_DSV,
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_COUNT
    } rz_gfx_descriptor_heap_type;

    typedef enum rz_gfx_descriptor_heap_flags
    {
        RZ_GFX_DESCRIPTOR_HEAP_FLAG_NONE                        = 0,
        RZ_GFX_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE              = 1 << 0,
        RZ_GFX_DESCRIPTOR_HEAP_FLAG_CPU_ONLY_VISIBLE            = 1 << 1,
        RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST   = 1 << 2,
        RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_RINGBUFFER = 1 << 3,
        RZ_GFX_DESCRIPTOR_HEAP_FLAG_COUNT                       = 5
    } rz_gfx_descriptor_heap_flags;

    typedef enum rz_gfx_shader_stage
    {
        RZ_GFX_SHADER_STAGE_NONE                    = 0,
        RZ_GFX_SHADER_STAGE_VERTEX                  = 1 << 0,
        RZ_GFX_SHADER_STAGE_GEOMETRY                = 1 << 1,
        RZ_GFX_SHADER_STAGE_TESSELLATION_CONTROL    = 1 << 2,
        RZ_GFX_SHADER_STAGE_TESSELLATION_EVALUATION = 1 << 3,
        RZ_GFX_SHADER_STAGE_PIXEL                   = 1 << 4,
        RZ_GFX_SHADER_STAGE_COMPUTE                 = 1 << 5,
        RZ_GFX_SHADER_STAGE_RAY_GEN                 = 1 << 6,
        RZ_GFX_SHADER_STAGE_RAY_MISS                = 1 << 7,
        RZ_GFX_SHADER_STAGE_RAY_CLOSEST_HIT         = 1 << 8,
        RZ_GFX_SHADER_STAGE_RAY_ANY_HIT             = 1 << 9,
        RZ_GFX_SHADER_STAGE_RAY_CALLABLE            = 1 << 10,
        RZ_GFX_SHADER_STAGE_TASK                    = 1 << 11,
        RZ_GFX_SHADER_STAGE_MESH                    = 1 << 12,
        RZ_GFX_SHADER_STAGE_COUNT                   = 13
    } rz_gfx_shader_stage;

    typedef enum rz_gfx_shader_data_type
    {
        RZ_GFX_SHADER_DATA_TYPE_NONE = 0,
        RZ_GFX_SHADER_DATA_TYPE_FLOAT32,
        RZ_GFX_SHADER_DATA_TYPE_VEC2,
        RZ_GFX_SHADER_DATA_TYPE_VEC3,
        RZ_GFX_SHADER_DATA_TYPE_VEC4,
        RZ_GFX_SHADER_DATA_TYPE_IVEC2,
        RZ_GFX_SHADER_DATA_TYPE_IVEC3,
        RZ_GFX_SHADER_DATA_TYPE_IVEC4,
        RZ_GFX_SHADER_DATA_TYPE_MAT3,
        RZ_GFX_SHADER_DATA_TYPE_MAT4,
        RZ_GFX_SHADER_DATA_TYPE_INT32,
        RZ_GFX_SHADER_DATA_TYPE_INT,
        RZ_GFX_SHADER_DATA_TYPE_UINT,
        RZ_GFX_SHADER_DATA_TYPE_BOOL,
        RZ_GFX_SHADER_DATA_TYPE_STRUCT,
        RZ_GFX_SHADER_DATA_TYPE_MAT4_ARRAY,
        RZ_GFX_SHADER_DATA_TYPE_COUNT
    } rz_gfx_shader_data_type;

    typedef enum rz_gfx_texture_address_mode
    {
        RZ_GFX_TEXTURE_ADDRESS_MODE_WRAP = 0,
        RZ_GFX_TEXTURE_ADDRESS_MODE_REPEAT,
        RZ_GFX_TEXTURE_ADDRESS_MODE_CLAMP,
        RZ_GFX_TEXTURE_ADDRESS_MODE_BORDER,
        RZ_GFX_TEXTURE_ADDRESS_MODE_COUNT
    } rz_gfx_texture_address_mode;

    typedef enum rz_gfx_texture_filter_type
    {
        RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST = 0,
        RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR,
        RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_NEAREST,
        RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_NEAREST,
        RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_LINEAR,
        RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR,
        RZ_GFX_TEXTURE_FILTER_TYPE_COUNT
    } rz_gfx_texture_filter_type;

    typedef enum rz_gfx_pipeline_type
    {
        RZ_GFX_PIPELINE_TYPE_GRAPHICS = 0,
        RZ_GFX_PIPELINE_TYPE_COMPUTE,
        RZ_GFX_PIPELINE_TYPE_RAYTRACING,
        RZ_GFX_PIPELINE_TYPE_COUNT
    } rz_gfx_pipeline_type;

    typedef enum rz_gfx_cull_mode_type
    {
        RZ_GFX_CULL_MODE_TYPE_BACK = 0,
        RZ_GFX_CULL_MODE_TYPE_FRONT,
        RZ_GFX_CULL_MODE_TYPE_FRONT_BACK,
        RZ_GFX_CULL_MODE_TYPE_NONE,
        RZ_GFX_CULL_MODE_TYPE_COUNT
    } rz_gfx_cull_mode_type;

    typedef enum rz_gfx_polygon_mode_type
    {
        RZ_GFX_POLYGON_MODE_TYPE_SOLID = 0,
        RZ_GFX_POLYGON_MODE_TYPE_WIREFRAME,
        RZ_GFX_POLYGON_MODE_TYPE_POINT,
        RZ_GFX_POLYGON_MODE_TYPE_COUNT
    } rz_gfx_polygon_mode_type;

    typedef enum rz_gfx_draw_type
    {
        RZ_GFX_DRAW_TYPE_POINT = 0,
        RZ_GFX_DRAW_TYPE_TRIANGLE,
        RZ_GFX_DRAW_TYPE_TRIANGLE_STRIP,
        RZ_GFX_DRAW_TYPE_LINE,
        RZ_GFX_DRAW_TYPE_LINE_STRIP,
        RZ_GFX_DRAW_TYPE_COUNT
    } rz_gfx_draw_type;

    typedef enum rz_gfx_blend_op_type
    {
        RZ_GFX_BLEND_OP_TYPE_ADD = 0,
        RZ_GFX_BLEND_OP_TYPE_SUBTRACT,
        RZ_GFX_BLEND_OP_TYPE_REVERSE_SUBTRACT,
        RZ_GFX_BLEND_OP_TYPE_MIN,
        RZ_GFX_BLEND_OP_TYPE_MAX,
        RZ_GFX_BLEND_OP_TYPE_COUNT
    } rz_gfx_blend_op_type;

    typedef enum rz_gfx_blend_factor_type
    {
        RZ_GFX_BLEND_FACTOR_TYPE_ZERO = 0,
        RZ_GFX_BLEND_FACTOR_TYPE_ONE,
        RZ_GFX_BLEND_FACTOR_TYPE_SRC_COLOR,
        RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_SRC_COLOR,
        RZ_GFX_BLEND_FACTOR_TYPE_DST_COLOR,
        RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_DST_COLOR,
        RZ_GFX_BLEND_FACTOR_TYPE_SRC_ALPHA,
        RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_SRC_ALPHA,
        RZ_GFX_BLEND_FACTOR_TYPE_DST_ALPHA,
        RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_DST_ALPHA,
        RZ_GFX_BLEND_FACTOR_TYPE_CONSTANT_COLOR,
        RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_CONSTANT_COLOR,
        RZ_GFX_BLEND_FACTOR_TYPE_CONSTANT_ALPHA,
        RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_CONSTANT_ALPHA,
        RZ_GFX_BLEND_FACTOR_TYPE_SRC_ALPHA_SATURATE,
        RZ_GFX_BLEND_FACTOR_TYPE_COUNT
    } rz_gfx_blend_factor_type;

    typedef enum rz_gfx_compare_op_type
    {
        RZ_GFX_COMPARE_OP_TYPE_NEVER = 0,
        RZ_GFX_COMPARE_OP_TYPE_LESS,
        RZ_GFX_COMPARE_OP_TYPE_EQUAL,
        RZ_GFX_COMPARE_OP_TYPE_LESS_OR_EQUAL,
        RZ_GFX_COMPARE_OP_TYPE_GREATER,
        RZ_GFX_COMPARE_OP_TYPE_NOT_EQUAL,
        RZ_GFX_COMPARE_OP_TYPE_GREATER_OR_EQUAL,
        RZ_GFX_COMPARE_OP_TYPE_ALWAYS,
        RZ_GFX_COMPARE_OP_TYPE_COUNT
    } rz_gfx_compare_op_type;

    typedef enum rz_gfx_buffer_usage_type
    {
        RZ_GFX_BUFFER_USAGE_TYPE_STATIC = 0,
        RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC,
        RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM,
        RZ_GFX_BUFFER_USAGE_TYPE_STAGING,
        RZ_GFX_BUFFER_USAGE_TYPE_READBACK,
        RZ_GFX_BUFFER_USAGE_TYPE_COUNT
    } rz_gfx_buffer_usage_type;

    typedef enum rz_gfx_buffer_type
    {
        RZ_GFX_BUFFER_TYPE_CONSTANT = 0,
        RZ_GFX_BUFFER_TYPE_VERTEX,
        RZ_GFX_BUFFER_TYPE_INDEX,
        RZ_GFX_BUFFER_TYPE_INDIRECT_ARGS,
        RZ_GFX_BUFFER_TYPE_STRUCTURED,
        RZ_GFX_BUFFER_TYPE_BYTE,
        RZ_GFX_BUFFER_TYPE_ACCELERATION_STRUCTURE,
        RZ_GFX_BUFFER_TYPE_RW_STRUCTURED,
        RZ_GFX_BUFFER_TYPE_RW_BYTE,
        RZ_GFX_BUFFER_TYPE_COUNT
    } rz_gfx_buffer_type;

    typedef enum rz_gfx_pipeline_stage
    {
        RZ_GFX_PIPELINE_STAGE_TOP_OF_PIPE = 0,
        RZ_GFX_PIPELINE_STAGE_DRAW_INDIRECT,
        RZ_GFX_PIPELINE_STAGE_DRAW,
        RZ_GFX_PIPELINE_STAGE_VERTEX_INPUT,
        RZ_GFX_PIPELINE_STAGE_VERTEX_SHADER,
        RZ_GFX_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER,
        RZ_GFX_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER,
        RZ_GFX_PIPELINE_STAGE_GEOMETRY_SHADER,
        RZ_GFX_PIPELINE_STAGE_FRAGMENT_SHADER,
        RZ_GFX_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS,
        RZ_GFX_PIPELINE_STAGE_LATE_FRAGMENT_TESTS,
        RZ_GFX_PIPELINE_STAGE_EARLY_OR_LATE_TESTS,
        RZ_GFX_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT,
        RZ_GFX_PIPELINE_STAGE_COMPUTE_SHADER,
        RZ_GFX_PIPELINE_STAGE_TRANSFER,
        RZ_GFX_PIPELINE_STAGE_MESH_SHADER,
        RZ_GFX_PIPELINE_STAGE_TASK_SHADER,
        RZ_GFX_PIPELINE_STAGE_BOTTOM_OF_PIPE,
        RZ_GFX_PIPELINE_STAGE_COUNT
    } rz_gfx_pipeline_stage;

    typedef enum rz_gfx_memory_access_mask
    {
        RZ_GFX_MEMORY_ACCESS_NONE = 0,
        RZ_GFX_MEMORY_ACCESS_INDIRECT_COMMAND_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_INDEX_BUFFER_DATA_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_UNIFORM_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_INPUT_ATTACHMENT_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_SHADER_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_SHADER_WRITE_BIT,
        RZ_GFX_MEMORY_ACCESS_COLOR_ATTACHMENT_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        RZ_GFX_MEMORY_ACCESS_COLOR_ATTACHMENT_READ_WRITE_BIT,
        RZ_GFX_MEMORY_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        RZ_GFX_MEMORY_ACCESS_TRANSFER_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_TRANSFER_WRITE_BIT,
        RZ_GFX_MEMORY_ACCESS_HOST_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_HOST_WRITE_BIT,
        RZ_GFX_MEMORY_ACCESS_MEMORY_READ_BIT,
        RZ_GFX_MEMORY_ACCESS_MEMORY_WRITE_BIT,
        RZ_GFX_MEMORY_ACCESS_MASK_COUNT
    } rz_gfx_memory_access_mask;

    typedef enum rz_gfx_draw_data_type
    {
        RZ_GFX_DRAW_DATA_TYPE_FLOAT = 0,
        RZ_GFX_DRAW_DATA_TYPE_UNSIGNED_INT,
        RZ_GFX_DRAW_DATA_TYPE_UNSIGNED_BYTE,
        RZ_GFX_DRAW_DATA_TYPE_COUNT
    } rz_gfx_draw_data_type;

    typedef enum rz_gfx_blend_presets
    {
        RZ_GFX_BLEND_PRESET_ADDITIVE = 0,
        RZ_GFX_BLEND_PRESET_ALPHA_BLEND,
        RZ_GFX_BLEND_PRESET_SUBTRACTIVE,
        RZ_GFX_BLEND_PRESET_MULTIPLY,
        RZ_GFX_BLEND_PRESET_DARKEN,
        RZ_GFX_BLEND_PRESET_COUNT
    } rz_gfx_blend_presets;

    typedef enum rz_gfx_shader_flags
    {
        RZ_GFX_SGADER_FLAG_NONE              = 0,
        RZ_GFX_SHADER_FLAG_BINDLESS          = 1 << 0,
        RZ_GFX_SHADER_FLAG_NO_ROOT_SIGNATURE = 1 << 1,
        RZ_GFX_SHADER_FLAG_INPUT_LAYOUT_SOA  = 1 << 2,
        RZ_GFX_SHADER_FLAG_COUNT             = 3
    } rz_gfx_shader_flags;

    typedef enum rz_gfx_target_fps
    {
        RZ_GFX_TARGET_FPS_60    = 60,
        RZ_GFX_TARGET_FPS_120   = 120,
        RZ_GFX_TARGET_FPS_COUNT = 2
    } rz_gfx_target_fps;

    typedef enum rz_gfx_semantic
    {
        RZ_GFX_SEMANTIC_POSITION = 0,
        RZ_GFX_SEMANTIC_NORMAL,
        RZ_GFX_SEMANTIC_TANGENT,
        RZ_GFX_SEMANTIC_BITANGENT,
        RZ_GFX_SEMANTIC_COLOR0,
        RZ_GFX_SEMANTIC_COLOR1,
        RZ_GFX_SEMANTIC_COLOR2,
        RZ_GFX_SEMANTIC_COLOR3,
        RZ_GFX_SEMANTIC_TEXCOORD0,
        RZ_GFX_SEMANTIC_TEXCOORD1,
        RZ_GFX_SEMANTIC_TEXCOORD2,
        RZ_GFX_SEMANTIC_TEXCOORD3,
        RZ_GFX_SEMANTIC_TEXCOORD4,
        RZ_GFX_SEMANTIC_TEXCOORD5,
        RZ_GFX_SEMANTIC_TEXCOORD6,
        RZ_GFX_SEMANTIC_TEXCOORD7,
        RZ_GFX_SEMANTIC_BONE_INDICES,
        RZ_GFX_SEMANTIC_BONE_WEIGHTS,
        RZ_GFX_SEMANTIC_INSTANCE_ID,
        RZ_GFX_SEMANTIC_CUSTOM0,
        RZ_GFX_SEMANTIC_CUSTOM1,
        RZ_GFX_SEMANTIC_CUSTOM2,
        RZ_GFX_SEMANTIC_CUSTOM3,
        RZ_GFX_SEMANTIC_COUNT
    } rz_gfx_semantic;

    typedef enum rz_gfx_input_class
    {
        RZ_GFX_INPUT_CLASS_PER_VERTEX   = 0,
        RZ_GFX_INPUT_CLASS_PER_INSTANCE = 1,
        RZ_GFX_INPUT_CLASS_COUNT
    } rz_gfx_input_class;

    typedef enum rz_gfx_index_type
    {
        RZ_GFX_INDEX_TYPE_UINT16 = 0,
        RZ_GFX_INDEX_TYPE_UINT32,
        RZ_GFX_INDEX_TYPE_COUNT
    } rz_gfx_index_type;

    /**
      * Graphics Features as supported by the GPU, even though Engine supports them
      * the GPU can override certain setting and query run-time info like LaneWidth etc.
      */
    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_features
    {
        union gfxsupport
        {
            uint32_t value;
            struct
            {
                uint32_t TesselateTerrain : 1;
                uint32_t WaveIntrinsics : 1;
                uint32_t ShaderModel6 : 1;
                uint32_t NullIndexDescriptors : 1;
                uint32_t TimelineSemaphores : 1;
                uint32_t BindlessRendering : 1;
                uint32_t reserved : 26;
            };
        } support;
        uint32_t MaxBindlessTextures;
        uint32_t MinLaneWidth;
        uint32_t MaxLaneWidth;
        uint8_t  _pad0[4];
    } rz_gfx_features;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_color_rgba
    {
        union
        {
            struct
            {
                float r;
                float g;
                float b;
                float a;
            };
            float raw[4];
        };
    } rz_gfx_color_rgba;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_color_rgb
    {
        union
        {
            struct
            {
                float r;
                float g;
                float b;
            };
            float raw[3];
        };
        uint8_t _pad0[4];
    } rz_gfx_color_rgb;

    typedef void (*rz_gfx_resource_create_fn)(void* where);
    typedef void (*rz_gfx_resource_destroy_fn)(void* resource);

    typedef rz_handle rz_gfx_resource_view_handle;
    typedef rz_handle rz_gfx_texture_handle;
    typedef rz_handle rz_gfx_sampler_handle;
    typedef rz_handle rz_gfx_buffer_handle;
    typedef rz_handle rz_gfx_cmdbuf_handle;
    typedef rz_handle rz_gfx_root_signature_handle;
    typedef rz_handle rz_gfx_shader_handle;
    typedef rz_handle rz_gfx_swapchain_handle;
    typedef rz_handle rz_gfx_syncobj_handle;
    typedef rz_handle rz_gfx_cmdpool_handle;
    typedef rz_handle rz_gfx_descriptor_heap_handle;
    typedef rz_handle rz_gfx_descriptor_table_handle;
    typedef rz_handle rz_gfx_pipeline_handle;

    // Forward Declarations
    typedef struct rz_gfx_resource_view    rz_gfx_resource_view;
    typedef struct rz_gfx_texture          rz_gfx_texture;
    typedef struct rz_gfx_sampler          rz_gfx_sampler;
    typedef struct rz_gfx_buffer           rz_gfx_buffer;
    typedef struct rz_gfx_cmdbuf           rz_gfx_cmdbuf;
    typedef struct rz_gfx_root_signature   rz_gfx_root_signature;
    typedef struct rz_gfx_shader           rz_gfx_shader;
    typedef struct rz_gfx_syncobj          rz_gfx_syncobj;
    typedef struct rz_gfx_cmdpool          rz_gfx_cmdpool;
    typedef struct rz_gfx_descriptor_heap  rz_gfx_descriptor_heap;
    typedef struct rz_gfx_descriptor_table rz_gfx_descriptor_table;
    typedef struct rz_gfx_pipeline         rz_gfx_pipeline;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_buffer_view_desc
    {
        const rz_gfx_buffer* pBuffer;
        uint64_t             offset;
        uint64_t             size;
        uint32_t             stride;
        rz_gfx_format        format;    // Format of the buffer, used for structured buffers
    } rz_gfx_buffer_view_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_texture_view_desc
    {
        const rz_gfx_texture* pTexture;
        uint32_t              baseMip;
        uint32_t              baseArrayLayer;
        uint32_t              dimension;
        uint32_t              _pad0[3];
    } rz_gfx_texture_view_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_sampler_view_desc
    {
        const rz_gfx_sampler* pSampler;
    } rz_gfx_sampler_view_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_resource_view_desc
    {
        rz_gfx_descriptor_type  descriptorType;
        rz_gfx_descriptor_heap* pRtvDsvHeap;    // Only use it to create RTV/DSV immediately
        // We could the same for samplers too, but unless we use Immutable or Bindless Sampler, we need a descriptor table to bind samplers
        union
        {
            rz_gfx_buffer_view_desc  bufferViewDesc;
            rz_gfx_texture_view_desc textureViewDesc;
            rz_gfx_sampler_view_desc samplerViewDesc;    // Basically the sampler object itself
        };
    } rz_gfx_resource_view_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_texture_desc
    {
        uint32_t                   width;
        uint32_t                   height;
        uint32_t                   depth;
        uint32_t                   mipLevels;
        uint32_t                   arraySize;
        rz_gfx_format              format;
        rz_gfx_texture_type        textureType;
        rz_gfx_resource_view_hints resourceHints;    // Hints for how this texture should be viewed, used for binding
        void*                      pPixelData;       // Pointer to the pixel data, used for texture creation, freed by user, RHI does not own this memory
    } rz_gfx_texture_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_sampler_desc
    {
        rz_gfx_texture_filter_type  minFilter;
        rz_gfx_texture_filter_type  magFilter;
        rz_gfx_texture_filter_type  mipFilter;
        rz_gfx_texture_address_mode addressModeU;
        rz_gfx_texture_address_mode addressModeV;
        rz_gfx_texture_address_mode addressModeW;
        uint32_t                    maxAnisotropy;
        rz_gfx_compare_op_type      compareOp;
        float                       mipLODBias;
        float                       minLod;
        float                       maxLod;
        uint32_t                    _pad0;
    } rz_gfx_sampler_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_binding_location
    {
        uint32_t binding;
        uint32_t space;
        uint8_t  _pad0[8];
    } rz_gfx_binding_location;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_descriptor
    {
        const char*             pName;
        rz_gfx_descriptor_type  type;
        uint32_t                sizeInBytes;
        rz_gfx_binding_location location;
        uint32_t                offsetInBytes;
        uint32_t                memberCount;
        uint32_t                arraySize;
        uint8_t                 _pad0[4];
    } rz_gfx_descriptor;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_descriptor_table_desc
    {
        uint32_t                tableIndex;
        rz_gfx_resource_view*   pResourceViews;    // Only for temporary table creation, we cache the handles that the user can free during descriptor table destruction
        uint32_t                resourceViewsCount;
        rz_gfx_descriptor_heap* pHeap;
    } rz_gfx_descriptor_table_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_descriptor_table_layout
    {
        rz_gfx_descriptor* pDescriptors;
        uint32_t           descriptorCount;
        uint32_t           tableIndex;
    } rz_gfx_descriptor_table_layout;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_root_constant_desc
    {
        rz_gfx_binding_location location;
        uint32_t                sizeInBytes;
        uint32_t                offsetInBytes;
        rz_gfx_shader_stage     shaderStage;
        uint8_t                 _pad0[4];
    } rz_gfx_root_constant_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_root_signature_desc
    {
        rz_gfx_descriptor_table_layout* pDescriptorTableLayouts;
        rz_gfx_root_constant_desc*      pRootConstantsDesc;
        uint32_t                        descriptorTableLayoutsCount;
        uint32_t                        rootConstantCount;
        uint8_t                         _pad0[8];
    } rz_gfx_root_signature_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_cmdpool_desc
    {
        rz_gfx_cmdpool_type poolType;
    } rz_gfx_cmdpool_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_cmdbuf_desc
    {
        const rz_gfx_cmdpool* pool;
        uint8_t               _pad0[8];
    } rz_gfx_cmdbuf_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_buffer_desc
    {
        rz_gfx_buffer_type         type;
        rz_gfx_buffer_usage_type   usage;
        rz_gfx_resource_view_hints resourceHints;    // Hints for how this texture should be viewed, used for binding
        uint32_t                   sizeInBytes;
        uint32_t                   stride;          // For structured buffers
        uint32_t                   elementCount;    // For structured buffers
        const void*                pInitData;       // Pointer to the initial data, used for buffer creation, freed by user, RHI does not own this memory
        uint8_t                    _pad0[4];
    } rz_gfx_buffer_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_shader_stage_blob
    {
        rz_gfx_shader_stage stage;
        uint32_t            size;
        const char*         bytecode;
    } rz_gfx_shader_stage_blob;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_input_element
    {
        const char*        pSemanticName;
        uint32_t           semanticIndex : 5;
        rz_gfx_format      format : 6;
        uint32_t           inputSlot : 4;
        uint32_t           alignedByteOffset : 8;
        rz_gfx_input_class inputClass : 1;
        uint32_t           instanceStepRate : 8;
        uint32_t           _pad0;
    } rz_gfx_input_element;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_shader_desc
    {
        rz_gfx_pipeline_type  pipelineType;
        uint32_t              elementsCount;
        rz_gfx_input_element* pElements;
        uint32_t              flags;
        uint32_t              _pad0[3];
        union
        {
            const char* rzsfFilePath;    // PIGGY_BACKING_MEMORY: helper member to re-use this union, used before shader blobs are filled, safe to use here since freed before it reaches RHI
            struct
            {
                rz_gfx_shader_stage_blob vs;
                rz_gfx_shader_stage_blob ps;
                rz_gfx_shader_stage_blob gs;
                rz_gfx_shader_stage_blob tcs;
                rz_gfx_shader_stage_blob tes;
                rz_gfx_shader_stage_blob task;
                rz_gfx_shader_stage_blob mesh;
            } raster;

            struct
            {
                rz_gfx_shader_stage_blob cs;
            } compute;

            struct
            {
                rz_gfx_shader_stage_blob rgen;
                rz_gfx_shader_stage_blob miss;
                rz_gfx_shader_stage_blob chit;
                rz_gfx_shader_stage_blob ahit;
                rz_gfx_shader_stage_blob callable;
            } raytracing;
        };
    } rz_gfx_shader_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_descriptor_heap_desc
    {
        rz_gfx_descriptor_heap_type heapType;
        uint32_t                    descriptorCount;
        uint32_t                    flags;
        uint8_t                     _pad0[4];
    } rz_gfx_descriptor_heap_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_pipeline_desc
    {
        const rz_gfx_shader*         pShader;
        const rz_gfx_root_signature* pRootSig;

        uint32_t      renderTargetCount;
        rz_gfx_format depthStencilFormat;

        struct
        {
            uint32_t type : 2;
            uint32_t cullMode : 3;
            uint32_t polygonMode : 2;
            uint32_t depthWriteEnabled : 1;
            uint32_t depthTestEnabled : 1;
            uint32_t depthCompareOp : 3;
            uint32_t stencilTestEnabled : 1;
            uint32_t blendEnabled : 1;
            uint32_t rasterizerDiscardEnabled : 1;
            uint32_t primitiveRestartEnabled : 1;
            uint32_t drawType : 2;
            uint32_t useBlendPreset : 1;    // If set to true, use the blend preset, else use the blend factors below
            uint32_t reserved0 : 13;
        };

        union
        {
            struct
            {
                uint32_t blendPreset : 5;    // Use this to set the blend preset, will be used to fill the blend factors
                uint32_t reserved1 : 27;     // Reserved for future use, so we can expand the struct without breaking ABI
            };
            struct
            {
                uint32_t srcColorBlendFactor : 4;
                uint32_t dstColorBlendFactor : 4;
                uint32_t srcAlphaBlendFactor : 4;
                uint32_t dstAlphaBlendFactor : 4;
                uint32_t colorBlendOp : 3;
                uint32_t alphaBlendOp : 3;
                uint32_t stencilState : 3;    // Will overflow, so promote to next byte and to keep blend presets together
                uint32_t reserved2 : 7;       // padding to 32 bits
            };
        };
        rz_gfx_format renderTargetFormats[RAZIX_MAX_RENDER_TARGETS];
    } rz_gfx_pipeline_desc;

    /**
     * Why this design? embed desc?
     * 1. It allows us to simulate inheritance in C, where we can have a common base struct
     * 2. Helps with ResourceManager to use macro generation to create resources pool with a single common interface and also for RHI
     * 3. Simulate private members in C, so we can have public members that are filled by the user and private members that are filled by the backend
     * 4. Simulate hot and cold data separation, engine and RHI can use this to separate hot and cold data based on rz_gfx_xxx_desc separation, RHI will need data without rz_gfx_resource and desc
     */

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_resource
    {
        const char*                pName;
        rz_handle                  handle;
        rz_gfx_resource_view_hints viewHints;
        rz_gfx_resource_type       type;
        rz_gfx_resource_state      currentState;
        uint8_t                    _pad0[4];

        union
        {
            rz_gfx_resource_view_desc    resourceViewDesc;
            rz_gfx_texture_desc          textureDesc;
            rz_gfx_sampler_desc          samplerDesc;
            rz_gfx_cmdpool_desc          cmdpoolDesc;
            rz_gfx_cmdbuf_desc           cmdbufDesc;
            rz_gfx_root_signature_desc   rootSignatureDesc;
            rz_gfx_descriptor_heap_desc  descriptorHeapDesc;
            rz_gfx_descriptor_table_desc descriptorTableDesc;
            rz_gfx_buffer_desc           bufferDesc;
            rz_gfx_shader_desc           shaderDesc;    // Big boi!
            rz_gfx_pipeline_desc         pipelineDesc;
        } desc;    // These are filled by the user, public members filled by backend are stored in each gfx_type separately
    } rz_gfx_resource;

    //---------------------------------------------------------------------------------------------

    typedef void* (*rzRHI_malloc_fn)(size_t size, void* user);
    typedef void* (*rzRHI_calloc_fn)(size_t count, size_t size, void* user);
    typedef void* (*rzRHI_realloc_fn)(void* ptr, size_t new_size, void* user);
    typedef void (*rzRHI_free_fn)(void* ptr, void* user);

    typedef struct rz_alloc_callbacks
    {
        rzRHI_malloc_fn  malloc_fn;
        rzRHI_calloc_fn  calloc_fn;
        rzRHI_realloc_fn realloc_fn;
        rzRHI_free_fn    free_fn;
        void*            user;
    } rz_alloc_callbacks;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_context
    {
        uint32_t      width;
        uint32_t      height;
        uint32_t      frameIndex;
        rz_render_api renderAPI;
        // TODO: Store a ref to malloc/calloc/free C-style callback functions for RHI memory needs
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

    typedef uint64_t rz_gfx_syncpoint;

    struct RAZIX_RHI_ALIGN_16 rz_gfx_syncobj
    {
        rz_gfx_syncpoint    waitSyncpoint;
        rz_gfx_syncobj_type type;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_syncobj vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_syncobj dx12;
#endif
    };

    //---------------------------------------------------------------------------------------------
    // Resource Structs

    struct RAZIX_RHI_ALIGN_16 rz_gfx_resource_view
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_resview vk;
#endif
#if defined RAZIX_RENDER_API_DIRECTX12
        dx12_resview dx12;
#endif
    };

    struct RAZIX_RHI_ALIGN_16 rz_gfx_texture
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_texture vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_texture dx12;
#endif
    };

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_swapchain
    {
        uint32_t             width;
        uint32_t             height;
        uint32_t             imageCount;
        uint32_t             currBackBufferIdx;
        rz_gfx_texture       backbuffers[RAZIX_MAX_SWAP_IMAGES_COUNT];
        rz_gfx_resource_view backbuffersResViews[RAZIX_MAX_SWAP_IMAGES_COUNT];
#ifdef RAZIX_RENDER_API_VULKAN
        vk_swapchain vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_swapchain dx12;
#endif
    } rz_gfx_swapchain;

    struct RAZIX_RHI_ALIGN_16 rz_gfx_sampler
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_sampler vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_sampler dx12;
#endif
    };

    // Note:- Exception!, this is not a resource as it's managed by the Renderer and very few in number, might make is a Resource later
    struct RAZIX_RHI_ALIGN_16 rz_gfx_cmdpool
    {
        RAZIX_GFX_RESOURCE;
        rz_gfx_cmdpool_type type;
        union
        {
#ifdef RAZIX_RENDER_API_VULKAN
            vk_cmdpool vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            dx12_cmdpool dx12;
#endif
        };
    };

    struct RAZIX_RHI_ALIGN_16 rz_gfx_cmdbuf
    {
        RAZIX_GFX_RESOURCE;
        union
        {
#ifdef RAZIX_RENDER_API_VULKAN
            vk_cmdbuf vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            dx12_cmdbuf dx12;
#endif
        };
    };

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_descriptor_free_range
    {
        uint32_t start;
        uint32_t numDescriptors;
    } rz_gfx_descriptor_free_range;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_descriptor_freelist_allocator
    {
        uint32_t                      numFreeRanges;
        uint32_t                      capacity;
        rz_gfx_descriptor_free_range* freeRanges;
    } rz_gfx_descriptor_freelist_allocator;

    struct RAZIX_RHI_ALIGN_16 rz_gfx_descriptor_heap
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_descriptor_heap vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_descriptor_heap dx12;
#endif
        // TODO: Use a fence to track the descriptors while allocating and freeing them
        // Only needed if we are using it across multiple threads/workloads
        union
        {
            struct
            {
                rz_gfx_descriptor_freelist_allocator* freeListAllocator;
                uint32_t                              _pad0[2];
            };

            struct
            {
                uint32_t ringBufferHead;
                uint32_t ringBufferTail;
                uint32_t isFull;
                uint32_t _pad1;
            };
        };
    };

    struct RAZIX_RHI_ALIGN_16 rz_gfx_descriptor_table
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_descriptor_table vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_descriptor_table dx12;
#endif
    };

    struct RAZIX_RHI_ALIGN_16 rz_gfx_root_signature
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_root_signature vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_root_signature dx12;
#endif
    };

    struct RAZIX_RHI_ALIGN_16 rz_gfx_shader
    {
        RAZIX_GFX_RESOURCE;
        rz_gfx_root_signature_handle rootSignature;
        uint32_t                     shaderStageMask;
        union
        {
#ifdef RAZIX_RENDER_API_VULKAN
            vk_shader vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            dx12_shader dx12;
#endif
        };
    };

    struct RAZIX_RHI_ALIGN_16 rz_gfx_pipeline
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_pipeline vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_pipeline dx12;
#endif
    };

    struct RAZIX_RHI_ALIGN_16 rz_gfx_buffer
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_buffer vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_buffer dx12;
#endif
    };

    //---------------------------------------------------------------------------------------------

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_attachment
    {
        const rz_gfx_resource_view* pResourceView;
        rz_gfx_color_rgba           clearColor;
        struct
        {
            uint32_t clear : 1;
            uint32_t reserved : 31;
        };
        uint8_t _pad0[4];
    } gfx_attachment;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_viewport
    {
        int32_t  x;
        int32_t  y;
        uint32_t width, height;
        uint32_t minDepth;
        uint32_t maxDepth;
        uint8_t  _pad0[8];
    } rz_gfx_viewport;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_rect
    {
        int32_t  x;
        int32_t  y;
        uint32_t width, height;
    } rz_gfx_rect;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_renderpass
    {
        uint32_t          colorAttachmentsCount;
        uint32_t          _pad0;
        uint32_t          extents[RAZIX_EXTENTS_ELEM_COUNT];
        gfx_attachment    colorAttachments[RAZIX_MAX_RENDER_TARGETS];
        gfx_attachment    depthAttachment;
        uint32_t          layers;
        rz_gfx_resolution resolution;    // TODO: Use this
        uint8_t           _pad1[8];
    } rz_gfx_renderpass;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_shader_reflection
    {
        rz_gfx_root_signature_desc rootSignatureDesc;
        rz_gfx_input_element*      pInputElements;
        uint32_t                   elementCount;
        uint8_t                    _pad0[4];
    } rz_gfx_shader_reflection;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_buffer_update
    {
        const rz_gfx_buffer* pBuffer;
        uint32_t             sizeInBytes;
        uint32_t             offset;
        const void*          pData;
        uint8_t              _pad0[8];
    } rz_gfx_buffer_update;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_texture_readback
    {
        uint8_t* data;
        uint32_t width;
        uint32_t height;
        uint32_t bpp;
        uint8_t  _pad0[12];
    } rz_gfx_texture_readback;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_buffer_readback
    {
        uint8_t* data;
        uint32_t sizeInBytes;
        uint32_t offset;
        uint8_t  _pad0[6];
    } rz_gfx_buffer_readback;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_draw_indirect_args
    {
        uint32_t vertexCount;
        uint32_t instanceCount;
        uint32_t firstVertex;
        uint32_t firstInstance;
    } rz_gfx_draw_indirect_args;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_draw_indexed_indirect_args
    {
        uint32_t indexCount;
        uint32_t instanceCount;
        uint32_t firstIndex;
        int32_t  vertexOffset;
        uint32_t firstInstance;
    } rz_gfx_draw_indexed_indirect_args;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_dispatch_indirect_args
    {
        uint32_t threadGroupCountX;
        uint32_t threadGroupCountY;
        uint32_t threadGroupCountZ;
    } rz_gfx_dispatch_indirect_args;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_submit_desc
    {
        const rz_gfx_cmdbuf*  pCmdBufs;
        uint32_t              cmdCount;
        const rz_gfx_syncobj* pWaitSyncobjs;
        uint32_t              waitSyncobjCount;
        const rz_gfx_syncobj* pSignalSyncobjs;
        uint32_t              signalSyncobjCount;
        rz_gfx_syncobj*       pFrameSyncobj;

    } rz_gfx_submit_desc;

    RAZIX_RHI_ALIGN_16 typedef struct rz_gfx_present_desc
    {
        const rz_gfx_swapchain* pSwapchain;
        const rz_gfx_syncobj*   pWaitSyncobjs;
        uint32_t                waitSyncobjCount;
        rz_gfx_syncobj*         pFrameSyncobj;
    } rz_gfx_present_desc;

    //---------------------------------------------------------------------------------------------
    // Gfx API

    RAZIX_RHI_API void rzGfxCtx_StartUp();
    RAZIX_RHI_API void rzGfxCtx_ShutDown();

    RAZIX_RHI_API rz_render_api rzGfxCtx_GetRenderAPI();
    RAZIX_RHI_API void          rzGfxCtx_SetRenderAPI(rz_render_api api);
    RAZIX_RHI_API const char*   rzGfxCtx_GetRenderAPIString();

    RAZIX_RHI_API const char* rzRHI_GetGPUVendorName(uint32_t id);

    // Utility Functions
    RAZIX_RHI_API bool rzRHI_IsDescriptorTypeTexture(rz_gfx_descriptor_type type);
    RAZIX_RHI_API bool rzRHI_IsDescriptorTypeBuffer(rz_gfx_descriptor_type type);
    RAZIX_RHI_API bool rzRHI_IsDescriptorTypeSampler(rz_gfx_descriptor_type type);
    RAZIX_RHI_API bool rzRHI_IsDescriptorTypeTextureRW(rz_gfx_descriptor_type type);
    RAZIX_RHI_API bool rzRHI_IsDescriptorTypeBufferRW(rz_gfx_descriptor_type type);

    RAZIX_RHI_API uint32_t rzRHI_GetBytesPerPixel(rz_gfx_format format);
    RAZIX_RHI_API uint32_t rzRHI_GetMipLevelCount(uint32_t width, uint32_t height);

    //---------------------------------------------------------------------------------------------
    // RHI Jump Table

    /**
     * Rendering API initialization like Instance, Device Creation etc. will happen here! one master place to start it all up!
     */
    typedef void (*rzRHI_GlobalCtxInitFn)(void);
    typedef void (*rzRHI_GlobalCtxDestroyFn)(void);

    //** NON-RESOURCE EXCEPTION **
    typedef void (*rzRHI_CreateSyncobjFn)(void* where, rz_gfx_syncobj_type);
    typedef void (*rzRHI_DestroySyncobjFn)(rz_gfx_syncobj*);

    //** NON-RESOURCE EXCEPTION **
    typedef void (*rzRHI_CreateSwapchainFn)(void* where, void*, uint32_t, uint32_t);
    typedef void (*rzRHI_DestroySwapchainFn)(rz_gfx_swapchain*);

    typedef void (*rzRHI_CreateCmdPoolFn)(void* where);
    typedef void (*rzRHI_DestroyCmdPoolFn)(void* ptr);

    typedef void (*rzRHI_CreateCmdBufFn)(void* where);
    typedef void (*rzRHI_DestroyCmdBufFn)(void* ptr);

    typedef void (*rzRHI_CreateShaderFn)(void* where);
    typedef void (*rzRHI_DestroyShaderFn)(void* ptr);

    typedef void (*rzRHI_CreateRootSignatureFn)(void* where);
    typedef void (*rzRHI_DestroyRootSignatureFn)(void* ptr);

    typedef void (*rzRHI_CreatePipelineFn)(void* where);
    typedef void (*rzRHI_DestroyPipelineFn)(void* ptr);

    typedef void (*rzRHI_CreateTextureFn)(void* where);
    typedef void (*rzRHI_DestroyTextureFn)(void* ptr);

    typedef void (*rzRHI_CreateSamplerFn)(void* where);
    typedef void (*rzRHI_DestroySamplerFn)(void* ptr);

    typedef void (*rzRHI_CreateBufferFn)(void* where);
    typedef void (*rzRHI_DestroyBufferFn)(void* ptr);

    typedef void (*rzRHI_CreateResourceViewFn)(void* where);
    typedef void (*rzRHI_DestroyResourceViewFn)(void* ptr);

    typedef void (*rzRHI_CreateDescriptorHeapFn)(void* where);
    typedef void (*rzRHI_DestroyDescriptorHeapFn)(void* ptr);

    typedef void (*rzRHI_CreateDescriptorTableFn)(void* where);
    typedef void (*rzRHI_DestroyDescriptorTableFn)(void* ptr);

    /**
     * RHI API
     */
    typedef void (*rzRHI_AcquireImageFn)(rz_gfx_swapchain* swapchain, const rz_gfx_syncobj* presentSignalSyncobj);
    typedef void (*rzRHI_WaitOnPrevCmdsFn)(const rz_gfx_syncobj* syncObj);
    typedef void (*rzRHI_SubmitCmdBufFn)(rz_gfx_submit_desc submitDesc);
    typedef void (*rzRHI_PresentFn)(rz_gfx_present_desc presentDesc);

    typedef void (*rzRHI_BeginCmdBufFn)(const rz_gfx_cmdbuf* cmdBuf);
    typedef void (*rzRHI_EndCmdBufFn)(const rz_gfx_cmdbuf* cmdBuf);

    typedef void (*rzRHI_BeginRenderPassFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_renderpass* renderPass);
    typedef void (*rzRHI_EndRenderPassFn)(const rz_gfx_cmdbuf* cmdBuf);

    typedef void (*rzRHI_SetViewportFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_viewport* viewport);
    typedef void (*rzRHI_SetScissorRectFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_rect* scissorRect);

    typedef void (*rzRHI_BindPipelineFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_pipeline* pipeline);
    typedef void (*rzRHI_BindGfxRootSigFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig);
    typedef void (*rzRHI_BindComputeRootSigFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig);

    typedef void (*rzRHI_BindDescriptorHeapsFn)(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_descriptor_heap** heaps, uint32_t heapCount);
    typedef void (*rzRHI_BindDescriptorTablesFn)(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_pipeline_type pipelineType, rz_gfx_descriptor_table** tables, uint32_t tableCount);

    typedef void (*rzRHI_BindVertexBuffersFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* const* buffers, uint32_t bufferCount, const uint32_t* offsets, const uint32_t* strides);
    typedef void (*rzRHI_BindIndexBufferFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* buffer, uint32_t offset, rz_gfx_index_type indexType);

    typedef void (*rzRHI_DrawAutoFn)(const rz_gfx_cmdbuf* cmdBuf, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
    typedef void (*rzRHI_DrawIndexedAutoFn)(const rz_gfx_cmdbuf* cmdBuf, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
    typedef void (*rzRHI_DispatchFn)(const rz_gfx_cmdbuf* cmdBuf, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
    typedef void (*rzRHI_DrawIndirectFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* argumentBuffer, uint32_t argumentBufferOffset, uint32_t drawCount);
    typedef void (*rzRHI_DrawIndexedIndirectFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* argumentBuffer, uint32_t argumentBufferOffset, uint32_t drawCount);
    typedef void (*rzRHI_DispatchIndirectFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* argumentBuffer, uint32_t argumentBufferOffset, uint32_t dispatchCount);

    typedef void (*rzRHI_UpdateDescriptorTableFn)(rz_gfx_descriptor_table* table, rz_gfx_resource_view* view, uint32_t binding);
    typedef void (*rzRHI_UpdateConstantBufferFn)(rz_gfx_buffer_update bufferUpdate);

    typedef void (*rzRHI_InsertImageBarrierFn)(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_texture* texture, rz_gfx_resource_state oldState, rz_gfx_resource_state newState);
    typedef void (*rzRHI_InsertBufferBarrierFn)(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_buffer* buffer, rz_gfx_resource_state oldState, rz_gfx_resource_state newState);
    typedef void (*rzRHI_InsertTextureReadbackFn)(const rz_gfx_texture* texture, rz_gfx_texture_readback* readback);
    typedef void (*rzRHI_InsertBufferReadbackFn)(const rz_gfx_buffer* buffer, rz_gfx_buffer_readback* readback);

    typedef void (*rzRHI_CopyBufferFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* srcBuffer, const rz_gfx_buffer* dstBuffer, uint32_t size, uint32_t srcOffset, uint32_t dstOffset);
    typedef void (*rzRHI_CopyTextureFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* srcTexture, const rz_gfx_texture* dstTexture);
    typedef void (*rzRHI_CopyBufferToTextureFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* srcBuffer, const rz_gfx_texture* dstTexture);
    typedef void (*rzRHI_CopyTextureToBufferFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* srcTexture, const rz_gfx_buffer* dstBuffer);
    typedef void (*rzRHI_GenerateMipmapsFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* texture);
    typedef void (*rzRHI_BlitTextureFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* srcTexture, const rz_gfx_texture* dstTexture);
    typedef void (*rzRHI_ResolveTextureFn)(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* srcTexture, const rz_gfx_texture* dstTexture);

    typedef void (*rzRHI_SignalGPUFn)(rz_gfx_syncobj* syncObj);
    typedef void (*rzRHI_FlushGPUWorkFn)(rz_gfx_syncobj* syncObj);
    typedef void (*rzRHI_ResizeSwapchainFn)(rz_gfx_swapchain* swapchain, uint32_t width, uint32_t height);

    typedef struct rz_rhi_api
    {
        rzRHI_GlobalCtxInitFn          GlobalCtxInit;
        rzRHI_GlobalCtxDestroyFn       GlobalCtxDestroy;
        rzRHI_CreateSyncobjFn          CreateSyncobj;
        rzRHI_DestroySyncobjFn         DestroySyncobj;
        rzRHI_CreateSwapchainFn        CreateSwapchain;
        rzRHI_DestroySwapchainFn       DestroySwapchain;
        rzRHI_CreateCmdPoolFn          CreateCmdPool;
        rzRHI_DestroyCmdPoolFn         DestroyCmdPool;
        rzRHI_CreateCmdBufFn           CreateCmdBuf;
        rzRHI_DestroyCmdBufFn          DestroyCmdBuf;
        rzRHI_CreateShaderFn           CreateShader;
        rzRHI_DestroyShaderFn          DestroyShader;
        rzRHI_CreateRootSignatureFn    CreateRootSignature;
        rzRHI_DestroyRootSignatureFn   DestroyRootSignature;
        rzRHI_CreatePipelineFn         CreatePipeline;
        rzRHI_DestroyPipelineFn        DestroyPipeline;
        rzRHI_CreateTextureFn          CreateTexture;
        rzRHI_DestroyTextureFn         DestroyTexture;
        rzRHI_CreateSamplerFn          CreateSampler;
        rzRHI_DestroySamplerFn         DestroySampler;
        rzRHI_CreateBufferFn           CreateBuffer;
        rzRHI_DestroyBufferFn          DestroyBuffer;
        rzRHI_CreateResourceViewFn     CreateResourceView;
        rzRHI_DestroyResourceViewFn    DestroyResourceView;
        rzRHI_CreateDescriptorHeapFn   CreateDescriptorHeap;
        rzRHI_DestroyDescriptorHeapFn  DestroyDescriptorHeap;
        rzRHI_CreateDescriptorTableFn  CreateDescriptorTable;
        rzRHI_DestroyDescriptorTableFn DestroyDescriptorTable;
        rzRHI_AcquireImageFn           AcquireImage;
        rzRHI_WaitOnPrevCmdsFn         WaitOnPrevCmds;
        rzRHI_PresentFn                Present;
        rzRHI_BeginCmdBufFn            BeginCmdBuf;
        rzRHI_EndCmdBufFn              EndCmdBuf;
        rzRHI_SubmitCmdBufFn           SubmitCmdBuf;
        rzRHI_BeginRenderPassFn        BeginRenderPass;
        rzRHI_EndRenderPassFn          EndRenderPass;
        rzRHI_SetScissorRectFn         SetScissorRect;
        rzRHI_SetViewportFn            SetViewport;
        rzRHI_BindPipelineFn           BindPipeline;
        rzRHI_BindGfxRootSigFn         BindGfxRootSig;
        rzRHI_BindComputeRootSigFn     BindComputeRootSig;
        rzRHI_BindDescriptorHeapsFn    BindDescriptorHeaps;
        rzRHI_BindDescriptorTablesFn   BindDescriptorTables;
        rzRHI_BindVertexBuffersFn      BindVertexBuffers;
        rzRHI_BindIndexBufferFn        BindIndexBuffer;
        rzRHI_DrawAutoFn               DrawAuto;
        rzRHI_DrawIndexedAutoFn        DrawIndexedAuto;
        rzRHI_DispatchFn               Dispatch;
        rzRHI_DrawIndirectFn           DrawIndirect;
        rzRHI_DrawIndexedIndirectFn    DrawIndexedIndirect;
        rzRHI_DispatchIndirectFn       DispatchIndirect;
        rzRHI_UpdateDescriptorTableFn  UpdateDescriptorTable;
        rzRHI_UpdateConstantBufferFn   UpdateConstantBuffer;
        rzRHI_CopyBufferFn             CopyBuffer;
        rzRHI_CopyTextureFn            CopyTexture;
        rzRHI_CopyBufferToTextureFn    CopyBufferToTexture;
        rzRHI_CopyTextureToBufferFn    CopyTextureToBuffer;
        rzRHI_GenerateMipmapsFn        GenerateMipmaps;
        rzRHI_BlitTextureFn            BlitTexture;
        rzRHI_ResolveTextureFn         ResolveTexture;
        rzRHI_InsertImageBarrierFn     InsertImageBarrier;
        rzRHI_InsertBufferBarrierFn    InsertBufferBarrier;
        rzRHI_InsertTextureReadbackFn  InsertTextureReadback;
        rzRHI_InsertBufferReadbackFn   InsertBufferReadback;
        rzRHI_SignalGPUFn              SignalGPU;
        rzRHI_FlushGPUWorkFn           FlushGPUWork;
        rzRHI_ResizeSwapchainFn        ResizeSwapchain;
    } rz_rhi_api;

    //---------------------------------------------------------------------------------------------
    // Globals
    //---------------------------------
    RAZIX_RHI_API extern rz_gfx_context g_GfxCtx;    // Global Graphics Context singleton instance
    //---------------------------------
    RAZIX_RHI_API extern rz_render_api g_RenderAPI;
    //---------------------------------
    RAZIX_RHI_API extern rz_rhi_api g_RHI;
    //---------------------------------
    RAZIX_RHI_API extern rz_gfx_features g_GraphicsFeatures;
    //---------------------------------

#define rzGfxCtx_GlobalCtxInit    g_RHI.GlobalCtxInit
#define rzGfxCtx_GlobalCtxDestroy g_RHI.GlobalCtxDestroy

// Adding profiling macros to enable/disable RHI Create/Destroy functions is redundant, RZResourceManager already does that
#define rzRHI_CreateSyncobj          g_RHI.CreateSyncobj
#define rzRHI_DestroySyncobj         g_RHI.DestroySyncobj
#define rzRHI_CreateSwapchain        g_RHI.CreateSwapchain
#define rzRHI_DestroySwapchain       g_RHI.DestroySwapchain
#define rzRHI_CreateCmdPool          g_RHI.CreateCmdPool
#define rzRHI_DestroyCmdPool         g_RHI.DestroyCmdPool
#define rzRHI_CreateCmdBuf           g_RHI.CreateCmdBuf
#define rzRHI_DestroyCmdBuf          g_RHI.DestroyCmdBuf
#define rzRHI_CreateShader           g_RHI.CreateShader
#define rzRHI_DestroyShader          g_RHI.DestroyShader
#define rzRHI_CreateRootSignature    g_RHI.CreateRootSignature
#define rzRHI_DestroyRootSignature   g_RHI.DestroyRootSignature
#define rzRHI_CreatePipeline         g_RHI.CreatePipeline
#define rzRHI_DestroyPipeline        g_RHI.DestroyPipeline
#define rzRHI_CreateTexture          g_RHI.CreateTexture
#define rzRHI_DestroyTexture         g_RHI.DestroyTexture
#define rzRHI_CreateSampler          g_RHI.CreateSampler
#define rzRHI_DestroySampler         g_RHI.DestroySampler
#define rzRHI_CreateBuffer           g_RHI.CreateBuffer
#define rzRHI_DestroyBuffer          g_RHI.DestroyBuffer
#define rzRHI_CreateResourceView     g_RHI.CreateResourceView
#define rzRHI_DestroyResourceView    g_RHI.DestroyResourceView
#define rzRHI_CreateDescriptorHeap   g_RHI.CreateDescriptorHeap
#define rzRHI_DestroyDescriptorHeap  g_RHI.DestroyDescriptorHeap
#define rzRHI_CreateDescriptorTable  g_RHI.CreateDescriptorTable
#define rzRHI_DestroyDescriptorTable g_RHI.DestroyDescriptorTable

#if !RZ_PROFILER_ENABLED
    #if defined(RAZIX_RHI_USE_RESOURCE_MANAGER_HANDLES) && defined(__cplusplus)
        #define rzRHI_AcquireImage                 g_RHI.AcquireImage
        #define rzRHI_WaitOnPrevCmds               g_RHI.WaitOnPrevCmds
        #define rzRHI_SubmitCmdBuf                 g_RHI.SubmitCmdBuf
        #define rzRHI_Present                      g_RHI.Present
        #define rzRHI_BeginCmdBuf(cb)              g_RHI.BeginCmdBuf(RZResourceManager::Get().getCommandBufferResource(cb))
        #define rzRHI_EndCmdBuf(cb)                g_RHI.EndCmdBuf(RZResourceManager::Get().getCommandBufferResource(cb))
        #define rzRHI_BeginRenderPass(cb, info)    g_RHI.BeginRenderPass(RZResourceManager::Get().getCommandBufferResource(cb), info)
        #define rzRHI_EndRenderPass(cb)            g_RHI.EndRenderPass(RZResourceManager::Get().getCommandBufferResource(cb))
        #define rzRHI_SetScissorRect(cb, viewport) g_RHI.SetScissorRect(RZResourceManager::Get().getCommandBufferResource(cb), viewport)
        #define rzRHI_SetViewport(cb, rect)        g_RHI.SetViewport(RZResourceManager::Get().getCommandBufferResource(cb), rect)
        #define rzRHI_BindPipeline(cb, pp)         g_RHI.BindPipeline(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getPipelineResource(pp))
        #define rzRHI_BindGfxRootSig(cb, rs)       g_RHI.BindGfxRootSig(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getRootSignatureResource(rs))
        #define rzRHI_BindComputeRootSig(cb, rs)   g_RHI.BindComputeRootSig(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getRootSignatureResource(rs))
        #define rzRHI_BindDescriptorHeaps(cb, heapHandles, N)                                                \
            do {                                                                                             \
                rz_gfx_descriptor_heap* _heaps[N];                                                           \
                for (uint32_t i = 0; i < N; i++)                                                             \
                    _heaps[i] = RZResourceManager::Get().getDescriptorHeapResource(heaps);                   \
                g_RHI.BindDescriptorHeaps(RZResourceManager::Get().getCommandBufferResource(cb), _heaps, N); \
            } while (0);

        #define rzRHI_BindDescriptorHeapsContainer(cb, heaps)                                       \
            do {                                                                                    \
                std::vector<rz_gfx_descriptor_heap*> heapPtrs;                                      \
                heapPtrs.reserve((heaps).size());                                                   \
                for (const auto& handle: (heaps))                                                   \
                    heapPtrs.push_back(RZResourceManager::Get().getDescriptorHeapResource(handle)); \
                g_RHI.BindDescriptorHeaps(                                                          \
                    RZResourceManager::Get().getCommandBufferResource(cb),                          \
                    heapPtrs.data(),                                                                \
                    heapPtrs.size());                                                               \
            } while (0)

        #define rzRHI_BindDescriptorTables(cb, ppt, dts, N)                                                      \
            do {                                                                                                 \
                rz_gfx_descriptor_table* dts[N];                                                                 \
                for (uint32_t i = 0; i < N; i++)                                                                 \
                    _dts[i] = RZResourceManager::Get().getDescriptorTableResource(dts);                          \
                g_RHI.BindDescriptorTables(RZResourceManager::Get().getCommandBufferResource(cb), ppt, _dts, N); \
            } while (0);

        #define rzRHI_BindDescriptorTablesContainer(cb, ppt, dts)                                     \
            do {                                                                                      \
                std::vector<rz_gfx_descriptor_table*> tablePtrs;                                      \
                tablePtrs.reserve((dts).size());                                                      \
                for (const auto& handle: (dts))                                                       \
                    tablePtrs.push_back(RZResourceManager::Get().getDescriptorTableResource(handle)); \
                g_RHI.BindDescriptorTables(                                                           \
                    RZResourceManager::Get().getCommandBufferResource(cb),                            \
                    ppt,                                                                              \
                    tablePtrs.data(),                                                                 \
                    tablePtrs.size());                                                                \
            } while (0)

        #define rzRHI_BindVertexBuffers(cb, bu, N, off, str)                                                        \
            do {                                                                                                    \
                rz_gfx_buffer* _bufs[N];                                                                            \
                for (uint32_t i = 0; i < N; i++)                                                                    \
                    _bufs[i] = RZResourceManager::Get().getBufferResource(bu);                                      \
                g_RHI.BindVertexBuffers(RZResourceManager::Get().getCommandBufferResource(cb), _bufs, N, off, str); \
            } while (0);

        #define rzRHI_BindVertexBuffersContainer(cb, bu, off, str)                         \
            do {                                                                           \
                std::vector<rz_gfx_buffer*> bufPtrs;                                       \
                bufPtrs.reserve((bu).size());                                              \
                for (const auto& handle: (bu))                                             \
                    bufPtrs.push_back(RZResourceManager::Get().getBufferResource(handle)); \
                g_RHI.BindVertexBuffers(                                                   \
                    RZResourceManager::Get().getCommandBufferResource(cb),                 \
                    bufPtrs.data(),                                                        \
                    bufPtrs.size(),                                                        \
                    off,                                                                   \
                    str);                                                                  \
            } while (0)

        #define rzRHI_BindIndexBuffer(cb, bu, off, it) g_RHI.BindIndexBuffer(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getBufferResource(bu), off, it)

        #define rzRHI_DrawAuto(cb, vc, ic, fv, fi)                       g_RHI.DrawAuto(RZResourceManager::Get().getCommandBufferResource(cb), vc, ic, fv, fi)
        #define rzRHI_DrawIndexedAuto(cb, ic, icount, iv, fi, fo)        g_RHI.DrawIndexedAuto(RZResourceManager::Get().getCommandBufferResource(cb), ic, icount, iv, fi, fo)
        #define rzRHI_Dispatch(cb, gx, gy, gz)                           g_RHI.Dispatch(RZResourceManager::Get().getCommandBufferResource(cb), gx, gy, gz)
        #define rzRHI_DrawIndirect(cb, bu, offset, maxDrawCount)         g_RHI.DrawIndirect(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getBufferResource(bu), offset, maxDrawCount)
        #define rzRHI_DrawIndexedIndirect(cb, bu, offset, maxDrawCount)  g_RHI.DrawIndexedIndirect(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getBufferResource(bu), offset, maxDrawCount)
        #define rzRHI_DispatchIndirect(cb, bu, offset, maxDispatchCount) g_RHI.DispatchIndirect(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getBufferResource(bu), offset, maxDispatchCount)

        #define rzRHI_UpdateDescriptorTable(dt, rv, N)                                                         \
            do {                                                                                               \
                rz_gfx_resource_view* _rvs[N];                                                                 \
                for (uint32_t i = 0; i < N; i++)                                                               \
                    _rvs[i] = RZResourceManager::Get().getResourceViewResource(rv);                            \
                g_RHI.UpdateDescriptorTable(RZResourceManager::Get().getDescriptorTableResource(dt), _rvs, N); \
            } while (0);
        #define rzRHI_UpdateConstantBuffer(bu)                      g_RHI.UpdateConstantBuffer(bu)
        #define rzRHI_InsertImageBarrier(cb, text, bs, as)          g_RHI.InsertImageBarrier(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getTextureResource(text), bs, as)
        #define rzRHI_InsertSwapchainImageBarrier(cb, text, bs, as) g_RHI.InsertImageBarrier(RZResourceManager::Get().getCommandBufferResource(cb), text, bs, as)
        #define rzRHI_InsertBufferBarrier(cb, buf, bs, as)          g_RHI.InsertBufferBarrier(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getBufferResource(buf), bs, as)
        #define rzRHI_InsertTextureReadback(text, rb)               g_RHI.InsertTextureReadback(RZResourceManager::Get().getTextureResource(text), rb)
        #define rzRHI_InsertSwapchainTextureReadback(text, rb)      g_RHI.InsertTextureReadback(text, rb)
        #define rzRHI_InsertBufferReadback(buf, rb)                 g_RHI.InsertBufferReadback(RZResourceManager::Get().getBufferResource(buf), rb)
        #define rzRHI_CopyBuffer(cb, sb, db, s, so, doff)           g_RHI.CopyBuffer(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getBufferResource(sb), RZResourceManager::Get().getBufferResource(db), s, so, doff)
        #define rzRHI_CopyTexture(cb, st, dt)                       g_RHI.CopyTexture(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getTextureResource(st), RZResourceManager::Get().getTextureResource(dt))
        #define rzRHI_CopyTextureToSwapchain(cb, st, dt)            g_RHI.CopyTexture(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getTextureResource(st), dt)
        #define rzRHI_CopyBufferToTexture(cb, sb, dt)               g_RHI.CopyBufferToTexture(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getBufferResource(sb), RZResourceManager::Get().getTextureResource(dt))
        #define rzRHI_CopyTextureToBuffer(cb, st, db)               g_RHI.CopyTextureToBuffer(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getTextureResource(st), RZResourceManager::Get().getBufferResource(db))
        #define rzRHI_GenerateMipmaps(cb, text)                     g_RHI.GenerateMipmaps(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getTextureResource(text))
        #define rzRHI_BlitTexture(cb, st, dt)                       g_RHI.BlitTexture(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getTextureResource(st), RZResourceManager::Get().getTextureResource(dt))
        #define rzRHI_ResolveTexture(cb, st, dt)                    g_RHI.ResolveTexture(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getTextureResource(st), RZResourceManager::Get().getTextureResource(dt))
        #define rzRHI_SignalGPU                                     g_RHI.SignalGPU
        #define rzRHI_FlushGPUWork                                  g_RHI.FlushGPUWork
        #define rzRHI_ResizeSwapchain                               g_RHI.ResizeSwapchain
        #define rzRHI_BeginFrame                                    g_RHI.BeginFrame
        #define rzRHI_EndFrame                                      g_RHI.EndFrame
    #else
        #define rzRHI_AcquireImage                   g_RHI.AcquireImage
        #define rzRHI_WaitOnPrevCmds                 g_RHI.WaitOnPrevCmds
        #define rzRHI_SubmitCmdBuf                   g_RHI.SubmitCmdBuf
        #define rzRHI_Present                        g_RHI.Present
        #define rzRHI_BeginCmdBuf                    g_RHI.BeginCmdBuf
        #define rzRHI_EndCmdBuf                      g_RHI.EndCmdBuf
        #define rzRHI_BeginRenderPass                g_RHI.BeginRenderPass
        #define rzRHI_EndRenderPass                  g_RHI.EndRenderPass
        #define rzRHI_SetScissorRect                 g_RHI.SetScissorRect
        #define rzRHI_SetViewport                    g_RHI.SetViewport
        #define rzRHI_BindPipeline                   g_RHI.BindPipeline
        #define rzRHI_BindGfxRootSig                 g_RHI.BindGfxRootSig
        #define rzRHI_BindComputeRootSig             g_RHI.BindComputeRootSig
        #define rzRHI_BindDescriptorHeaps            g_RHI.BindDescriptorHeaps
        #define rzRHI_BindDescriptorHeapsContainer   g_RHI.BindDescriptorHeaps
        #define rzRHI_BindDescriptorTables           g_RHI.BindDescriptorTables
        #define rzRHI_BindDescriptorTablesContainer  g_RHI.BindDescriptorTables
        #define rzRHI_BindVertexBuffers              g_RHI.BindVertexBuffers
        #define rzRHI_BindVertexBuffersContainer     g_RHI.BindVertexBuffers
        #define rzRHI_BindIndexBuffer                g_RHI.BindIndexBuffer
        #define rzRHI_DrawAuto                       g_RHI.DrawAuto
        #define rzRHI_DrawIndexedAuto                g_RHI.DrawIndexedAuto
        #define rzRHI_Dispatch                       g_RHI.Dispatch
        #define rzRHI_DrawIndirect                   g_RHI.DrawIndirect
        #define rzRHI_DrawIndexedIndirect            g_RHI.DrawIndexedIndirect
        #define rzRHI_DispatchIndirect               g_RHI.DispatchIndirect
        #define rzRHI_UpdateDescriptorTable          g_RHI.UpdateDescriptorTable
        #define rzRHI_UpdateConstantBuffer           g_RHI.UpdateConstantBuffer
        #define rzRHI_InsertImageBarrier             g_RHI.InsertImageBarrier
        #define rzRHI_InsertSwapchainImageBarrier    g_RHI.InsertImageBarrier
        #define rzRHI_InsertBufferBarrier            g_RHI.InsertBufferBarrier
        #define rzRHI_InsertTextureReadback          g_RHI.InsertTextureReadback
        #define rzRHI_InsertSwapchainTextureReadback g_RHI.InsertTextureReadback
        #define rzRHI_InsertBufferReadback           g_RHI.InsertBufferReadback
        #define rzRHI_CopyBuffer                     g_RHI.CopyBuffer
        #define rzRHI_CopyTexture                    g_RHI.CopyTexture
        #define rzRHI_CopyTextureToSwapchain         g_RHI.CopyTexture
        #define rzRHI_CopyBufferToTexture            g_RHI.CopyBufferToTexture
        #define rzRHI_CopyTextureToBuffer            g_RHI.CopyTextureToBuffer
        #define rzRHI_GenerateMipmaps                g_RHI.GenerateMipmaps
        #define rzRHI_BlitTexture                    g_RHI.BlitTexture
        #define rzRHI_ResolveTexture                 g_RHI.ResolveTexture
        #define rzRHI_SignalGPU                      g_RHI.SignalGPU
        #define rzRHI_FlushGPUWork                   g_RHI.FlushGPUWork
        #define rzRHI_ResizeSwapchain                g_RHI.ResizeSwapchain
        #define rzRHI_BeginFrame                     g_RHI.BeginFrame
        #define rzRHI_EndFrame                       g_RHI.EndFrame
    #endif
#else
    #if defined(RAZIX_RHI_USE_RESOURCE_MANAGER_HANDLES) && defined(__cplusplus)
        #define rzRHI_AcquireImage(sc, sso)                                                        \
            do {                                                                                   \
                RAZIX_PROFILE_SCOPEC("rzRHI_AcquireImage", RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS); \
                g_RHI.AcquireImage(sc, sso);                                                       \
            } while (0)

        #define rzRHI_WaitOnPrevCmds(so)                                                            \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_WaitOnPrevCmds", RZ_PROFILE_COLOR_RHI_SYNCHRONIZATION); \
                g_RHI.WaitOnPrevCmds(so);                                                           \
            } while (0)

        #define rzRHI_SubmitCmdBuf(desc)                                                          \
            do {                                                                                  \
                RAZIX_PROFILE_SCOPEC("rzRHI_SubmitCmdBuf", RZ_PROFILE_COLOR_RHI_COMMAND_BUFFERS); \
                g_RHI.SubmitCmdBuf(desc);                                                         \
            } while (0)

        #define rzRHI_Present(desc)                                                           \
            do {                                                                              \
                RAZIX_PROFILE_SCOPEC("rzRHI_Present", RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS); \
                g_RHI.Present(desc);                                                          \
            } while (0)

        #define rzRHI_BeginCmdBuf(cb)                                                            \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_BeginCmdBuf", RZ_PROFILE_COLOR_RHI_COMMAND_BUFFERS); \
                g_RHI.BeginCmdBuf(RZResourceManager::Get().getCommandBufferResource(cb));        \
            } while (0)

        #define rzRHI_EndCmdBuf(cb)                                                            \
            do {                                                                               \
                RAZIX_PROFILE_SCOPEC("rzRHI_EndCmdBuf", RZ_PROFILE_COLOR_RHI_COMMAND_BUFFERS); \
                g_RHI.EndCmdBuf(RZResourceManager::Get().getCommandBufferResource(cb));        \
            } while (0)

        #define rzRHI_BeginRenderPass(cb, info)                                                     \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_BeginRenderPass", RZ_PROFILE_COLOR_RHI_RENDER_PASSES);  \
                g_RHI.BeginRenderPass(RZResourceManager::Get().getCommandBufferResource(cb), info); \
            } while (0)

        #define rzRHI_EndRenderPass(cb)                                                          \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_EndRenderPass", RZ_PROFILE_COLOR_RHI_RENDER_PASSES); \
                g_RHI.EndRenderPass(RZResourceManager::Get().getCommandBufferResource(cb));      \
            } while (0)

        #define rzRHI_SetScissorRect(cb, viewport)                                                     \
            do {                                                                                       \
                RAZIX_PROFILE_SCOPEC("rzRHI_SetScissorRect", RZ_PROFILE_COLOR_RHI);                    \
                g_RHI.SetScissorRect(RZResourceManager::Get().getCommandBufferResource(cb), viewport); \
            } while (0)

        #define rzRHI_SetViewport(cb, rect)                                                     \
            do {                                                                                \
                RAZIX_PROFILE_SCOPEC("rzRHI_SetViewport", RZ_PROFILE_COLOR_RHI);                \
                g_RHI.SetViewport(RZResourceManager::Get().getCommandBufferResource(cb), rect); \
            } while (0)

        #define rzRHI_BindPipeline(cb, pp)                                                                                                   \
            do {                                                                                                                             \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindPipeline", RZ_PROFILE_COLOR_RHI_PIPELINE_BINDS);                                             \
                g_RHI.BindPipeline(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getPipelineResource(pp)); \
            } while (0)

        #define rzRHI_BindGfxRootSig(cb, rs)                                                                                                        \
            do {                                                                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindGfxRootSig", RZ_PROFILE_COLOR_RHI_PIPELINE_BINDS);                                                  \
                g_RHI.BindGfxRootSig(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getRootSignatureResource(rs)); \
            } while (0)

        #define rzRHI_BindComputeRootSig(cb, rs)                                                                                                        \
            do {                                                                                                                                        \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindComputeRootSig", RZ_PROFILE_COLOR_RHI_PIPELINE_BINDS);                                                  \
                g_RHI.BindComputeRootSig(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getRootSignatureResource(rs)); \
            } while (0)
        #define rzRHI_BindDescriptorHeaps(cb, heapHandles, N)                                                \
            do {                                                                                             \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindDescriptorHeaps", RZ_PROFILE_COLOR_RHI_DRAW_CALLS);          \
                rz_gfx_descriptor_heap* _heaps[N];                                                           \
                for (uint32_t i = 0; i < N; i++)                                                             \
                    _heaps[i] = RZResourceManager::Get().getDescriptorHeapResource(heaps[i]);                \
                g_RHI.BindDescriptorHeaps(RZResourceManager::Get().getCommandBufferResource(cb), _heaps, N); \
            } while (0);

        // TODO: use custom container for descriptor heaps and tables
        #define rzRHI_BindDescriptorHeapsContainer(cb, heaps)                                       \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindDescriptorHeaps",                                   \
                    RZ_PROFILE_COLOR_RHI_DRAW_CALLS);                                               \
                std::vector<rz_gfx_descriptor_heap*> heapPtrs;                                      \
                heapPtrs.reserve((heaps).size());                                                   \
                for (const auto& handle: (heaps))                                                   \
                    heapPtrs.push_back(RZResourceManager::Get().getDescriptorHeapResource(handle)); \
                g_RHI.BindDescriptorHeaps(                                                          \
                    RZResourceManager::Get().getCommandBufferResource(cb),                          \
                    heapPtrs.data(),                                                                \
                    heapPtrs.size());                                                               \
            } while (0)

        #define rzRHI_BindDescriptorTables(cb, ppt, dts, N)                                                      \
            do {                                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindDescriptorTables", RZ_PROFILE_COLOR_RHI_DRAW_CALLS);             \
                rz_gfx_descriptor_table* _dts[N];                                                                \
                for (uint32_t i = 0; i < N; i++)                                                                 \
                    _dts[i] = RZResourceManager::Get().getDescriptorTableResource(dts[i]);                       \
                g_RHI.BindDescriptorTables(RZResourceManager::Get().getCommandBufferResource(cb), ppt, _dts, N); \
            } while (0);

        #define rzRHI_BindDescriptorTablesContainer(cb, ppt, dts)                                     \
            do {                                                                                      \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindDescriptorTables", RZ_PROFILE_COLOR_RHI_DRAW_CALLS);  \
                std::vector<rz_gfx_descriptor_table*> tablePtrs;                                      \
                tablePtrs.reserve((dts).size());                                                      \
                for (const auto& handle: (dts))                                                       \
                    tablePtrs.push_back(RZResourceManager::Get().getDescriptorTableResource(handle)); \
                g_RHI.BindDescriptorTables(                                                           \
                    RZResourceManager::Get().getCommandBufferResource(cb),                            \
                    ppt,                                                                              \
                    tablePtrs.data(),                                                                 \
                    tablePtrs.size());                                                                \
            } while (0)

        #define rzRHI_BindVertexBuffers(cb, bu, N, off, str)                                                        \
            do {                                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindVertexBuffers", RZ_PROFILE_COLOR_RHI_DRAW_CALLS);                   \
                rz_gfx_buffer* _bufs[N];                                                                            \
                for (uint32_t i = 0; i < N; i++)                                                                    \
                    _bufs[i] = RZResourceManager::Get().getBufferResource(bu[i]);                                   \
                g_RHI.BindVertexBuffers(RZResourceManager::Get().getCommandBufferResource(cb), _bufs, N, off, str); \
            } while (0);

        #define rzRHI_BindVertexBuffersContainer(cb, bu, off, str)                                \
            do {                                                                                  \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindVertexBuffers", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                std::vector<rz_gfx_buffer*> bufPtrs;                                              \
                bufPtrs.reserve((bu).size());                                                     \
                for (const auto& handle: (bu))                                                    \
                    bufPtrs.push_back(RZResourceManager::Get().getBufferResource(handle));        \
                g_RHI.BindVertexBuffers(                                                          \
                    RZResourceManager::Get().getCommandBufferResource(cb),                        \
                    bufPtrs.data(),                                                               \
                    bufPtrs.size(),                                                               \
                    off,                                                                          \
                    str);                                                                         \
            } while (0)

        #define rzRHI_BindIndexBuffer(cb, bu, off, it)                                       \
            do {                                                                             \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindIndexBuffer", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.BindIndexBuffer(RZResourceManager::Get().getCommandBufferResource(cb), \
                    RZResourceManager::Get().getBufferResource(bu),                          \
                    off,                                                                     \
                    it);                                                                     \
            } while (0)

        #define rzRHI_DrawAuto(cb, vc, ic, fv, fi)                                                     \
            do {                                                                                       \
                RAZIX_PROFILE_SCOPEC("rzRHI_DrawAuto", RZ_PROFILE_COLOR_RHI_DRAW_CALLS);               \
                g_RHI.DrawAuto(RZResourceManager::Get().getCommandBufferResource(cb), vc, ic, fv, fi); \
            } while (0)

        #define rzRHI_DrawIndexedAuto(cb, ic, icount, iv, fi, fo)                                                     \
            do {                                                                                                      \
                RAZIX_PROFILE_SCOPEC("rzRHI_DrawIndexedAuto", RZ_PROFILE_COLOR_RHI_DRAW_CALLS);                       \
                g_RHI.DrawIndexedAuto(RZResourceManager::Get().getCommandBufferResource(cb), ic, icount, iv, fi, fo); \
            } while (0)

        #define rzRHI_Dispatch(cb, gx, gy, gz)                                                     \
            do {                                                                                   \
                RAZIX_PROFILE_SCOPEC("rzRHI_Dispatch", RZ_PROFILE_COLOR_RHI_DRAW_CALLS);           \
                g_RHI.Dispatch(RZResourceManager::Get().getCommandBufferResource(cb), gx, gy, gz); \
            } while (0)

        #define rzRHI_DrawIndirect(cb, bu, offset, maxDrawCount)                             \
            do {                                                                             \
                RAZIX_PROFILE_SCOPEC("rzRHI_DrawIndirect", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.DrawIndirect(RZResourceManager::Get().getCommandBufferResource(cb),    \
                    RZResourceManager::Get().getBufferResource(bu),                          \
                    offset,                                                                  \
                    maxDrawCount);                                                           \
            } while (0)

        #define rzRHI_DrawIndexedIndirect(cb, bu, offset, maxDrawCount)                             \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_DrawIndexedIndirect", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.DrawIndexedIndirect(RZResourceManager::Get().getCommandBufferResource(cb),    \
                    RZResourceManager::Get().getBufferResource(bu),                                 \
                    offset,                                                                         \
                    maxDrawCount);                                                                  \
            } while (0)

        #define rzRHI_DispatchIndirect(cb, bu, offset, maxDispatchCount)                         \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_DispatchIndirect", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.DispatchIndirect(RZResourceManager::Get().getCommandBufferResource(cb),    \
                    RZResourceManager::Get().getBufferResource(bu),                              \
                    offset,                                                                      \
                    maxDispatchCount);                                                           \
            } while (0)

        #define rzRHI_UpdateDescriptorTable(dt, rv, N)                                                         \
            do {                                                                                               \
                RAZIX_PROFILE_SCOPEC("rzRHI_UpdateDescriptorTable", RZ_PROFILE_COLOR_RHI);                     \
                rz_gfx_resource_view* _rvs[N];                                                                 \
                for (uint32_t i = 0; i < N; i++)                                                               \
                    _rvs[i] = RZResourceManager::Get().getResourceViewResource(rv[i]);                         \
                g_RHI.UpdateDescriptorTable(RZResourceManager::Get().getDescriptorTableResource(dt), _rvs, N); \
            } while (0);

        #define rzRHI_UpdateDescriptorTableContainer(dt, rvs)                                   \
            do {                                                                                \
                RAZIX_PROFILE_SCOPEC("rzRHI_UpdateDescriptorTable", RZ_PROFILE_COLOR_RHI);      \
                std::vector<rz_gfx_resource_view*> rvPtrs;                                      \
                rvPtrs.reserve((rvs).size());                                                   \
                for (const auto& handle: (rvs))                                                 \
                    rvPtrs.push_back(RZResourceManager::Get().getResourceViewResource(handle)); \
                g_RHI.UpdateDescriptorTable(                                                    \
                    RZResourceManager::Get().getDescriptorTableResource(dt),                    \
                    rvPtrs.data(),                                                              \
                    rvPtrs.size());                                                             \
            } while (0)

        #define rzRHI_UpdateConstantBuffer(bu)                                            \
            do {                                                                          \
                RAZIX_PROFILE_SCOPEC("rzRHI_UpdateConstantBuffer", RZ_PROFILE_COLOR_RHI); \
                g_RHI.UpdateConstantBuffer(bu);                                           \
            } while (0)

        #define rzRHI_InsertImageBarrier(cb, tex, bs, as)                                                                                                  \
            do {                                                                                                                                           \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertImageBarrier", RZ_PROFILE_COLOR_RHI_RESOURCE_BARRIERS);                                                  \
                g_RHI.InsertImageBarrier(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getTextureResource(tex), bs, as); \
            } while (0)

        #define rzRHI_InsertSwapchainImageBarrier(cb, tex, bs, as)                                                 \
            do {                                                                                                   \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertSwapchainImageBarrier", RZ_PROFILE_COLOR_RHI_RESOURCE_BARRIERS); \
                g_RHI.InsertImageBarrier(RZResourceManager::Get().getCommandBufferResource(cb), tex, bs, as);      \
            } while (0)

        #define rzRHI_InsertBufferBarrier(cb, buf, bs, as)                                                                                                 \
            do {                                                                                                                                           \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertBufferBarrier", RZ_PROFILE_COLOR_RHI_RESOURCE_BARRIERS);                                                 \
                g_RHI.InsertBufferBarrier(RZResourceManager::Get().getCommandBufferResource(cb), RZResourceManager::Get().getBufferResource(buf), bs, as); \
            } while (0)

        #define rzRHI_InsertTextureReadback(tex, rb)                                               \
            do {                                                                                   \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertTextureReadback", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.InsertTextureReadback(RZResourceManager::Get().getTextureResource(tex), rb); \
            } while (0)

        #define rzRHI_InsertBufferReadback(buf, rb)                                              \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertBufferReadback", RZ_PROFILE_COLOR_RHI);        \
                g_RHI.InsertBufferReadback(RZResourceManager::Get().getBufferResource(buf), rb); \
            } while (0)

        #define rzRHI_InsertSwapchainTextureReadback(tex, rb)                                       \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertSwapchainTextureReadback", RZ_PROFILE_COLOR_RHI); \
                g_RHI.InsertTextureReadback(tex, rb);                                               \
            } while (0)

        #define rzRHI_CopyBuffer(cb, sb, db, s, so, doff)                               \
            do {                                                                        \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyBuffer", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.CopyBuffer(RZResourceManager::Get().getCommandBufferResource(cb), \
                    RZResourceManager::Get().getBufferResource(sb),                     \
                    RZResourceManager::Get().getBufferResource(db),                     \
                    s,                                                                  \
                    so,                                                                 \
                    doff);                                                              \
            } while (0)

        #define rzRHI_CopyTexture(cb, st, dt)                                            \
            do {                                                                         \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyTexture", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.CopyTexture(RZResourceManager::Get().getCommandBufferResource(cb), \
                    RZResourceManager::Get().getTextureResource(st),                     \
                    RZResourceManager::Get().getTextureResource(dt));                    \
            } while (0)

        #define rzRHI_CopyTextureToSwapchain(cb, st, dt)                                    \
            do {                                                                            \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyTextureToSwapchain", RZ_PROFILE_COLOR_RHI); \
                g_RHI.CopyTexture(RZResourceManager::Get().getCommandBufferResource(cb),    \
                    RZResourceManager::Get().getTextureResource(st),                        \
                    dt);                                                                    \
            } while (0)

        #define rzRHI_CopyBufferToTexture(cb, sb, dt)                                            \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyBufferToTexture", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.CopyBufferToTexture(RZResourceManager::Get().getCommandBufferResource(cb), \
                    RZResourceManager::Get().getBufferResource(sb),                              \
                    RZResourceManager::Get().getTextureResource(dt));                            \
            } while (0)

        #define rzRHI_CopyTextureToBuffer(cb, st, db)                                            \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyTextureToBuffer", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.CopyTextureToBuffer(RZResourceManager::Get().getCommandBufferResource(cb), \
                    RZResourceManager::Get().getTextureResource(st),                             \
                    RZResourceManager::Get().getBufferResource(db));                             \
            } while (0)

        #define rzRHI_GenerateMipmaps(cb, text)                                              \
            do {                                                                             \
                RAZIX_PROFILE_SCOPEC("rzRHI_GenerateMipmaps", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.GenerateMipmaps(RZResourceManager::Get().getCommandBufferResource(cb), \
                    RZResourceManager::Get().getTextureResource(text));                      \
            } while (0)

        #define rzRHI_BlitTexture(cb, st, dt)                                            \
            do {                                                                         \
                RAZIX_PROFILE_SCOPEC("rzRHI_BlitTexture", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.BlitTexture(RZResourceManager::Get().getCommandBufferResource(cb), \
                    RZResourceManager::Get().getTextureResource(st),                     \
                    RZResourceManager::Get().getTextureResource(dt));                    \
            } while (0)

        #define rzRHI_ResolveTexture(cb, st, dt)                                            \
            do {                                                                            \
                RAZIX_PROFILE_SCOPEC("rzRHI_ResolveTexture", RZ_PROFILE_COLOR_RHI);         \
                g_RHI.ResolveTexture(RZResourceManager::Get().getCommandBufferResource(cb), \
                    RZResourceManager::Get().getTextureResource(st),                        \
                    RZResourceManager::Get().getTextureResource(dt));                       \
            } while (0)

        #define rzRHI_SignalGPU(so)                                                            \
            do {                                                                               \
                RAZIX_PROFILE_SCOPEC("rzRHI_SignalGPU", RZ_PROFILE_COLOR_RHI_SYNCHRONIZATION); \
                g_RHI.SignalGPU(so);                                                           \
            } while (0)

        #define rzRHI_FlushGPUWork(so)                                                            \
            do {                                                                                  \
                RAZIX_PROFILE_SCOPEC("rzRHI_FlushGPUWork", RZ_PROFILE_COLOR_RHI_SYNCHRONIZATION); \
                g_RHI.FlushGPUWork(so);                                                           \
            } while (0)

        #define rzRHI_ResizeSwapchain(sp, w, h)                                      \
            do {                                                                     \
                RAZIX_PROFILE_SCOPEC("rzRHI_ResizeSwapchain", RZ_PROFILE_COLOR_RHI); \
                g_RHI.ResizeSwapchain(sp, w, h);                                     \
            } while (0)

        #define rzRHI_BeginFrame(sc, wso, sso, fsps)                                             \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_BeginFrame", RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS); \
                g_RHI.BeginFrame(sc, wso, sso, fsps);                                            \
            } while (0)

        #define rzRHI_EndFrame(sc, fsso, psso, pwso, fsps, gsc)                                \
            do {                                                                               \
                RAZIX_PROFILE_SCOPEC("rzRHI_EndFrame", RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS); \
                g_RHI.EndFrame(sc, fsso, psso, pwso, fsps, gsc);                               \
            } while (0)

    #else
        // C mode or direct handles mode with profiling support where available
        #define rzRHI_AcquireImage(sc, sso)                                                              \
            do {                                                                                         \
                RAZIX_PROFILE_SCOPEC_BEGIN("rzRHI_AcquireImage", RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS); \
                g_RHI.AcquireImage(sc);                                                                  \
                RAZIX_PROFILE_SCOPEC_END();                                                              \
            } while (0)

        #define rzRHI_WaitOnPrevCmds(so)                                                            \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_WaitOnPrevCmds", RZ_PROFILE_COLOR_RHI_SYNCHRONIZATION); \
                g_RHI.WaitOnPrevCmds(so);                                                           \
                RAZIX_PROFILE_SCOPEC_END();                                                         \
            } while (0)

        #define rzRHI_SubmitCmdBuf(desc)                                                          \
            do {                                                                                  \
                RAZIX_PROFILE_SCOPEC("rzRHI_SubmitCmdBuf", RZ_PROFILE_COLOR_RHI_COMMAND_BUFFERS); \
                g_RHI.SubmitCmdBuf(desc);                                                         \
                RAZIX_PROFILE_SCOPEC_END();                                                       \
            } while (0)

        #define rzRHI_Present(desc)                                                           \
            do {                                                                              \
                RAZIX_PROFILE_SCOPEC("rzRHI_Present", RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS); \
                g_RHI.Present(desc);                                                          \
                RAZIX_PROFILE_SCOPEC_END();                                                   \
            } while (0)

        #define rzRHI_BeginCmdBuf(cb)                                                            \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_BeginCmdBuf", RZ_PROFILE_COLOR_RHI_COMMAND_BUFFERS); \
                g_RHI.BeginCmdBuf(cb);                                                           \
                RAZIX_PROFILE_SCOPEC_END();                                                      \
            } while (0)

        #define rzRHI_EndCmdBuf(cb)                                                            \
            do {                                                                               \
                RAZIX_PROFILE_SCOPEC("rzRHI_EndCmdBuf", RZ_PROFILE_COLOR_RHI_COMMAND_BUFFERS); \
                g_RHI.EndCmdBuf(cb);                                                           \
                RAZIX_PROFILE_SCOPEC_END();                                                    \
            } while (0)

        #define rzRHI_BeginRenderPass(cb, info)                                                    \
            do {                                                                                   \
                RAZIX_PROFILE_SCOPEC("rzRHI_BeginRenderPass", RZ_PROFILE_COLOR_RHI_RENDER_PASSES); \
                g_RHI.BeginRenderPass(cb, info);                                                   \
                RAZIX_PROFILE_SCOPEC_END();                                                        \
            } while (0)

        #define rzRHI_EndRenderPass(cb)                                                          \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_EndRenderPass", RZ_PROFILE_COLOR_RHI_RENDER_PASSES); \
                g_RHI.EndRenderPass(cb);                                                         \
                RAZIX_PROFILE_SCOPEC_END();                                                      \
            } while (0)

        #define rzRHI_SetScissorRect(cb, rect)                                      \
            do {                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_SetScissorRect", RZ_PROFILE_COLOR_RHI); \
                g_RHI.SetScissorRect(cb, rect);                                     \
                RAZIX_PROFILE_SCOPEC_END();                                         \
            } while (0)

        #define rzRHI_SetViewport(cb, viewport)                                  \
            do {                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_SetViewport", RZ_PROFILE_COLOR_RHI); \
                g_RHI.SetViewport(cb, viewport);                                 \
                RAZIX_PROFILE_SCOPEC_END();                                      \
            } while (0)

        #define rzRHI_BindPipeline(cb, pp)                                                       \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindPipeline", RZ_PROFILE_COLOR_RHI_PIPELINE_BINDS); \
                g_RHI.BindPipeline(cb, pp);                                                      \
                RAZIX_PROFILE_SCOPEC_END();                                                      \
            } while (0)

        #define rzRHI_BindGfxRootSig(cb, rs)                                                       \
            do {                                                                                   \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindGfxRootSig", RZ_PROFILE_COLOR_RHI_PIPELINE_BINDS); \
                g_RHI.BindGfxRootSig(cb, rs);                                                      \
                RAZIX_PROFILE_SCOPEC_END();                                                        \
            } while (0)

        #define rzRHI_BindComputeRootSig(cb, rs)                                                       \
            do {                                                                                       \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindComputeRootSig", RZ_PROFILE_COLOR_RHI_PIPELINE_BINDS); \
                g_RHI.BindComputeRootSig(cb, rs);                                                      \
                RAZIX_PROFILE_SCOPEC_END();                                                            \
            } while (0)

        #define rzRHI_BindDescriptorHeaps(cb, heaps, N)                                             \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindDescriptorHeaps", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.BindDescriptorHeaps(cb, heaps, N);                                            \
            } while (0);

        #define rzRHI_BindDescriptorHeapsContainer(cb, heaps, N)                                    \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindDescriptorHeaps", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.BindDescriptorHeaps(cb, heaps, N);                                            \
            } while (0)

        #define rzRHI_BindDescriptorTables(cb, ppt, dts, N)                                          \
            do {                                                                                     \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindDescriptorTables", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.BindDescriptorTables(cb, ppt, dts, N);                                         \
            } while (0);

        #define rzRHI_BindDescriptorTablesContainer(cb, ppt, dts, N)                                 \
            do {                                                                                     \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindDescriptorTables", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.BindDescriptorTables(cb, ppt, dts, N);                                         \
            } while (0)

        #define rzRHI_BindVertexBuffers(cb, bu, N, off, str)                                      \
            do {                                                                                  \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindVertexBuffers", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.BindVertexBuffers(cb, bu, N, off, str);                                     \
            } while (0);

        #define rzRHI_BindVertexBuffersContainer(cb, bu, off, str)                                \
            do {                                                                                  \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindVertexBuffers", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.BindVertexBuffers(cb, bu, off, str);                                        \
            } while (0)

        #define rzRHI_BindIndexBuffer(cb, bu, off, it)                               \
            do {                                                                     \
                RAZIX_PROFILE_SCOPEC("rzRHI_BindIndexBuffer", RZ_PROFILE_COLOR_RHI); \
                g_RHI.BindIndexBuffer(cb, bu, off, it);                              \
            } while (0)

        #define rzRHI_DrawAuto(cb, vc, ic, fv, fi)                                       \
            do {                                                                         \
                RAZIX_PROFILE_SCOPEC("rzRHI_DrawAuto", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.DrawAuto(cb, vc, ic, fv, fi);                                      \
                RAZIX_PROFILE_SCOPEC_END();                                              \
            } while (0)
        #define rzRHI_DrawIndexedAuto(cb, ic, icount, iv, fi, fo)                               \
            do {                                                                                \
                RAZIX_PROFILE_SCOPEC("rzRHI_DrawIndexedAuto", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.DrawIndexedAuto(cb, ic, icount, iv, fi, fo);                              \
                RAZIX_PROFILE_SCOPEC_END();                                                     \
            } while (0)
        #define rzRHI_Dispatch(cb, gx, gy, gz)                                           \
            do {                                                                         \
                RAZIX_PROFILE_SCOPEC("rzRHI_Dispatch", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.Dispatch(cb, gx, gy, gz);                                          \
                RAZIX_PROFILE_SCOPEC_END();                                              \
            } while (0)

        #define rzRHI_DrawIndirect(cb, bu, offset, maxDrawCount)                             \
            do {                                                                             \
                RAZIX_PROFILE_SCOPEC("rzRHI_DrawIndirect", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.DrawIndirect(cb, bu, offset, maxDrawCount);                            \
                RAZIX_PROFILE_SCOPEC_END();                                                  \
            } while (0)

        #define rzRHI_DrawIndexedIndirect(cb, bu, offset, maxDrawCount)                             \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_DrawIndexedIndirect", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.DrawIndexedIndirect(cb, bu, offset, maxDrawCount);                            \
                RAZIX_PROFILE_SCOPEC_END();                                                         \
            } while (0)

        #define rzRHI_DispatchIndirect(cb, bu, offset, maxDispatchCount)                         \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_DispatchIndirect", RZ_PROFILE_COLOR_RHI_DRAW_CALLS); \
                g_RHI.DispatchIndirect(cb, bu, offset, maxDispatchCount);                        \
                RAZIX_PROFILE_SCOPEC_END();                                                      \
            } while (0)

        #define rzRHI_UpdateDescriptorTable(dt, rv, N)                                     \
            do {                                                                           \
                RAZIX_PROFILE_SCOPEC("rzRHI_UpdateDescriptorTable", RZ_PROFILE_COLOR_RHI); \
                g_RHI.UpdateDescriptorTable(dt, rv, N);                                    \
                RAZIX_PROFILE_SCOPEC_END();                                                \
            } while (0);

        #define rzRHI_UpdateConstantBuffer(bu)                                            \
            do {                                                                          \
                RAZIX_PROFILE_SCOPEC("rzRHI_UpdateConstantBuffer", RZ_PROFILE_COLOR_RHI); \
                g_RHI.UpdateConstantBuffer(bu);                                           \
                RAZIX_PROFILE_SCOPEC_END();                                               \
            } while (0)

        #define rzRHI_InsertImageBarrier(cb, tex, bs, as)                                                 \
            do {                                                                                          \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertImageBarrier", RZ_PROFILE_COLOR_RHI_RESOURCE_BARRIERS); \
                g_RHI.InsertImageBarrier(cb, tex, bs, as);                                                \
                RAZIX_PROFILE_SCOPEC_END();                                                               \
            } while (0)

        #define rzRHI_InsertSwapchainImageBarrier(cb, tex, bs, as)                                                 \
            do {                                                                                                   \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertSwapchainImageBarrier", RZ_PROFILE_COLOR_RHI_RESOURCE_BARRIERS); \
                g_RHI.InsertImageBarrier(cb, tex, bs, as);                                                         \
                RAZIX_PROFILE_SCOPEC_END();                                                                        \
            } while (0)

        #define rzRHI_InsertBufferBarrier(cb, buf, bs, as)                                                 \
            do {                                                                                           \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertBufferBarrier", RZ_PROFILE_COLOR_RHI_RESOURCE_BARRIERS); \
                g_RHI.InsertBufferBarrier(cb, buf, bs, as);                                                \
                RAZIX_PROFILE_SCOPEC_END();                                                                \
            } while (0)

        #define rzRHI_InsertTextureReadback(tex, rb)                                       \
            do {                                                                           \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertTextureReadback", RZ_PROFILE_COLOR_RHI); \
                g_RHI.InsertTextureReadback(tex, rb);                                      \
                RAZIX_PROFILE_SCOPEC_END();                                                \
            } while (0)

        #define rzRHI_InsertSwapchainTextureReadback(tex, rb)                                       \
            do {                                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertSwapchainTextureReadback", RZ_PROFILE_COLOR_RHI); \
                g_RHI.InsertTextureReadback(tex, rb);                                               \
                RAZIX_PROFILE_SCOPEC_END();                                                         \
            } while (0)

        #define rzRHI_InsertBufferReadback(buf, rb)                                       \
            do {                                                                          \
                RAZIX_PROFILE_SCOPEC("rzRHI_InsertBufferReadback", RZ_PROFILE_COLOR_RHI); \
                g_RHI.InsertBufferReadback(buf, rb);                                      \
                RAZIX_PROFILE_SCOPEC_END();                                               \
            } while (0)

        #define rzRHI_CopyBuffer(cb, sb, db, s, so, doff)                       \
            do {                                                                \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyBuffer", RZ_PROFILE_COLOR_RHI); \
                g_RHI.CopyBuffer(cb, sb, db, s, so, doff);                      \
                RAZIX_PROFILE_SCOPEC_END();                                     \
            } while (0)

        #define rzRHI_CopyTexture(cb, st, dt)                                    \
            do {                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyTexture", RZ_PROFILE_COLOR_RHI); \
                g_RHI.CopyTexture(cb, st, dt);                                   \
                RAZIX_PROFILE_SCOPEC_END();                                      \
            } while (0)

        #define rzRHI_CopyTextureToSwapchain(cb, st, dt)                                    \
            do {                                                                            \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyTextureToSwapchain", RZ_PROFILE_COLOR_RHI); \
                g_RHI.CopyTexture(cb, st, dt);                                              \
                RAZIX_PROFILE_SCOPEC_END();                                                 \
            } while (0)

        #define rzRHI_CopyBufferToTexture(cb, sb, dt)                                    \
            do {                                                                         \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyBufferToTexture", RZ_PROFILE_COLOR_RHI); \
                g_RHI.CopyBufferToTexture(cb, sb, dt);                                   \
                RAZIX_PROFILE_SCOPEC_END();                                              \
            } while (0)

        #define rzRHI_CopyTextureToBuffer(cb, st, db)                                    \
            do {                                                                         \
                RAZIX_PROFILE_SCOPEC("rzRHI_CopyTextureToBuffer", RZ_PROFILE_COLOR_RHI); \
                g_RHI.CopyTextureToBuffer(cb, st, db);                                   \
                RAZIX_PROFILE_SCOPEC_END();                                              \
            } while (0)

        #define rzRHI_GenerateMipmaps(cb, text)                                      \
            do {                                                                     \
                RAZIX_PROFILE_SCOPEC("rzRHI_GenerateMipmaps", RZ_PROFILE_COLOR_RHI); \
                g_RHI.GenerateMipmaps(cb, text);                                     \
                RAZIX_PROFILE_SCOPEC_END();                                          \
            } while (0)

        #define rzRHI_BlitTexture(cb, st, dt)                                    \
            do {                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_BlitTexture", RZ_PROFILE_COLOR_RHI); \
                g_RHI.BlitTexture(cb, st, dt);                                   \
                RAZIX_PROFILE_SCOPEC_END();                                      \
            } while (0)

        #define rzRHI_ResolveTexture(cb, st, dt)                                    \
            do {                                                                    \
                RAZIX_PROFILE_SCOPEC("rzRHI_ResolveTexture", RZ_PROFILE_COLOR_RHI); \
                g_RHI.ResolveTexture(cb, st, dt);                                   \
                RAZIX_PROFILE_SCOPEC_END();                                         \
            } while (0)

        #define rzRHI_SignalGPU(so)                                                            \
            do {                                                                               \
                RAZIX_PROFILE_SCOPEC("rzRHI_SignalGPU", RZ_PROFILE_COLOR_RHI_SYNCHRONIZATION); \
                g_RHI.SignalGPU(so, sp);                                                       \
                RAZIX_PROFILE_SCOPEC_END();                                                    \
            } while (0)

        #define rzRHI_FlushGPUWork(so)                                                            \
            do {                                                                                  \
                RAZIX_PROFILE_SCOPEC("rzRHI_FlushGPUWork", RZ_PROFILE_COLOR_RHI_SYNCHRONIZATION); \
                g_RHI.FlushGPUWork(so);                                                           \
                RAZIX_PROFILE_SCOPEC_END();                                                       \
            } while (0)

        #define rzRHI_ResizeSwapchain(sp, w, h)                                      \
            do {                                                                     \
                RAZIX_PROFILE_SCOPEC("rzRHI_ResizeSwapchain", RZ_PROFILE_COLOR_RHI); \
                g_RHI.ResizeSwapchain(sp, w, h);                                     \
                RAZIX_PROFILE_SCOPEC_END();                                          \
            } while (0)

        #define rzRHI_BeginFrame(sc, wso, sso, fsps)                                             \
            do {                                                                                 \
                RAZIX_PROFILE_SCOPEC("rzRHI_BeginFrame", RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS); \
                g_RHI.BeginFrame(sc, wso, sso, fsps);                                            \
                RAZIX_PROFILE_SCOPEC_END();                                                      \
            } while (0)

        #define rzRHI_EndFrame(sc, fsso, psso, pwso, fsps, gsc)                                \
            do {                                                                               \
                RAZIX_PROFILE_SCOPEC("rzRHI_EndFrame", RZ_PROFILE_COLOR_RHI_FRAME_OPERATIONS); \
                g_RHI.EndFrame(sc, fsso, psso, pwso, fsps, gsc);                               \
                RAZIX_PROFILE_SCOPEC_END();                                                    \
            } while (0)
    #endif    // defined(RAZIX_RHI_USE_RESOURCE_MANAGER_HANDLES) && defined(__cplusplus)

#endif    // !defined(RZ_PROFILER_ENABLED)

#ifdef __cplusplus
}
#endif    // __cplusplus
#endif    // RHI_H
