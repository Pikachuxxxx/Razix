#ifndef VK_RHI_H
#define VK_RHI_H

// TODO: this header will be moved to RHI.h before merging the PR #422 and #420

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

typedef struct vk_ctx
{
    VkInstance       instance;
    VkPhysicalDevice gpu;
    VkDevice         device;
    VkQueue          graphicsQueue;
    VkQueue          asyncComputeQueue;
    VkQueue          transferQueue;
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

typedef struct vk_texture
{
    VkImage        image;
    VkDeviceMemory memory;
} vk_texture;

typedef struct vk_sampler
{
    VkSampler sampler;
} vk_sampler;

typedef struct vk_resview
{
    union
    {
        VkBufferView               bufferView;
        VkImageView                imageView;
        VkSampler                  sampler;
        VkAccelerationStructureKHR accelerationStructure;
    };
} vk_resview;

typedef struct vk_buffer
{
    VkBuffer       buffer;
    VkDeviceMemory memory;
} vk_buffer;

#endif    // RAZIX_RENDER_API_VULKAN
#endif    // vk_RHI_H
