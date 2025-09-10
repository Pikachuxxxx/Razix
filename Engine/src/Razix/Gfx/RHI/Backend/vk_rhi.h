#ifndef VK_RHI_H
#define VK_RHI_H

#include <stdbool.h>
#include <stdint.h>

// TODO: this header will be moved to RHI.h before merging the PR #422 and #420

#ifdef RAZIX_RENDER_API_VULKAN
    #ifdef RAZIX_PLATFORM_WINDOWS
        #define VK_USE_PLATFORM_WIN32_KHR
    #endif

    #if defined(RAZIX_PLATFORM_MACOS)
        #define VK_USE_PLATFORM_MACOS_MVK
        #define RAZIX_PLATFORM_MOLTENVK
        // TODO: Add support for KosmicKrisp
        // https://www.lunarg.com/a-vulkan-on-metal-mesa-3d-graphics-driver/
        // https://gitlab.freedesktop.org/mesa/mesa/-/issues/11990
    #endif

    #if defined(RAZIX_PLATFORM_LINUX)
        #define VK_USE_PLATFORM_WAYLAND_KHR
        #define VK_USE_PLATFORM_XCB_KHR
        #define VK_USE_PLATFORM_XLIB_KHR
        #define RAZIX_PLATFORM_WAYLAND
        #define RAZIX_PLATFORM_XCB
        #define RAZIX_PLATFORM_XLIB
    #endif

    #include <volk.h>

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
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR*      formats;
    uint32_t                 formatCount;
    VkPresentModeKHR*        presentModes;
    uint32_t                 presentModeCount;
} VkSwapchainSupportDetails;

typedef struct vk_ctx
{
    VkInstance                       instance;
    VkPhysicalDevice                 gpu;
    VkDevice                         device;
    VkSurfaceKHR                     surface;
    VkQueueFamilyIndices             queueFamilyIndices;
    VkQueue                          graphicsQueue;
    VkQueue                          presentQueue;
    VkPhysicalDeviceProperties       deviceProperties;
    VkPhysicalDeviceFeatures         deviceFeatures;
    VkPhysicalDeviceMemoryProperties memoryProperties;

        // Debug and validation
    #ifdef RAZIX_DEBUG
    VkDebugUtilsMessengerEXT debugMessenger;
    #endif
} vk_ctx;

typedef struct vk_syncobj
{
    union
    {
        VkSemaphore semaphore;
        VkFence     fence;
    };
} vk_syncobj;

typedef struct vk_swapchain
{
    VkSwapchainKHR            swapchain;
    VkFormat                  imageFormat;
    uint32_t                  imageCount;
    VkImage*                  images;
    VkImageView*              imageViews;
    VkSwapchainSupportDetails supportDetails;
} vk_swapchain;

typedef struct vk_cmdpool
{
    VkCommandPool cmdPool;
} vk_cmdpool;

typedef struct vk_cmdbuf
{
    VkCommandBuffer cmdBuf;
    VkCommandPool   cmdPool;
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
