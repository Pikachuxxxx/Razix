#include "RHI.h"

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
            return true;
        default:
            return false;
    }
}

bool rzRHI_IsDescriptorTypeBuffer(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
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
            return false; // These are read-only texture types
        default:
            return false; // Not a texture type
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
        case RZ_GFX_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS:
            return false; // These are read-only buffer types
        default:
            return false; // Not a buffer type
    }
}