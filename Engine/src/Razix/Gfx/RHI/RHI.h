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

#define RAZIX_PUSH_CONSTANT_REFLECTION_NAME_PREFIX "PushConstant"
#define RAZIX_PUSH_CONSTANT_REFLECTION_NAME_VK     RAZIX_PUSH_CONSTANT_REFLECTION_NAME_PREFIX
#define RAZIX_PUSH_CONSTANT_REFLECTION_NAME_DX12   "PushConstantBuffer"

#define RAZIX_EXTENTS_ELEM_COUNT 2
#define RAZIX_X(v)               ((v)[0])
#define RAZIX_Y(v)               ((v)[1])
#define RAZIX_Z(v)               ((v)[2])
#define RAZIX_W(v)               ((v)[3])

#define RAZIX_MAX_SHADER_SOURCE_SIZE 1024 * 1024
#define RAZIX_MAX_LINE_LENGTH        1024
#define RAZIX_MAX_SHADER_STAGES      RZ_GFX_SHADER_STAGE_COUNT
#define RAZIX_MAX_INCLUDE_DEPTH      16

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
        RZ_GFX_TEXTURE_TYPE_CUBE_ARRAY
    } rz_gfx_texture_type;

    typedef enum rz_gfx_cmdpool_type
    {
        RZ_GFX_CMDPOOL_TYPE_GRAPHICS,    // Graphics Command Pool
        RZ_GFX_CMDPOOL_TYPE_COMPUTE,     // Compute Command Pool
        RZ_GFX_CMDPOOL_TYPE_TRANSFER,    // Transfer Command Pool
    } rz_gfx_cmdpool_type;

    typedef enum rz_gfx_resolution
    {
        RZ_GFX_RESOLUTION_1080p,       /* native HD resolution 1920x1080 rendering                            */
        RZ_GFX_RESOLUTION_1440p,       /* native 2K resolution 2560x1440 rendering                            */
        RZ_GFX_RESOLUTION_4K_UPSCALED, /* Upscaled using FSR/DLSS                                             */
        RZ_GFX_RESOLUTION_4K_NATIVE,   /* native 3840x2160 rendering                                          */
        RZ_GFX_RESOLUTION_WINDOW,      /* Selects the resolution dynamically based on the presentation window */
        RZ_GFX_RESOLUTION_CUSTOM,      /* Custom resolution for rendering                                     */
    } rz_gfx_resolution;

    typedef enum rz_gfx_resource_state
    {
        RZ_GFX_RESOURCE_STATE_UNDEFINED = 0,
        RZ_GFX_RESOURCE_STATE_RENDER_TARGET,
        RZ_GFX_RESOURCE_STATE_SHADER_READ,
        RZ_GFX_RESOURCE_STATE_COPY_SRC,
        RZ_GFX_RESOURCE_STATE_COPY_DST,
        RZ_GFX_RESOURCE_STATE_PRESENT,
        RZ_GFX_RESOURCE_STATE_DEPTH_WRITE,
        RZ_GFX_RESOURCE_STATE_GENERAL,
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
        RZ_GFX_DESCRIPTOR_TYPE_NONE           = 0xFFFFFFFF,
        RZ_GFX_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 0,
        RZ_GFX_DESCRIPTOR_TYPE_PUSH_CONSTANT,
        RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED,    // (Vulkan-only, not recommended)
        RZ_GFX_DESCRIPTOR_TYPE_TEXTURE,
        RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE,
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
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_CBV_UAV_SRV = 0,
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_SAMPLER,
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_RTV,
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_DSV,
        RZ_GFX_DESCRIPTOR_HEAP_TYPE_COUNT
    } rz_gfx_descriptor_heap_type;

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
        RZ_GFX_SHADER_DATA_TYPE_MAT4_ARRAY
    } rz_gfx_shader_data_type;

    typedef enum rz_gfx_texture_wrap_type
    {
        RZ_GFX_TEXTURE_WRAP_TYPE_REPEAT = 0,
        RZ_GFX_TEXTURE_WRAP_TYPE_MIRRORED_REPEAT,
        RZ_GFX_TEXTURE_WRAP_TYPE_CLAMP_TO_EDGE,
        RZ_GFX_TEXTURE_WRAP_TYPE_CLAMP_TO_BORDER,
        RZ_GFX_TEXTURE_WRAP_TYPE_COUNT
    } rz_gfx_texture_wrap_type;

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
        RZ_GFX_POLYGON_MODE_TYPE_FILL = 0,
        RZ_GFX_POLYGON_MODE_TYPE_LINE,
        RZ_GFX_POLYGON_MODE_TYPE_POINT,
        RZ_GFX_POLYGON_MODE_TYPE_COUNT
    } rz_gfx_polygon_mode_type;

    typedef enum rz_gfx_draw_type
    {
        RZ_GFX_DRAW_TYPE_POINT = 0,
        RZ_GFX_DRAW_TYPE_TRIANGLE,
        RZ_GFX_DRAW_TYPE_LINE,
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
        RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM,
        RZ_GFX_BUFFER_USAGE_TYPE_STAGING,
        RZ_GFX_BUFFER_USAGE_TYPE_INDIRECT_DRAW_ARGS,
        RZ_GFX_BUFFER_USAGE_TYPE_READBACK,
        RZ_GFX_BUFFER_USAGE_TYPE_COUNT
    } rz_gfx_buffer_usage_type;

    typedef enum rz_gfx_buffer_type
    {
        RZ_GFX_BUFFER_TYPE_CONSTANT = 0,
        RZ_GFX_BUFFER_TYPE_RW_CONSTANT,
        RZ_GFX_BUFFER_TYPE_STORAGE,
        RZ_GFX_BUFFER_TYPE_RW_STRUCTURED,
        RZ_GFX_BUFFER_TYPE_RW_DATA,
        RZ_GFX_BUFFER_TYPE_RW_REGULAR,
        RZ_GFX_BUFFER_TYPE_STRUCTURED,
        RZ_GFX_BUFFER_TYPE_DATA,
        RZ_GFX_BUFFER_TYPE_REGULAR,
        RZ_GFX_BUFFER_TYPE_ACCELERATION_STRUCTURE,
        RZ_GFX_BUFFER_TYPE_COUNT
    } rz_gfx_buffer_type;

    typedef enum rz_gfx_pipeline_stage_type
    {
        RZ_GFX_PIPELINE_STAGE_TYPE_TOP_OF_PIPE = 0,
        RZ_GFX_PIPELINE_STAGE_TYPE_DRAW_INDIRECT,
        RZ_GFX_PIPELINE_STAGE_TYPE_DRAW,
        RZ_GFX_PIPELINE_STAGE_TYPE_VERTEX_INPUT,
        RZ_GFX_PIPELINE_STAGE_TYPE_VERTEX_SHADER,
        RZ_GFX_PIPELINE_STAGE_TYPE_TESSELLATION_CONTROL_SHADER,
        RZ_GFX_PIPELINE_STAGE_TYPE_TESSELLATION_EVALUATION_SHADER,
        RZ_GFX_PIPELINE_STAGE_TYPE_GEOMETRY_SHADER,
        RZ_GFX_PIPELINE_STAGE_TYPE_FRAGMENT_SHADER,
        RZ_GFX_PIPELINE_STAGE_TYPE_EARLY_FRAGMENT_TESTS,
        RZ_GFX_PIPELINE_STAGE_TYPE_LATE_FRAGMENT_TESTS,
        RZ_GFX_PIPELINE_STAGE_TYPE_EARLY_OR_LATE_TESTS,
        RZ_GFX_PIPELINE_STAGE_TYPE_COLOR_ATTACHMENT_OUTPUT,
        RZ_GFX_PIPELINE_STAGE_TYPE_COMPUTE_SHADER,
        RZ_GFX_PIPELINE_STAGE_TYPE_TRANSFER,
        RZ_GFX_PIPELINE_STAGE_TYPE_MESH_SHADER,
        RZ_GFX_PIPELINE_STAGE_TYPE_TASK_SHADER,
        RZ_GFX_PIPELINE_STAGE_TYPE_BOTTOM_OF_PIPE,
        RZ_GFX_PIPELINE_STAGE_TYPE_COUNT
    } rz_gfx_pipeline_stage_type;

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
        RZ_GFX_DRAW_DATA_TYPE_UNSIGNED_BYTE
    } rz_gfx_draw_data_type;

    typedef enum rz_gfx_blend_presets
    {
        RZ_GFX_BLEND_PRESET_ADDITIVE = 0,
        RZ_GFX_BLEND_PRESET_ALPHA_BLEND,
        RZ_GFX_BLEND_PRESET_SUBTRACTIVE,
        RZ_GFX_BLEND_PRESET_MULTIPLY,
        RZ_GFX_BLEND_PRESET_DARKEN
    } rz_gfx_blend_presets;

    typedef enum rz_gfx_target_fps
    {
        RZ_GFX_TARGET_FPS_60  = 60,
        RZ_GFX_TARGET_FPS_120 = 120
    } rz_gfx_target_fps;

    typedef rz_handle rz_gfx_texture_handle;
    typedef rz_handle rz_gfx_cmdbuf_handle;
    typedef rz_handle rz_gfx_root_signature_handle;
    typedef rz_handle rz_gfx_shader_handle;
    typedef rz_handle rz_gfx_swapchain_handle;
    typedef rz_handle rz_gfx_syncobj_handle;
    typedef rz_handle rz_gfx_cmdpool_handle;
    typedef rz_handle rz_gfx_descriptor_heap_handle;
    typedef rz_handle rz_gfx_descriptor_table_handle;
    typedef rz_handle rz_gfx_pipeline_handle;

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
        bool     SupportsBindlessRendering;
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

    typedef struct rz_gfx_texture_desc
    {
        uint32_t            width;
        uint32_t            height;
        uint32_t            depth;
        uint32_t            mipLevels;
        uint32_t            arraySize;
        rz_gfx_format       format;
        rz_gfx_texture_type textureType;
    } rz_gfx_texture_desc;

    typedef struct rz_gfx_texture
    {
        RAZIX_GFX_RESOURCE;
        rz_gfx_texture_desc desc;
#ifdef RAZIX_RENDER_API_VULKAN
            //vk_texture vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_texture dx12;
#endif
    } rz_gfx_texture;

    typedef struct rz_gfx_swapchain
    {
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

    typedef struct rz_gfx_cmdpool
    {
        rz_gfx_cmdpool_type type;    // Type of the command pool, e.g. Graphics, Compute, Transfer
        union
        {
#ifdef RAZIX_RENDER_API_VULKAN
            vk_cmdpool vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
            dx12_cmdpool dx12;
#endif
        };

    } rz_gfx_cmdpool;

    typedef struct rz_gfx_cmdbuf_desc
    {
        const rz_gfx_cmdpool* pool;
    } rz_gfx_cmdbuf_desc;

    typedef struct rz_gfx_cmdbuf
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

    } rz_gfx_cmdbuf;

    typedef struct rz_gfx_color_rgba
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

    typedef struct rz_gfx_color_rgb
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
    } rz_gfx_color_rgb;

    typedef struct rz_gfx_attachment
    {
        rz_gfx_color_rgba     clearColor;
        const rz_gfx_texture* texture;
        uint8_t               mip;
        uint8_t               layer;
        bool                  clear;
    } gfx_attachment;

    typedef struct rz_gfx_viewport
    {
        int32_t  x, y;
        uint32_t width, height;
        uint32_t minDepth;
        uint32_t maxDepth;
    } rz_gfx_viewport;

    typedef struct rz_gfx_rect
    {
        int32_t  x, y;
        uint32_t width, height;
    } rz_gfx_rect;

    typedef struct rz_gfx_renderpass
    {
        uint32_t          colorAttachmentsCount;
        uint32_t          _pad0;
        gfx_attachment    colorAttachments[RAZIX_MAX_RENDER_TARGETS];
        gfx_attachment    depthAttachment;
        uint32_t          extents[RAZIX_EXTENTS_ELEM_COUNT];
        uint32_t          layers;
        rz_gfx_resolution resolution;
    } rz_gfx_renderpass;

    typedef struct rz_gfx_descriptor_heap_desc
    {
        const char*                 name;
        rz_gfx_descriptor_heap_type heap_type;
        uint32_t                    descriptor_count;
    } rz_gfx_descriptor_heap_desc;

    typedef struct rz_gfx_descriptor_heap
    {
        RAZIX_GFX_RESOURCE;
        rz_gfx_descriptor_heap_desc desc;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_descriptor_heap vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_descriptor_heap dx12;
#endif
    } rz_gfx_descriptor_heap;

    typedef struct rz_gfx_binding_location
    {
        uint32_t binding;
        uint32_t space;
    } rz_gfx_binding_location;

    typedef struct rz_gfx_descriptor
    {
        const char*             name;
        rz_gfx_binding_location location;
        rz_gfx_descriptor_type  type;
        uint32_t                arraySize;
        uint32_t                sizeInBytes;
        uint32_t                offsetInBytes;
        uint32_t                memberCount;
    } rz_gfx_descriptor;

    typedef struct rz_gfx_descriptor_table_desc
    {
        uint32_t           tableIndex;
        rz_gfx_descriptor* descriptors;
        uint32_t           descriptorCount;
    } rz_gfx_descriptor_table_desc;

    typedef struct rz_gfx_root_constant_desc
    {
        rz_gfx_binding_location location;
        uint32_t                sizeInBytes;
        uint32_t                offsetInBytes;
        rz_gfx_shader_stage     shaderStage;
        const char*             typeNameStr;
    } rz_gfx_root_constant_desc;

    typedef struct rz_gfx_root_signature_desc
    {
        rz_gfx_descriptor_table_desc* descriptorTables;
        uint32_t                      descriptorTableCount;
        rz_gfx_root_constant_desc*    rootConstants;
        uint32_t                      rootConstantCount;
    } rz_gfx_root_signature_desc;

    typedef struct rz_gfx_descriptor_table
    {
        RAZIX_GFX_RESOURCE;
        rz_gfx_descriptor_table_desc desc;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_descriptor_table vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_descriptor_table dx12;
#endif
    } rz_gfx_descriptor_table;

    typedef struct rz_gfx_root_signature
    {
        RAZIX_GFX_RESOURCE;
#ifdef RAZIX_RENDER_API_VULKAN
        vk_root_signature vk;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        dx12_root_signature dx12;
#endif
    } rz_gfx_root_signature;

    typedef struct rz_gfx_shader_stage_file
    {
        rz_gfx_shader_stage stage;
        const char*         filePath;    // Path to .cso/.spv/etc
    } rz_gfx_shader_stage_file;

    typedef struct rz_gfx_shader_desc
    {
        const char*          name;
        rz_gfx_pipeline_type pipelineType;

        union
        {
            struct
            {
                rz_gfx_shader_stage_file vs;
                rz_gfx_shader_stage_file ps;
                rz_gfx_shader_stage_file gs;
                rz_gfx_shader_stage_file tcs;
                rz_gfx_shader_stage_file tes;
            } raster;

            struct
            {
                rz_gfx_shader_stage_file cs;
            } compute;

            struct
            {
                rz_gfx_shader_stage_file task;
                rz_gfx_shader_stage_file mesh;
                rz_gfx_shader_stage_file ps;
            } mesh;

            struct
            {
                rz_gfx_shader_stage_file rgen;
                rz_gfx_shader_stage_file miss;
                rz_gfx_shader_stage_file chit;
                rz_gfx_shader_stage_file ahit;
                rz_gfx_shader_stage_file callable;
            } raytracing;
        };
    } rz_gfx_shader_desc;

    typedef struct rz_gfx_shader
    {
        RAZIX_GFX_RESOURCE;
        rz_gfx_shader_desc           desc;
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
    } rz_gfx_shader;

    // TODO: Pipeline create desc and pipeline structs
    typedef struct rz_gfx_pipeline_desc
    {
        const char*                  name;
        rz_gfx_pipeline_type         type;
        rz_gfx_shader_handle         shader;
        rz_gfx_root_signature_handle rootSignature;
        rz_gfx_cull_mode_type        cullMode;
        rz_gfx_polygon_mode_type     polygonMode;
        bool                         depthTestEnabled;
        bool                         depthWriteEnabled;
        bool                         stencilTestEnabled;
        bool                         blendEnabled;
        rz_gfx_blend_presets         blendPreset;
    } rz_gfx_pipeline_desc;

    //---------------------------------------------------------------------------------------------
    // Gfx API

    RAZIX_API void rzGfxCtx_StartUp();
    RAZIX_API void rzGfxCtx_ShutDown();

    RAZIX_API rz_render_api rzGfxCtx_GetRenderAPI();
    RAZIX_API void          rzGfxCtx_SetRenderAPI(rz_render_api api);
    RAZIX_API const char*   rzGfxCtx_GetRenderAPIString();

    RAZIX_API rz_gfx_shader_stage rzGfx_StringToShaderStage(const char* stage);

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

    typedef void (*rzRHI_CreateCmdPoolFn)(void* where, rz_gfx_cmdpool_type);
    typedef void (*rzRHI_DestroyCmdPoolFn)(rz_gfx_cmdpool*);

    typedef void (*rzRHI_CreateCmdBufFn)(void* where, rz_gfx_cmdbuf_desc desc);
    typedef void (*rzRHI_DestroyCmdBufFn)(rz_gfx_cmdbuf*);

    typedef void                  (*rzRHI_CreateShaderFn)(void* where, rz_gfx_shader_desc desc);
    typedef void                  (*rzRHI_DestroyShaderFn)(rz_gfx_shader* shader);
    typedef rz_gfx_root_signature (*rzRHI_ReflectShaderFn)(const rz_gfx_shader* shaderDesc);

    typedef void (*rzRHI_CreateRootSignatureFn)(void* where, rz_gfx_root_signature_desc desc);
    typedef void (*rzRHI_DestroyRootSignatureFn)(rz_gfx_root_signature*);

    typedef void (*rzRHI_CreateDescriptorHeapFn)(void* where, rz_gfx_descriptor_heap_desc desc);
    typedef void (*rzRHI_DestroyDescriptorHeapFn)(rz_gfx_descriptor_heap*);

    typedef void (*rzRHI_CreateDescriptorTableFn)(void* where, rz_gfx_descriptor_table_desc desc);

    /**
     * RHI API
     */
    typedef void (*rzRHI_BeginFrameFn)(rz_gfx_swapchain*, const rz_gfx_syncobj*, rz_gfx_syncpoint* frameSyncpoints, rz_gfx_syncpoint* globalSyncpoint);
    typedef void (*rzRHI_EndFrameFn)(const rz_gfx_swapchain*, const rz_gfx_syncobj*, rz_gfx_syncpoint* frameSyncpoints, rz_gfx_syncpoint* globalSyncpoint);

    typedef void (*rzRHI_AcquireImageFn)(rz_gfx_swapchain*);
    typedef void (*rzRHI_WaitOnPrevCmdsFn)(const rz_gfx_syncobj*, rz_gfx_syncpoint);
    typedef void (*rzRHI_PresentFn)(const rz_gfx_swapchain*);

    typedef void (*rzRHI_BeginCmdBufFn)(const rz_gfx_cmdbuf*);
    typedef void (*rzRHI_EndCmdBufFn)(const rz_gfx_cmdbuf*);
    typedef void (*rzRHI_SubmitCmdBufFn)(rz_gfx_cmdbuf*);

    typedef void (*rzRHI_BeginRenderPassFn)(const rz_gfx_cmdbuf*, rz_gfx_renderpass);
    typedef void (*rzRHI_EndRenderPassFn)(const rz_gfx_cmdbuf*);

    typedef void (*rzRHI_SetViewportFn)(rz_gfx_cmdbuf* cmdBuf, const rz_gfx_viewport* viewport);
    typedef void (*rzRHI_SetScissorRectFn)(rz_gfx_cmdbuf* cmdBuf, const rz_gfx_rect* rect);

    typedef void (*rzRHI_InsertImageBarrierFn)(rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture*, rz_gfx_resource_state, rz_gfx_resource_state);

    typedef rz_gfx_syncpoint (*rzRHI_SignalGPUFn)(const rz_gfx_syncobj*, rz_gfx_syncpoint*);
    typedef void             (*rzRHI_FlushGPUWorkFn)(const rz_gfx_syncobj*, rz_gfx_syncpoint*);
    typedef void             (*rzRHI_ResizeSwapchainFn)(rz_gfx_swapchain*, uint32_t, uint32_t);

    typedef struct rz_rhi_api
    {
        rzRHI_GlobalCtxInitFn    GlobalCtxInit;
        rzRHI_GlobalCtxDestroyFn GlobalCtxDestroy;
        //-----------------------------------------
        rzRHI_CreateSyncobjFn        CreateSyncobj;
        rzRHI_DestroySyncobjFn       DestroySyncobj;
        rzRHI_CreateSwapchainFn      CreateSwapchain;
        rzRHI_DestroySwapchainFn     DestroySwapchain;
        rzRHI_CreateCmdPoolFn        CreateCmdPool;
        rzRHI_DestroyCmdPoolFn       DestroyCmdPool;
        rzRHI_CreateCmdBufFn         CreateCmdBuf;
        rzRHI_DestroyCmdBufFn        DestroyCmdBuf;
        rzRHI_CreateShaderFn         CreateShader;
        rzRHI_DestroyShaderFn        DestroyShader;
        rzRHI_ReflectShaderFn        ReflectShader;
        rzRHI_CreateRootSignatureFn  CreateRootSignature;
        rzRHI_DestroyRootSignatureFn DestroyRootSignature;
        //....
        rzRHI_CreateDescriptorHeapFn  CreateDescriptorHeap;
        rzRHI_DestroyDescriptorHeapFn DestroyDescriptorHeap;
        rzRHI_CreateDescriptorTableFn CreateDescriptorTable;

        rzRHI_AcquireImageFn       AcquireImage;
        rzRHI_WaitOnPrevCmdsFn     WaitOnPrevCmds;
        rzRHI_PresentFn            Present;
        rzRHI_BeginCmdBufFn        BeginCmdBuf;
        rzRHI_EndCmdBufFn          EndCmdBuf;
        rzRHI_SubmitCmdBufFn       SubmitCmdBuf;
        rzRHI_BeginRenderPassFn    BeginRenderPass;
        rzRHI_EndRenderPassFn      EndRenderPass;
        rzRHI_SetScissorRectFn     SetScissorRect;
        rzRHI_SetViewportFn        SetViewport;
        rzRHI_InsertImageBarrierFn InsertImageBarrier;

        // ....

        rzRHI_SignalGPUFn       SignalGPU;
        rzRHI_FlushGPUWorkFn    FlushGPUWork;
        rzRHI_ResizeSwapchainFn ResizeSwapchain;
        //-----------------------------------------
        rzRHI_BeginFrameFn BeginFrame;
        rzRHI_EndFrameFn   EndFrame;
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

#define rzRHI_CreateSyncobj         g_RHI.CreateSyncobj
#define rzRHI_DestroySyncobj        g_RHI.DestroySyncobj
#define rzRHI_CreateSwapchain       g_RHI.CreateSwapchain
#define rzRHI_DestroySwapchain      g_RHI.DestroySwapchain
#define rzRHI_CreateCmdPool         g_RHI.CreateCmdPool
#define rzRHI_DestroyCmdPool        g_RHI.DestroyCmdPool
#define rzRHI_CreateCmdBuf          g_RHI.CreateCmdBuf
#define rzRHI_DestroyCmdBuf         g_RHI.DestroyCmdBuf
#define rzRHI_CreateShader          g_RHI.CreateShader
#define rzRHI_DestroyShader         g_RHI.DestroyShader
#define rzRHI_ReflectShader         g_RHI.ReflectShader
#define rzRHI_CreateDescriptorHeap  g_RHI.CreateDescriptorHeap
#define rzRHI_DestroyDescriptorHeap g_RHI.DestroyDescriptorHeap
#define rzRHI_CreateDescriptorTable g_RHI.CreateDescriptorTable
#define rzRHI_CreateRootSignature   g_RHI.CreateRootSignature
#define rzRHI_DestroyRootSignature  g_RHI.DestroyRootSignature

#define rzRHI_AcquireImage       g_RHI.AcquireImage
#define rzRHI_WaitOnPrevCmds     g_RHI.WaitOnPrevCmds
#define rzRHI_Present            g_RHI.Present
#define rzRHI_BeginCmdBuf        g_RHI.BeginCmdBuf
#define rzRHI_EndCmdBuf          g_RHI.EndCmdBuf
#define rzRHI_SubmitCmdBuf       g_RHI.SubmitCmdBuf
#define rzRHI_BeginRenderPass    g_RHI.BeginRenderPass
#define rzRHI_EndRenderPass      g_RHI.EndRenderPass
#define rzRHI_SetScissorRect     g_RHI.SetScissorRect
#define rzRHI_SetViewport        g_RHI.SetViewport
#define rzRHI_InsertImageBarrier g_RHI.InsertImageBarrier

#define rzRHI_SignalGPU       g_RHI.SignalGPU
#define rzRHI_FlushGPUWork    g_RHI.FlushGPUWork
#define rzRHI_ResizeSwapchain g_RHI.ResizeSwapchain
#define rzRHI_BeginFrame      g_RHI.BeginFrame
#define rzRHI_EndFrame        g_RHI.EndFrame

#ifdef __cplusplus
}
#endif    // __cplusplus
#endif    // RHI_H
