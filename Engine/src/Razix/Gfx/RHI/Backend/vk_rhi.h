#ifndef VK_RHI_H
#define VK_RHI_H
#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

typedef struct vk_gfx_ctx
{
    VkInstance instance;
} vk_gfx_ctx;

#endif    // RAZIX_RENDER_API_VULKAN
#endif    // DX12_RHI_H
