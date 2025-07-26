#ifndef VK_RHI_H
#define VK_RHI_H
#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

typedef struct vk_ctx
{
    VkInstance       instance;
    VkPhysicalDevice gpu;
    VkDevice         device;
} vk_ctx;

typedef struct vk_cmdpool
{
    VkCommandPool cmdPool;
} vk_cmdpool;

typedef struct vk_cmdbuf
{
    VkCommandBuffer cmdBuf;
} vk_cmdbuf;

typedef struct vk_descriptor_heap
{
    VkDescriptorPool pool;
} vk_descriptor_heap;

typedef struct vk_descriptor_table
{
    VkDescriptorSetLayout setLayout;
    VkDescriptorSet       descriptorSet;
} vk_descriptor_table;

typedef struct vk_root_signature
{
    VkPipelineLayout pipelineLayout;
} vk_root_signature;

typedef struct vk_shader
{
    VkShaderModule module;    // one per stage if needed
} vk_shader;

typedef struct vk_pipeline
{
    VkPipeline       pipeline;
    VkPipelineLayout pipelineLayout;
} vk_pipeline;

#endif    // RAZIX_RENDER_API_VULKAN
#endif    // vk_RHI_H
