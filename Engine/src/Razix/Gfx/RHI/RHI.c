#include "RHI.h"

#include <math.h    >

RAZIX_RHI_API rz_gfx_context g_GfxCtx = {0};
// Default
RAZIX_RHI_API rz_render_api   g_RenderAPI        = RZ_RENDER_API_VULKAN;
RAZIX_RHI_API rz_rhi_api      g_RHI              = {0};
RAZIX_RHI_API rz_gfx_features g_GraphicsFeatures = {0};

// Defined in vk/dx12_rhi.c
RAZIX_RHI_API extern const rz_rhi_api vk_rhi;
RAZIX_RHI_API extern const rz_rhi_api dx12_rhi;

void rzGfxCtx_StartUp()
{
    switch (g_RenderAPI) {
            // Load the DLL and then initialize RHI table
        case RZ_RENDER_API_VULKAN:
            RAZIX_RHI_LOG_INFO("Initializing Vulkan RHI");
            g_RHI = vk_rhi;
            break;
        case RZ_RENDER_API_D3D12:
            RAZIX_RHI_LOG_INFO("Initializing DirectX 12 RHI");
            g_RHI = dx12_rhi;
            break;
        default:
            RAZIX_RHI_LOG_ERROR("Unsupported Render API: %d", (uint32_t) g_RenderAPI);
            break;
    }

    rzGfxCtx_GlobalCtxInit();
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

uint32_t rzRHI_GetMipLevelCount(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0)
        return 0;
    return (uint32_t) (floor(log2((double) ((width > height) ? width : height)))) + 1;
}
