#include "vk_rhi.h"

#include "Razix/Gfx/RHI/rhi.h"

static void vk_GlobalCtxInit(void)
{
    // Create the instance
}

//---------------------------------------------------------------------------------------------

rz_rhi_api vk_rhi = {
    .GlobalCtxInit = vk_GlobalCtxInit,    // GlobalCtxInit
    NULL,                                 // AcquireImage
};