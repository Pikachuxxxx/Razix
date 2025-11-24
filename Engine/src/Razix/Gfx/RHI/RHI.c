#include "RHI.h"

#include <math.h>

RAZIX_RHI_API rz_gfx_context g_GfxCtx = {0};
// Default
RAZIX_RHI_API rz_render_api   g_RenderAPI        = RZ_RENDER_API_VULKAN;
RAZIX_RHI_API rz_rhi_api      g_RHI              = {0};
RAZIX_RHI_API rz_gfx_features g_GraphicsFeatures = {0};

// Defined in vk/dx12_rhi.c
RAZIX_RHI_API extern const rz_rhi_api vk_rhi;
RAZIX_RHI_API extern const rz_rhi_api dx12_rhi;

void rzGfxCtx_StartUp(rz_gfx_context_desc init)
{
    switch (g_RenderAPI) {
            // Load the DLL and then initialize RHI table
        case RZ_RENDER_API_VULKAN:
            RAZIX_RHI_LOG_INFO("Initializing Vulkan RHI");
            g_RHI = vk_rhi;
            break;
#if defined(RAZIX_PLATFORM_WINDOWS)
        case RZ_RENDER_API_D3D12:
            RAZIX_RHI_LOG_INFO("Initializing DirectX 12 RHI");
            g_RHI = dx12_rhi;
            break;
#endif
        default:
            RAZIX_RHI_LOG_ERROR("Unsupported Render API: %d", (uint32_t) g_RenderAPI);
            RAZIX_RHI_LOG_ERROR("Falling back to Vulkan");
            g_RenderAPI = RZ_RENDER_API_VULKAN;
            g_RHI       = vk_rhi;
            break;
    }

    rzGfxCtx_GlobalCtxInit(init);
}

void rzGfxCtx_ShutDown()
{
    rzGfxCtx_GlobalCtxDestroy();
    // IDK what to do here yet, but we need to clean up the RHI resources
}

rz_render_api rzGfxCtx_GetRenderAPI()
{
    return g_RenderAPI;
}

void rzGfxCtx_SetRenderAPI(rz_render_api api)
{
    g_RenderAPI = api;
}

const char* rzGfxCtx_GetRenderAPIString()
{
    switch (g_RenderAPI) {
        case RZ_RENDER_API_VULKAN: return "Vulkan"; break;
        case RZ_RENDER_API_D3D12: return "DirectX 12"; break;
        case RZ_RENDER_API_GXM: return "SCE GXM (PSVita)"; break;
        case RZ_RENDER_API_GCM: return "SCE GCM (PS3)"; break;
        case RZ_RENDER_API_AGC: return "SCE AGC (PS5)"; break;
        default: return "None"; break;
    }
}

const char* rzRHI_GetGPUVendorName(uint32_t vid)
{
    switch (vid) {
        case 0x1002: return "AMD";
        case 0x10DE: return "NVIDIA";
        case 0x8086: return "Intel";
        case 0x13B5: return "ARM";
        case 0x5143: return "Qualcomm";
        case 0x1010: return "ImgTec";
        case 0x1AE0: return "Apple";
        default: return "Unknown";
    }
}

bool rzRHI_IsDescriptorTypeTexture(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_TEXTURE:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE:
        case RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED:
        case RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE:
        case RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE:
            return true;
        default:
            return false;
    }
}

bool rzRHI_IsDescriptorTypeBuffer(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TYPED:
        case RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_BYTE_ADDRESS:
        case RZ_GFX_DESCRIPTOR_TYPE_APPEND_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_CONSUME_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED_COUNTER:
            return true;
        default:
            return false;
    }
}

bool rzRHI_IsDescriptorTypeSampler(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_SAMPLER:
        case RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED:
            return true;
        default:
            return false;
    }
}

bool rzRHI_IsDescriptorTypeTextureRW(rz_gfx_descriptor_type type)
{
    // Only return true for texture types that are explicitly read-write
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE:
            return true;
        case RZ_GFX_DESCRIPTOR_TYPE_TEXTURE:
        case RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED:
            return false;    // These are read-only texture types
        default:
            return false;    // Not a texture type
    }
}

bool rzRHI_IsDescriptorTypeBufferRW(rz_gfx_descriptor_type type)
{
    // Return true for buffer types that are explicitly read-write
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TYPED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_BYTE_ADDRESS:
        case RZ_GFX_DESCRIPTOR_TYPE_APPEND_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_CONSUME_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED_COUNTER:
            return true;
        case RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER:
        case RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS:
            return false;    // These are read-only buffer types
        default:
            return false;    // Not a buffer type
    }
}

bool rzRHI_IsDepthFormat(rz_gfx_format format)
{
    switch (format) {
        case RZ_GFX_FORMAT_D16_UNORM:
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT:
        case RZ_GFX_FORMAT_D32_FLOAT:
        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT:
            return true;
        default:
            return false;
    }
}

uint32_t rzRHI_GetBytesPerPixel(rz_gfx_format format)
{
    switch (format) {
        // Undefined format
        case RZ_GFX_FORMAT_UNDEFINED:
            return 0;

        // 8-bit formats (1 byte per component)
        case RZ_GFX_FORMAT_R8_UNORM:
        case RZ_GFX_FORMAT_R8_UINT:
        case RZ_GFX_FORMAT_STENCIL8:
            return 1;

        // 16-bit single component (2 bytes)
        case RZ_GFX_FORMAT_R16_UNORM:
        case RZ_GFX_FORMAT_R16_FLOAT:
        case RZ_GFX_FORMAT_D16_UNORM:
            return 2;

        // 16-bit dual component (4 bytes)
        case RZ_GFX_FORMAT_R16G16_FLOAT:
        case RZ_GFX_FORMAT_R16G16_UNORM:
        case RZ_GFX_FORMAT_R8G8_UNORM:
            return 4;

        // 32-bit single component (4 bytes)
        case RZ_GFX_FORMAT_R32_SINT:
        case RZ_GFX_FORMAT_R32_UINT:
        case RZ_GFX_FORMAT_R32_FLOAT:
        case RZ_GFX_FORMAT_D32_FLOAT:
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT:
        case RZ_GFX_FORMAT_R11G11B10_FLOAT:
        case RZ_GFX_FORMAT_R11G11B10_UINT:
            return 4;

        // 32-bit dual component (8 bytes)
        case RZ_GFX_FORMAT_R32G32_SINT:
        case RZ_GFX_FORMAT_R32G32_UINT:
        case RZ_GFX_FORMAT_R32G32_FLOAT:
        case RZ_GFX_FORMAT_R16G16B16A16_UNORM:
        case RZ_GFX_FORMAT_R16G16B16A16_FLOAT:
            return 8;

        // RGB 8-bit formats (3 bytes)
        case RZ_GFX_FORMAT_R8G8B8_UNORM:
        case RZ_GFX_FORMAT_RGB8_UNORM:
        case RZ_GFX_FORMAT_RGB:
            return 3;

        // RGBA 8-bit formats (4 bytes)
        case RZ_GFX_FORMAT_R8G8B8A8_UNORM:
        case RZ_GFX_FORMAT_R8G8B8A8_SRGB:
        case RZ_GFX_FORMAT_B8G8R8A8_UNORM:
        case RZ_GFX_FORMAT_B8G8R8A8_SRGB:
        case RZ_GFX_FORMAT_RGBA:
        case RZ_GFX_FORMAT_SCREEN:
            return 4;

        // RGB 16-bit formats (6 bytes)
        case RZ_GFX_FORMAT_RGB16_UNORM:
            return 6;

        // 32-bit triple component (12 bytes)
        case RZ_GFX_FORMAT_R32G32B32_SINT:
        case RZ_GFX_FORMAT_R32G32B32_UINT:
        case RZ_GFX_FORMAT_R32G32B32_FLOAT:
        case RZ_GFX_FORMAT_RGB32_UINT:
            return 12;

        // 32-bit quad component (16 bytes)
        case RZ_GFX_FORMAT_R32G32B32A32_SINT:
        case RZ_GFX_FORMAT_R32G32B32A32_UINT:
        case RZ_GFX_FORMAT_R32G32B32A32_FLOAT:
            return 16;

        // Depth-stencil combined format (8 bytes)
        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT:
            return 8;

        // Block compressed formats - these are special cases
        // Block formats compress 4x4 pixel blocks, so bytes per pixel depends on block size
        case RZ_GFX_FORMAT_BC1_RGBA_UNORM:
            return 0;    // BC1: 8 bytes per 4x4 block = 0.5 bytes per pixel (use special handling)

        case RZ_GFX_FORMAT_BC3_RGBA_UNORM:
        case RZ_GFX_FORMAT_BC6_UNORM:
        case RZ_GFX_FORMAT_BC7_UNORM:
        case RZ_GFX_FORMAT_BC7_SRGB:
            return 0;    // BC3/BC6/BC7: 16 bytes per 4x4 block = 1 byte per pixel (use special handling)

        default:
            return 0;    // Unsupported or invalid format
    }
}

uint32_t rzRHI_GetFormatStrideComponentsCount(rz_gfx_format format)
{
    switch (format) {
        // Undefined format
        case RZ_GFX_FORMAT_UNDEFINED:
            return 0;

        // Single component formats
        case RZ_GFX_FORMAT_R8_UNORM:
        case RZ_GFX_FORMAT_R8_UINT:
        case RZ_GFX_FORMAT_R16_UNORM:
        case RZ_GFX_FORMAT_R16_FLOAT:
        case RZ_GFX_FORMAT_R32_SINT:
        case RZ_GFX_FORMAT_R32_UINT:
        case RZ_GFX_FORMAT_R32_FLOAT:
        case RZ_GFX_FORMAT_D16_UNORM:
        case RZ_GFX_FORMAT_D32_FLOAT:
        case RZ_GFX_FORMAT_STENCIL8:
            return 1;

        // Dual component formats
        case RZ_GFX_FORMAT_R8G8_UNORM:
        case RZ_GFX_FORMAT_R16G16_FLOAT:
        case RZ_GFX_FORMAT_R16G16_UNORM:
        case RZ_GFX_FORMAT_R32G32_SINT:
        case RZ_GFX_FORMAT_R32G32_UINT:
        case RZ_GFX_FORMAT_R32G32_FLOAT:
            return 2;

        // Triple component formats
        case RZ_GFX_FORMAT_R8G8B8_UNORM:
        case RZ_GFX_FORMAT_RGB8_UNORM:
        case RZ_GFX_FORMAT_RGB:
        case RZ_GFX_FORMAT_RGB16_UNORM:
        case RZ_GFX_FORMAT_R32G32B32_SINT:
        case RZ_GFX_FORMAT_R32G32B32_UINT:
        case RZ_GFX_FORMAT_R32G32B32_FLOAT:
        case RZ_GFX_FORMAT_RGB32_UINT:
        case RZ_GFX_FORMAT_R11G11B10_FLOAT:
        case RZ_GFX_FORMAT_R11G11B10_UINT:
            return 3;

        // Quad component formats
        case RZ_GFX_FORMAT_R8G8B8A8_UNORM:
        case RZ_GFX_FORMAT_R8G8B8A8_SRGB:
        case RZ_GFX_FORMAT_B8G8R8A8_UNORM:
        case RZ_GFX_FORMAT_B8G8R8A8_SRGB:
        case RZ_GFX_FORMAT_RGBA:
        case RZ_GFX_FORMAT_SCREEN:
        case RZ_GFX_FORMAT_R16G16B16A16_UNORM:
        case RZ_GFX_FORMAT_R16G16B16A16_FLOAT:
        case RZ_GFX_FORMAT_R32G32B32A32_SINT:
        case RZ_GFX_FORMAT_R32G32B32A32_UINT:
        case RZ_GFX_FORMAT_R32G32B32A32_FLOAT:
            return 4;

        // Special depth-stencil formats (2 components: depth + stencil)
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT:
        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT:
            return 2;

        // Block compressed formats (treat as 4 components since they represent RGBA)
        case RZ_GFX_FORMAT_BC1_RGBA_UNORM:
        case RZ_GFX_FORMAT_BC3_RGBA_UNORM:
        case RZ_GFX_FORMAT_BC6_UNORM:
        case RZ_GFX_FORMAT_BC7_UNORM:
        case RZ_GFX_FORMAT_BC7_SRGB:
            return 4;

        default:
            return 0;    // Unsupported or invalid format
    }
}

uint32_t rzRHI_GetFormatComponentSize(rz_gfx_format format)
{
    switch (format) {
        // Undefined format
        case RZ_GFX_FORMAT_UNDEFINED:
            return 0;

        // 8-bit formats (1 byte per component)
        case RZ_GFX_FORMAT_R8_UNORM:
        case RZ_GFX_FORMAT_R8_UINT:
        case RZ_GFX_FORMAT_R8G8_UNORM:
        case RZ_GFX_FORMAT_R8G8B8_UNORM:
        case RZ_GFX_FORMAT_RGB8_UNORM:
        case RZ_GFX_FORMAT_RGB:
        case RZ_GFX_FORMAT_R8G8B8A8_UNORM:
        case RZ_GFX_FORMAT_R8G8B8A8_SRGB:
        case RZ_GFX_FORMAT_B8G8R8A8_UNORM:
        case RZ_GFX_FORMAT_B8G8R8A8_SRGB:
        case RZ_GFX_FORMAT_RGBA:
        case RZ_GFX_FORMAT_SCREEN:
        case RZ_GFX_FORMAT_STENCIL8:
            return sizeof(uint8_t);    // 1 byte

        // 16-bit formats (2 bytes per component)
        case RZ_GFX_FORMAT_R16_UNORM:
        case RZ_GFX_FORMAT_R16_FLOAT:
        case RZ_GFX_FORMAT_R16G16_UNORM:
        case RZ_GFX_FORMAT_R16G16_FLOAT:
        case RZ_GFX_FORMAT_RGB16_UNORM:
        case RZ_GFX_FORMAT_R16G16B16A16_UNORM:
        case RZ_GFX_FORMAT_R16G16B16A16_FLOAT:
        case RZ_GFX_FORMAT_D16_UNORM:
            return sizeof(uint16_t);    // 2 bytes

        // 32-bit integer formats
        case RZ_GFX_FORMAT_R32_SINT:
        case RZ_GFX_FORMAT_R32G32_SINT:
        case RZ_GFX_FORMAT_R32G32B32_SINT:
        case RZ_GFX_FORMAT_R32G32B32A32_SINT:
            return sizeof(int32_t);    // 4 bytes

        // 32-bit unsigned integer formats
        case RZ_GFX_FORMAT_R32_UINT:
        case RZ_GFX_FORMAT_R32G32_UINT:
        case RZ_GFX_FORMAT_R32G32B32_UINT:
        case RZ_GFX_FORMAT_RGB32_UINT:
        case RZ_GFX_FORMAT_R32G32B32A32_UINT:
        case RZ_GFX_FORMAT_R11G11B10_UINT:
            return sizeof(uint32_t);    // 4 bytes

        // 32-bit float formats
        case RZ_GFX_FORMAT_R32_FLOAT:
        case RZ_GFX_FORMAT_R32G32_FLOAT:
        case RZ_GFX_FORMAT_R32G32B32_FLOAT:
        case RZ_GFX_FORMAT_R32G32B32A32_FLOAT:
        case RZ_GFX_FORMAT_D32_FLOAT:
        case RZ_GFX_FORMAT_R11G11B10_FLOAT:
            return sizeof(float);    // 4 bytes

        // Special depth-stencil formats
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT:
            return sizeof(uint32_t);    // 4 bytes (24-bit depth + 8-bit stencil packed)

        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT:
            return sizeof(uint64_t);    // 8 bytes (32-bit float depth + 32-bit stencil/padding)

        // Block compressed formats - return the block size since component size doesn't apply
        case RZ_GFX_FORMAT_BC1_RGBA_UNORM:
            return 8;    // 8 bytes per 4x4 block

        case RZ_GFX_FORMAT_BC3_RGBA_UNORM:
        case RZ_GFX_FORMAT_BC6_UNORM:
        case RZ_GFX_FORMAT_BC7_UNORM:
        case RZ_GFX_FORMAT_BC7_SRGB:
            return 16;    // 16 bytes per 4x4 block

        default:
            return 0;    // Unsupported or invalid format
    }
}

uint32_t rzRHI_GetMipLevelCount(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0)
        return 0;
    return (uint32_t) (floor(log2((double) ((width > height) ? width : height)))) + 1;
}

rz_gfx_resource_state rzRHI_DeduceResourceState(rz_gfx_descriptor_type type, rz_gfx_res_view_op_flags resViewOpFlags, bool isWrite)
{
    rz_gfx_resource_state deducedState = RZ_GFX_RESOURCE_STATE_UNDEFINED;

    // these flags are enough to determine the state directly
    if (resViewOpFlags & RZ_GFX_RES_VIEW_OP_FLAG_COPY_DST) {
        deducedState = RZ_GFX_RESOURCE_STATE_COPY_DST;
    } else if (resViewOpFlags & RZ_GFX_RES_VIEW_OP_FLAG_COPY_SRC) {
        deducedState = RZ_GFX_RESOURCE_STATE_COPY_SRC;
    } else if (resViewOpFlags & RZ_GFX_RES_VIEW_OP_FLAG_PRESENT) {
        deducedState = RZ_GFX_RESOURCE_STATE_PRESENT;
    } else if (resViewOpFlags & RZ_GFX_RES_VIEW_OP_FLAG_RESOLVE_SRC) {
        deducedState = RZ_GFX_RESOURCE_STATE_RESOLVE_SRC;
    } else if (resViewOpFlags & RZ_GFX_RES_VIEW_OP_FLAG_RESOLVE_DST) {
        deducedState = RZ_GFX_RESOURCE_STATE_RESOLVE_DST;
    } else if (resViewOpFlags & RZ_GFX_RES_VIEW_OP_FLAG_INDIRECT) {
        deducedState = RZ_GFX_RESOURCE_STATE_INDIRECT_ARGUMENT;
    } else {
        switch (type) {
            case RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER:
                deducedState = RZ_GFX_RESOURCE_STATE_CONSTANT_BUFFER;
                break;

            case RZ_GFX_DESCRIPTOR_TYPE_PUSH_CONSTANT:
                deducedState = RZ_GFX_RESOURCE_STATE_UNDEFINED;
                break;

            case RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED:
            case RZ_GFX_DESCRIPTOR_TYPE_TEXTURE:
            case RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED:
            case RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS:
                deducedState = RZ_GFX_RESOURCE_STATE_SHADER_READ;
                break;

            case RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE:
            case RZ_GFX_DESCRIPTOR_TYPE_RW_TYPED:
            case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED:
            case RZ_GFX_DESCRIPTOR_TYPE_RW_BYTE_ADDRESS:
            case RZ_GFX_DESCRIPTOR_TYPE_APPEND_STRUCTURED:
            case RZ_GFX_DESCRIPTOR_TYPE_CONSUME_STRUCTURED:
            case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED_COUNTER:
                deducedState = isWrite ? RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS
                                       : RZ_GFX_RESOURCE_STATE_SHADER_READ;
                break;

            case RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE:
                deducedState = isWrite ? RZ_GFX_RESOURCE_STATE_RENDER_TARGET
                                       : RZ_GFX_RESOURCE_STATE_SHADER_READ;
                break;

            case RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE:
                deducedState = isWrite ? RZ_GFX_RESOURCE_STATE_DEPTH_WRITE
                                       : RZ_GFX_RESOURCE_STATE_DEPTH_READ;
                break;

            case RZ_GFX_DESCRIPTOR_TYPE_SAMPLER:
                deducedState = RZ_GFX_RESOURCE_STATE_UNDEFINED;
                break;

            case RZ_GFX_DESCRIPTOR_TYPE_RT_ACCELERATION_STRUCTURE:
                deducedState = RZ_GFX_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
                break;

            case RZ_GFX_DESCRIPTOR_TYPE_NONE:
            case RZ_GFX_DESCRIPTOR_TYPE_COUNT:
            default:
                deducedState = RZ_GFX_RESOURCE_STATE_UNDEFINED;
                break;
        }
    }

    return deducedState;
}

const char* rzRHI_GetResourceTypeString(rz_gfx_resource_type type)
{
    switch (type) {
        case RZ_GFX_RESOURCE_TYPE_INVALID: return "Invalid"; break;
        case RZ_GFX_RESOURCE_TYPE_RESOURCE_VIEW: return "Resource View"; break;
        case RZ_GFX_RESOURCE_TYPE_TEXTURE: return "Texture"; break;
        case RZ_GFX_RESOURCE_TYPE_BUFFER: return "Buffer"; break;
        case RZ_GFX_RESOURCE_TYPE_SAMPLER: return "Sampler"; break;
        case RZ_GFX_RESOURCE_TYPE_SHADER: return "Shader"; break;
        case RZ_GFX_RESOURCE_TYPE_DESCRIPTOR_HEAP: return "Descriptor Heap"; break;
        case RZ_GFX_RESOURCE_TYPE_DESCRIPTOR_TABLE: return "Descriptor Table"; break;
        case RZ_GFX_RESOURCE_TYPE_ROOT_SIGNATURE: return "Root Signature"; break;
        case RZ_GFX_RESOURCE_TYPE_CMD_POOL: return "Command Pool"; break;
        case RZ_GFX_RESOURCE_TYPE_CMD_BUFFER: return "Command Buffer"; break;
        case RZ_GFX_RESOURCE_TYPE_PIPELINE: return "Pipeline"; break;
        case RZ_GFX_RESOURCE_TYPE_COUNT: return "Count"; break;
        default: return "Unknown"; break;
    }
}

const char* rzRHI_GetDescriptorTypeString(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_NONE: return "NONE";
        case RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER: return "CONSTANT_BUFFER";
        case RZ_GFX_DESCRIPTOR_TYPE_PUSH_CONSTANT: return "PUSH_CONSTANT";
        case RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED: return "IMAGE_SAMPLER_COMBINED";
        case RZ_GFX_DESCRIPTOR_TYPE_TEXTURE: return "TEXTURE";
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE: return "RW_TEXTURE";
        case RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE: return "RENDER_TEXTURE";
        case RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE: return "DEPTH_STENCIL_TEXTURE";
        case RZ_GFX_DESCRIPTOR_TYPE_SAMPLER: return "SAMPLER";
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TYPED: return "RW_TYPED";
        case RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED: return "STRUCTURED";
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED: return "RW_STRUCTURED";
        case RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS: return "BYTE_ADDRESS";
        case RZ_GFX_DESCRIPTOR_TYPE_RW_BYTE_ADDRESS: return "RW_BYTE_ADDRESS";
        case RZ_GFX_DESCRIPTOR_TYPE_APPEND_STRUCTURED: return "APPEND_STRUCTURED";
        case RZ_GFX_DESCRIPTOR_TYPE_CONSUME_STRUCTURED: return "CONSUME_STRUCTURED";
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED_COUNTER: return "RW_STRUCTURED_COUNTER";
        case RZ_GFX_DESCRIPTOR_TYPE_RT_ACCELERATION_STRUCTURE: return "RT_ACCELERATION_STRUCTURE";
        case RZ_GFX_DESCRIPTOR_TYPE_COUNT: return "COUNT";
        default: return "UNKNOWN_DESCRIPTOR_TYPE";
    }
}

char* rzRHI_ResourceOpFlagsString(rz_gfx_res_view_op_flags flags, char* buffer, size_t bufferLen)
{
    buffer[0] = '\0';
    int first = 1;
    if (flags == RZ_GFX_RES_VIEW_OP_FLAG_NONE) {
        strncpy(buffer, "NONE", bufferLen - 1);
        buffer[bufferLen - 1] = '\0';
        return buffer;
    }
    if (flags & RZ_GFX_RES_VIEW_OP_FLAG_COPY_SRC) {
        if (!first) strncat(buffer, " | ", bufferLen - strlen(buffer) - 1);
        strncat(buffer, "COPY_SRC", bufferLen - strlen(buffer) - 1);
        first = 0;
    }
    if (flags & RZ_GFX_RES_VIEW_OP_FLAG_COPY_DST) {
        if (!first) strncat(buffer, " | ", bufferLen - strlen(buffer) - 1);
        strncat(buffer, "COPY_DST", bufferLen - strlen(buffer) - 1);
        first = 0;
    }
    if (flags & RZ_GFX_RES_VIEW_OP_FLAG_PRESENT) {
        if (!first) strncat(buffer, " | ", bufferLen - strlen(buffer) - 1);
        strncat(buffer, "PRESENT", bufferLen - strlen(buffer) - 1);
        first = 0;
    }
    if (flags & RZ_GFX_RES_VIEW_OP_FLAG_RESOLVE_SRC) {
        if (!first) strncat(buffer, " | ", bufferLen - strlen(buffer) - 1);
        strncat(buffer, "RESOLVE_SRC", bufferLen - strlen(buffer) - 1);
        first = 0;
    }
    if (flags & RZ_GFX_RES_VIEW_OP_FLAG_RESOLVE_DST) {
        if (!first) strncat(buffer, " | ", bufferLen - strlen(buffer) - 1);
        strncat(buffer, "RESOLVE_DST", bufferLen - strlen(buffer) - 1);
        first = 0;
    }
    if (flags & RZ_GFX_RES_VIEW_OP_FLAG_INDIRECT) {
        if (!first) strncat(buffer, " | ", bufferLen - strlen(buffer) - 1);
        strncat(buffer, "INDIRECT", bufferLen - strlen(buffer) - 1);
        first = 0;
    }
    if (flags & RZ_GFX_RES_VIEW_OP_FLAG_SKIP_BARRIER) {
        if (!first) strncat(buffer, " | ", bufferLen - strlen(buffer) - 1);
        strncat(buffer, "SKIP_BARRIER", bufferLen - strlen(buffer) - 1);
        first = 0;
    }
    return buffer;
}

const char* rzRHI_GetResourceStateString(rz_gfx_resource_state state)
{
    switch (state) {
        case RZ_GFX_RESOURCE_STATE_UNDEFINED: return "UNDEFINED";
        case RZ_GFX_RESOURCE_STATE_COMMON: return "COMMON";
        case RZ_GFX_RESOURCE_STATE_GENERIC_READ: return "GENERIC_READ";
        case RZ_GFX_RESOURCE_STATE_RENDER_TARGET: return "RENDER_TARGET";
        case RZ_GFX_RESOURCE_STATE_DEPTH_WRITE: return "DEPTH_WRITE";
        case RZ_GFX_RESOURCE_STATE_DEPTH_READ: return "DEPTH_READ";
        case RZ_GFX_RESOURCE_STATE_SHADER_READ: return "SHADER_READ";
        case RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS: return "UNORDERED_ACCESS";
        case RZ_GFX_RESOURCE_STATE_COPY_SRC: return "COPY_SRC";
        case RZ_GFX_RESOURCE_STATE_COPY_DST: return "COPY_DST";
        case RZ_GFX_RESOURCE_STATE_PRESENT: return "PRESENT";
        case RZ_GFX_RESOURCE_STATE_VERTEX_BUFFER: return "VERTEX_BUFFER";
        case RZ_GFX_RESOURCE_STATE_INDEX_BUFFER: return "INDEX_BUFFER";
        case RZ_GFX_RESOURCE_STATE_CONSTANT_BUFFER: return "CONSTANT_BUFFER";
        case RZ_GFX_RESOURCE_STATE_INDIRECT_ARGUMENT: return "INDIRECT_ARGUMENT";
        case RZ_GFX_RESOURCE_STATE_RESOLVE_SRC: return "RESOLVE_SRC";
        case RZ_GFX_RESOURCE_STATE_RESOLVE_DST: return "RESOLVE_DST";
        case RZ_GFX_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE: return "RAYTRACING_ACCELERATION_STRUCTURE";
        case RZ_GFX_RESOURCE_STATE_SHADING_RATE_SOURCE: return "SHADING_RATE_SOURCE";
        case RZ_GFX_RESOURCE_STATE_VIDEO_DECODE_READ: return "VIDEO_DECODE_READ";
        case RZ_GFX_RESOURCE_STATE_VIDEO_DECODE_WRITE: return "VIDEO_DECODE_WRITE";
        case RZ_GFX_RESOURCE_STATE_COUNT: return "COUNT";
        default: return "UNKNOWN_RESOURCE_STATE";
    }
}