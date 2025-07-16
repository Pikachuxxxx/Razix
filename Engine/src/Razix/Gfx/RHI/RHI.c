#include "RHI.h"

rz_gfx_context g_GfxCtx = {0};
// Default
rz_render_api   g_RenderAPI        = RZ_RENDER_API_VULKAN;
rz_rhi_api      g_RHI              = {0};
rz_gfx_features g_GraphicsFeatures = {0};

// Defined in vk/dx12_rhi.c
RAZIX_API extern const rz_rhi_api vk_rhi;
RAZIX_API extern const rz_rhi_api dx12_rhi;

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

rz_gfx_shader_stage rzGfx_StringToShaderStage(const char* stage)
{
    if (strstr(stage, "vertex")) return RZ_GFX_SHADER_STAGE_VERTEX;
    if (strstr(stage, "geometry")) return RZ_GFX_SHADER_STAGE_GEOMETRY;
    if (strstr(stage, "fragment")) return RZ_GFX_SHADER_STAGE_PIXEL;
    if (strstr(stage, "compute")) return RZ_GFX_SHADER_STAGE_COMPUTE;
    return RZ_GFX_SHADER_STAGE_NONE;
}