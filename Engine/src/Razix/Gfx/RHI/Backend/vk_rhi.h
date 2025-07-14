#ifndef VK_RHI_H
#define VK_RHI_H
#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

typedef struct vk_ctx
{
    VkInstance instance;
} vk_ctx;

typedef struct vk_cmdpool
{
    VkCommandPool cmdPool;
} vk_cmdpool;

typedef struct vk_cmdbuf
{
    VkCommandBuffer cmdBuf;
} vk_cmdbuf;

#endif    // RAZIX_RENDER_API_VULKAN
#endif    // vk_RHI_H
