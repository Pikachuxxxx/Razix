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
        #define RAZIX_MACOS_MOLTENVK
        // Experimental driver backend, build your own mesa 26.0.0 for the latest kosmickrisp drivers! until then it's disabled
        //#define RAZIX_MACOS_KOSMICKRISP_DRIVER
        // https://www.lunarg.com/a-vulkan-on-metal-mesa-3d-graphics-driver/
        // https://gitlab.freedesktop.org/mesa/mesa/-/issues/11990
        // https://www.lunarg.com/wp-content/uploads/2025/10/XDC-2025-KosmicKrisp-Overview.pdf
        // mesa 26.0.0-devel has vulkan 1.3 conformant kosmickrisp driver and Raizx Gfx tests all pass
        // build using meson setup build \
        // -Dvulkan-drivers=kosmickrisp \
        // -Dgallium-drivers= \
        // -Dplatforms=macos \
        // -Dglx=disabled \
        // -Degl=disabled \
        // -Dllvm=enabled \
        // --strip \
        // --buildtype=release
        // use export VK_ICD_FILENAMES to load the kosmickrisp_mesa_devenv_icd.aarch64.json and load the libvulkan_kosmickrisp.dylib
        // instead of libvulkan_moltenvk.dylib, this is still experimental but works fine, enable the RAZIX_MACOS_KOSMICKRISP_DRIVER
        // when you have a working setup of kosmickrisp build, until it's released in mesa/lunarg upcoming vulkan SDK we will stick to MVK for stability
        // As for installing the kosmickrisp driver I will provide instructions in readme and we will use it as experimental, however I will keep testing
        // and developing it locally and try to fix the driver in mesa if any issues arise.
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
    VkImage                   images[4];
    VkImageView               imageViews[4];
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
    uint32_t         allocatedSets;
} vk_descriptor_heap;

typedef struct vk_descriptor_table
{
    VkDescriptorSetLayout setLayout;
    VkDescriptorSet       set;
} vk_descriptor_table;

typedef struct vk_root_signature
{
    VkPipelineLayout pipelineLayout;
} vk_root_signature;

typedef struct vk_shader
{
    VkShaderModule modules[13];
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
