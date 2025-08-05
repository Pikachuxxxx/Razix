#ifndef VK_RHI_H
#define VK_RHI_H
#ifdef RAZIX_RENDER_API_VULKAN

    #include <stdbool.h>
    #include <stdint.h>

    #define VK_NO_PROTOTYPES
    #include <vulkan/vulkan.h>

typedef struct VkQueueFamilyIndices
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    uint32_t computeFamily;
    uint32_t transferFamily;
    bool     hasGraphics;
    bool     hasPresent;
    bool     hasCompute;
    bool     hasTransfer;
} VkQueueFamilyIndices;

typedef struct VkSwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    VkSurfaceFormatKHR*             formats;
    uint32_t                        formatCount;
    VkPresentModeKHR*               presentModes;
    uint32_t                        presentModeCount;
} VkSwapchainSupportDetails;

typedef struct vk_ctx
{
    VkInstance               instance;
    VkPhysicalDevice         gpu;
    VkDevice                 device;
    VkSurfaceKHR             surface;
    VkQueueFamilyIndices     queueFamilyIndices;
    VkQueue                  graphicsQueue;
    VkQueue                  presentQueue;
    VkQueue                  computeQueue;
    VkQueue                  transferQueue;
    VkCommandPool            transientCmdPool;  // For one-time command submissions
    
    // Device features and properties
    VkPhysicalDeviceProperties       deviceProperties;
    VkPhysicalDeviceFeatures         deviceFeatures;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    
    // Debug and validation
    #ifdef RAZIX_DEBUG
    VkDebugUtilsMessengerEXT debugMessenger;
    #endif
    
    // Available extensions and layers
    char** enabledInstanceExtensions;
    uint32_t enabledInstanceExtensionCount;
    char** enabledDeviceExtensions;
    uint32_t enabledDeviceExtensionCount;
    char** enabledLayers;
    uint32_t enabledLayerCount;
} vk_ctx;

typedef struct vk_syncobj
{
    VkSemaphore semaphore;
    VkFence     fence;
} vk_syncobj;

typedef struct vk_swapchain
{
    VkSwapchainKHR               swapchain;
    VkFormat                     imageFormat;
    VkExtent2D                   extent;
    uint32_t                     imageCount;
    VkImage*                     images;
    VkImageView*                 imageViews;
    VkFramebuffer*               framebuffers;
    VkRenderPass                 renderPass;
    uint32_t                     currentImageIndex;
    VkSwapchainSupportDetails    supportDetails;
} vk_swapchain;

typedef struct vk_cmdpool
{
    VkCommandPool cmdPool;
    VkQueueFlags  queueFlags;
} vk_cmdpool;

typedef struct vk_cmdbuf
{
    VkCommandBuffer cmdBuf;
    VkCommandPool   parentPool;
    bool            isRecording;
} vk_cmdbuf;

typedef struct vk_descriptor_heap
{
    VkDescriptorPool pool;
    VkDescriptorType poolType;
} vk_descriptor_heap;

typedef struct vk_descriptor_table
{
    VkDescriptorSetLayout setLayout;
    VkDescriptorSet       descriptorSet;
    uint32_t              descriptorCount;
} vk_descriptor_table;

typedef struct vk_root_signature
{
    VkPipelineLayout      pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayouts[8]; // Max descriptor tables
    uint32_t              layoutCount;
    VkPushConstantRange   pushConstantRanges[8];
    uint32_t              pushConstantRangeCount;
} vk_root_signature;

typedef struct vk_shader_stage
{
    VkShaderModule module;
    VkShaderStageFlags stage;
} vk_shader_stage;

typedef struct vk_shader
{
    vk_shader_stage stages[13]; // Max shader stages
    uint32_t        stageCount;
} vk_shader;

typedef struct vk_pipeline
{
    VkPipeline       pipeline;
    VkPipelineLayout pipelineLayout;
    VkPipelineBindPoint bindPoint;
} vk_pipeline;

typedef struct vk_texture
{
    VkImage        image;
    VkImageView    imageView;
    VkDeviceMemory memory;
    VkFormat       format;
    VkExtent3D     extent;
    uint32_t       mipLevels;
    uint32_t       arrayLayers;
    VkImageLayout  currentLayout;
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
    } view;
} vk_resview;

#endif    // RAZIX_RENDER_API_VULKAN
#endif    // VK_RHI_H
