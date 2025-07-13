#include "RHI.h"

rz_gfx_context g_GfxCtx = {0};
// Default
rz_render_api g_RenderAPI = RZ_RENDER_API_VULKAN;
rz_rhi_api    g_RHI       = {0};

// Defined in vk/dx12_rhi.c
extern const rz_rhi_api vk_rhi;
extern const rz_rhi_api dx12_rhi;

void rzGfxCtx_StartUp()
{
    switch (g_RenderAPI) {
        case RZ_RENDER_API_VULKAN:
            g_RHI = vk_rhi;
            break;
        case RZ_RENDER_API_D3D12:
            g_RHI = dx12_rhi;
            break;
        default:
            RAZIX_RHI_LOG_ERROR("Unsupported Render API: %d", (uint32_t) g_RenderAPI);
            break;
    }
}

void rzGfxCtx_ShutDown()
{
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

void rzGfxCtx_GlobalCtxInit()
{
    g_RHI.GlobalCtxInit();
}
