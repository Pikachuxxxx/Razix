#include "vk_rhi.h"

#include "Razix/Gfx/RHI/RHI.h"

#include <string.h>
#include <stdlib.h>

// Type friendly defines
#define VkContext g_GfxCtx.vk
#define VkDevice  g_GfxCtx.vk.device
#define VkGPU     g_GfxCtx.vk.gpu

#if defined RAZIX_DEBUG
    #define CHECK_VK(x) vk_util_check_result((x), __func__, __FILE__, __LINE__)
#else
    #define CHECK_VK(x) (x)
#endif

//---------------------------------------------------------------------------------------------
// Constants and configuration
//---------------------------------------------------------------------------------------------

static const char* requiredInstanceExtensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#endif
#ifdef RAZIX_DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
};

static const char* requiredDeviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

#ifdef RAZIX_DEBUG
static const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

//---------------------------------------------------------------------------------------------
// Utility functions
//---------------------------------------------------------------------------------------------

#ifdef RAZIX_DEBUG
static bool vk_util_check_result(VkResult result, const char* func, const char* file, int line)
{
    if (result != VK_SUCCESS) {
        const char* errorStr = "VK_UNKNOWN_ERROR";
        
        switch (result) {
            case VK_NOT_READY: errorStr = "VK_NOT_READY"; break;
            case VK_TIMEOUT: errorStr = "VK_TIMEOUT"; break;
            case VK_ERROR_OUT_OF_HOST_MEMORY: errorStr = "VK_ERROR_OUT_OF_HOST_MEMORY"; break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY: errorStr = "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;
            case VK_ERROR_INITIALIZATION_FAILED: errorStr = "VK_ERROR_INITIALIZATION_FAILED"; break;
            case VK_ERROR_DEVICE_LOST: errorStr = "VK_ERROR_DEVICE_LOST"; break;
            case VK_ERROR_MEMORY_MAP_FAILED: errorStr = "VK_ERROR_MEMORY_MAP_FAILED"; break;
            case VK_ERROR_LAYER_NOT_PRESENT: errorStr = "VK_ERROR_LAYER_NOT_PRESENT"; break;
            case VK_ERROR_EXTENSION_NOT_PRESENT: errorStr = "VK_ERROR_EXTENSION_NOT_PRESENT"; break;
            case VK_ERROR_FEATURE_NOT_PRESENT: errorStr = "VK_ERROR_FEATURE_NOT_PRESENT"; break;
            case VK_ERROR_INCOMPATIBLE_DRIVER: errorStr = "VK_ERROR_INCOMPATIBLE_DRIVER"; break;
            case VK_ERROR_TOO_MANY_OBJECTS: errorStr = "VK_ERROR_TOO_MANY_OBJECTS"; break;
            case VK_ERROR_FORMAT_NOT_SUPPORTED: errorStr = "VK_ERROR_FORMAT_NOT_SUPPORTED"; break;
            case VK_ERROR_FRAGMENTED_POOL: errorStr = "VK_ERROR_FRAGMENTED_POOL"; break;
            case VK_ERROR_SURFACE_LOST_KHR: errorStr = "VK_ERROR_SURFACE_LOST_KHR"; break;
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: errorStr = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; break;
            case VK_SUBOPTIMAL_KHR: errorStr = "VK_SUBOPTIMAL_KHR"; break;
            case VK_ERROR_OUT_OF_DATE_KHR: errorStr = "VK_ERROR_OUT_OF_DATE_KHR"; break;
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: errorStr = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; break;
            case VK_ERROR_VALIDATION_FAILED_EXT: errorStr = "VK_ERROR_VALIDATION_FAILED_EXT"; break;
            default: break;
        }
        
        RAZIX_RHI_LOG_ERROR("[Vulkan] Error :: %s\n -> In function %s (%s:%d)\n", errorStr, func, file, line);
        return false;
    }
    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    (void)messageType;
    (void)pUserData;
    
    const char* severityStr = "UNKNOWN";
    const char* color = ANSI_COLOR_RESET;
    
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            severityStr = "VERBOSE";
            color = ANSI_COLOR_GRAY;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severityStr = "INFO";
            color = ANSI_COLOR_CYAN;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severityStr = "WARNING";
            color = ANSI_COLOR_YELLOW;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severityStr = "ERROR";
            color = ANSI_COLOR_RED;
            break;
    }
    
    printf("%s[%s] [VK/DEBUG/%s] %s%s\n", color, _rhi_log_timestamp(), severityStr, pCallbackData->pMessage, ANSI_COLOR_RESET);
    
    return VK_FALSE;
}
#endif

static VkFormat vk_util_rz_gfx_format_to_vk_format(rz_gfx_format format)
{
    switch (format) {
        // 8-bit
        case RZ_GFX_FORMAT_R8_UNORM: return VK_FORMAT_R8_UNORM;
        case RZ_GFX_FORMAT_R8_UINT: return VK_FORMAT_R8_UINT;
        
        // 16-bit
        case RZ_GFX_FORMAT_R16_UNORM: return VK_FORMAT_R16_UNORM;
        case RZ_GFX_FORMAT_R16_FLOAT: return VK_FORMAT_R16_SFLOAT;
        case RZ_GFX_FORMAT_R16G16_FLOAT: return VK_FORMAT_R16G16_SFLOAT;
        case RZ_GFX_FORMAT_R16G16_UNORM: return VK_FORMAT_R16G16_UNORM;
        case RZ_GFX_FORMAT_R16G16B16A16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
        case RZ_GFX_FORMAT_R16G16B16A16_FLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
        
        // 32-bit
        case RZ_GFX_FORMAT_R32_SINT: return VK_FORMAT_R32_SINT;
        case RZ_GFX_FORMAT_R32_UINT: return VK_FORMAT_R32_UINT;
        case RZ_GFX_FORMAT_R32_FLOAT: return VK_FORMAT_R32_SFLOAT;
        case RZ_GFX_FORMAT_R32G32_SINT: return VK_FORMAT_R32G32_SINT;
        case RZ_GFX_FORMAT_R32G32_UINT: return VK_FORMAT_R32G32_UINT;
        case RZ_GFX_FORMAT_R32G32_FLOAT: return VK_FORMAT_R32G32_SFLOAT;
        case RZ_GFX_FORMAT_R32G32B32_SINT: return VK_FORMAT_R32G32B32_SINT;
        case RZ_GFX_FORMAT_R32G32B32_UINT: return VK_FORMAT_R32G32B32_UINT;
        case RZ_GFX_FORMAT_R32G32B32_FLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
        case RZ_GFX_FORMAT_R32G32B32A32_SINT: return VK_FORMAT_R32G32B32A32_SINT;
        case RZ_GFX_FORMAT_R32G32B32A32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
        case RZ_GFX_FORMAT_R32G32B32A32_FLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
        
        // Packed
        case RZ_GFX_FORMAT_R11G11B10_FLOAT: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        
        // Color formats
        case RZ_GFX_FORMAT_R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
        case RZ_GFX_FORMAT_R8G8B8_UNORM: return VK_FORMAT_R8G8B8_UNORM;
        case RZ_GFX_FORMAT_R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
        case RZ_GFX_FORMAT_R8G8B8A8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
        case RZ_GFX_FORMAT_B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
        case RZ_GFX_FORMAT_B8G8R8A8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
        
        // Depth-stencil
        case RZ_GFX_FORMAT_D16_UNORM: return VK_FORMAT_D16_UNORM;
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
        case RZ_GFX_FORMAT_D32_FLOAT: return VK_FORMAT_D32_SFLOAT;
        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        
        // Swapchain pseudo format
        case RZ_GFX_FORMAT_SCREEN: return RAZIX_SWAPCHAIN_FORMAT_VK;
        
        // Block compression
        case RZ_GFX_FORMAT_BC1_RGBA_UNORM: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case RZ_GFX_FORMAT_BC3_RGBA_UNORM: return VK_FORMAT_BC3_UNORM_BLOCK;
        case RZ_GFX_FORMAT_BC6_UNORM: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case RZ_GFX_FORMAT_BC7_UNORM: return VK_FORMAT_BC7_UNORM_BLOCK;
        case RZ_GFX_FORMAT_BC7_SRGB: return VK_FORMAT_BC7_SRGB_BLOCK;
        
        case RZ_GFX_FORMAT_UNDEFINED:
        default: return VK_FORMAT_UNDEFINED;
    }
}

//---------------------------------------------------------------------------------------------
// Core implementation functions
//---------------------------------------------------------------------------------------------

static void vk_GlobalCtxInit(void)
{
    RAZIX_RHI_LOG_INFO("[Vulkan] Initializing Vulkan context");
    
    // Create instance
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Razix Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Razix";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;
    
    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    const uint32_t extensionCount = sizeof(requiredInstanceExtensions) / sizeof(requiredInstanceExtensions[0]);
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = requiredInstanceExtensions;
    
#ifdef RAZIX_DEBUG
    const uint32_t layerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
    createInfo.enabledLayerCount = layerCount;
    createInfo.ppEnabledLayerNames = validationLayers;
    
    // Debug messenger create info for instance creation/destruction
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = vk_debug_callback;
    
    createInfo.pNext = &debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = NULL;
#endif
    
    if (CHECK_VK(vkCreateInstance(&createInfo, NULL, &VkContext.instance)) != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("[Vulkan] Failed to create instance");
        return;
    }
    
    RAZIX_RHI_LOG_INFO("[Vulkan] Instance created successfully");
    
#ifdef RAZIX_DEBUG
    // Setup debug messenger  
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {0};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = vk_debug_callback;
    
    // Note: In a real implementation, would need to load function pointers
    // if (CHECK_VK(vkCreateDebugUtilsMessengerEXT(VkContext.instance, &debugInfo, NULL, &VkContext.debugMessenger)) == VK_SUCCESS) {
    //     RAZIX_RHI_LOG_INFO("[Vulkan] Debug messenger created successfully");
    // }
#endif
    
    // Note: For a complete implementation, would also:
    // 1. Enumerate and select physical device
    // 2. Create logical device  
    // 3. Get queue families and create queues
    // 4. Create command pools
    // 5. Setup graphics features
    
    // Set graphics features based on Vulkan capabilities
    g_GraphicsFeatures.support.EnableVSync = false;
    g_GraphicsFeatures.support.TesselateTerrain = false; // Would check deviceFeatures.tessellationShader
    g_GraphicsFeatures.support.SupportsBindless = false; // Would check deviceFeatures.shaderSampledImageArrayDynamicIndexing
    g_GraphicsFeatures.support.SupportsWaveIntrinsics = false; // Requires checking for specific extensions
    g_GraphicsFeatures.support.SupportsShaderModel6 = false; // Vulkan uses SPIR-V, not shader models
    g_GraphicsFeatures.support.SupportsNullIndexDescriptors = false;
    g_GraphicsFeatures.support.SupportsTimelineSemaphores = false; // Requires checking for VK_KHR_timeline_semaphore
    g_GraphicsFeatures.support.SupportsBindlessRendering = false;
    g_GraphicsFeatures.MaxBindlessTextures = 4096; // Typical limit
    g_GraphicsFeatures.MinLaneWidth = 32; // Typical for most GPUs
    g_GraphicsFeatures.MaxLaneWidth = 64; // Typical maximum
    
    RAZIX_RHI_LOG_INFO("[Vulkan] Context initialization completed successfully");
}

static void vk_GlobalCtxDestroy(void)
{
    RAZIX_RHI_LOG_INFO("[Vulkan] Destroying Vulkan context");
    
#ifdef RAZIX_DEBUG
    // Destroy debug messenger
    // if (VkContext.debugMessenger != VK_NULL_HANDLE) {
    //     vkDestroyDebugUtilsMessengerEXT(VkContext.instance, VkContext.debugMessenger, NULL);
    //     VkContext.debugMessenger = VK_NULL_HANDLE;
    // }
#endif
    
    // Destroy instance
    if (VkContext.instance != VK_NULL_HANDLE) {
        vkDestroyInstance(VkContext.instance, NULL);
        VkContext.instance = VK_NULL_HANDLE;
    }
    
    RAZIX_RHI_LOG_INFO("[Vulkan] Context destroyed successfully");
}

//---------------------------------------------------------------------------------------------
// Sync Objects
//---------------------------------------------------------------------------------------------

static void vk_CreateSyncobj(void* where, rz_gfx_syncobj_type type)
{
    rz_gfx_syncobj* syncobj = (rz_gfx_syncobj*)where;
    
    // Create semaphore
    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    if (CHECK_VK(vkCreateSemaphore(VkDevice, &semaphoreInfo, NULL, &syncobj->vk.semaphore)) != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("[Vulkan] Failed to create semaphore");
        return;
    }
    
    // Create fence
    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start in signaled state
    
    if (CHECK_VK(vkCreateFence(VkDevice, &fenceInfo, NULL, &syncobj->vk.fence)) != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("[Vulkan] Failed to create fence");
        vkDestroySemaphore(VkDevice, syncobj->vk.semaphore, NULL);
        return;
    }
    
    RAZIX_RHI_LOG_TRACE("[Vulkan] Sync object created successfully");
}

static void vk_DestroySyncobj(rz_gfx_syncobj* syncobj)
{
    if (syncobj->vk.semaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(VkDevice, syncobj->vk.semaphore, NULL);
        syncobj->vk.semaphore = VK_NULL_HANDLE;
    }
    
    if (syncobj->vk.fence != VK_NULL_HANDLE) {
        vkDestroyFence(VkDevice, syncobj->vk.fence, NULL);
        syncobj->vk.fence = VK_NULL_HANDLE;
    }
}

//---------------------------------------------------------------------------------------------
// Command Pools and Buffers
//---------------------------------------------------------------------------------------------

static void vk_CreateCmdPool(void* where)
{
    rz_gfx_cmdpool* cmdPool = (rz_gfx_cmdpool*)where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&cmdPool->resource.handle), "Invalid cmd pool handle");
    
    VkCommandPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    rz_gfx_cmdpool_type poolType = cmdPool->resource.desc.cmdpoolDesc.poolType;
    switch (poolType) {
        case RZ_GFX_CMDPOOL_TYPE_GRAPHICS:
            poolInfo.queueFamilyIndex = VkContext.queueFamilyIndices.graphicsFamily;
            cmdPool->vk.queueFlags = VK_QUEUE_GRAPHICS_BIT;
            break;
        case RZ_GFX_CMDPOOL_TYPE_COMPUTE:
            poolInfo.queueFamilyIndex = VkContext.queueFamilyIndices.computeFamily;
            cmdPool->vk.queueFlags = VK_QUEUE_COMPUTE_BIT;
            break;
        case RZ_GFX_CMDPOOL_TYPE_TRANSFER:
            poolInfo.queueFamilyIndex = VkContext.queueFamilyIndices.transferFamily;
            cmdPool->vk.queueFlags = VK_QUEUE_TRANSFER_BIT;
            break;
        default:
            RAZIX_RHI_LOG_ERROR("[Vulkan] Invalid command pool type");
            return;
    }
    
    if (CHECK_VK(vkCreateCommandPool(VkDevice, &poolInfo, NULL, &cmdPool->vk.cmdPool)) != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("[Vulkan] Failed to create command pool");
        return;
    }
    
    RAZIX_RHI_LOG_TRACE("[Vulkan] Command pool created successfully");
}

static void vk_DestroyCmdPool(void* cmdPool)
{
    RAZIX_RHI_ASSERT(cmdPool != NULL, "Command pool is NULL");
    rz_gfx_cmdpool* pool = (rz_gfx_cmdpool*)cmdPool;
    
    if (pool->vk.cmdPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(VkDevice, pool->vk.cmdPool, NULL);
        pool->vk.cmdPool = VK_NULL_HANDLE;
    }
}

static void vk_CreateCmdBuf(void* where)
{
    rz_gfx_cmdbuf* cmdBuf = (rz_gfx_cmdbuf*)where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&cmdBuf->resource.handle), "Invalid command buffer handle");
    
    const rz_gfx_cmdpool* cmdPool = cmdBuf->resource.desc.cmdbufDesc.pool;
    RAZIX_RHI_ASSERT(cmdPool != NULL, "Command buffer must have a valid command pool");
    
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = cmdPool->vk.cmdPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    
    if (CHECK_VK(vkAllocateCommandBuffers(VkDevice, &allocInfo, &cmdBuf->vk.cmdBuf)) != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("[Vulkan] Failed to allocate command buffer");
        return;
    }
    
    cmdBuf->vk.parentPool = cmdPool->vk.cmdPool;
    cmdBuf->vk.isRecording = false;
    
    RAZIX_RHI_LOG_TRACE("[Vulkan] Command buffer created successfully");
}

static void vk_DestroyCmdBuf(void* cmdBuf)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer is NULL");
    rz_gfx_cmdbuf* buffer = (rz_gfx_cmdbuf*)cmdBuf;
    
    if (buffer->vk.cmdBuf != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(VkDevice, buffer->vk.parentPool, 1, &buffer->vk.cmdBuf);
        buffer->vk.cmdBuf = VK_NULL_HANDLE;
    }
}

//---------------------------------------------------------------------------------------------
// Texture (Basic Implementation for Swapchain)
//---------------------------------------------------------------------------------------------

static void vk_CreateTexture(void* where)
{
    rz_gfx_texture* texture = (rz_gfx_texture*)where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&texture->resource.handle), "Invalid texture handle");
    
    // This is a basic implementation for swapchain textures
    rz_gfx_texture_desc* desc = &texture->resource.desc.textureDesc;
    
    texture->vk.format = vk_util_rz_gfx_format_to_vk_format(desc->format);
    texture->vk.extent = (VkExtent3D){desc->width, desc->height, desc->depth};
    texture->vk.mipLevels = desc->mipLevels;
    texture->vk.arrayLayers = desc->arraySize;
    texture->vk.currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    
    // For swapchain images, the actual VkImage will be set externally
    texture->vk.image = VK_NULL_HANDLE;
    texture->vk.memory = VK_NULL_HANDLE;
    texture->vk.imageView = VK_NULL_HANDLE;
    
    RAZIX_RHI_LOG_TRACE("[Vulkan] Texture structure initialized");
}

static void vk_DestroyTexture(void* texture)
{
    RAZIX_RHI_ASSERT(texture != NULL, "Texture is NULL");
    rz_gfx_texture* tex = (rz_gfx_texture*)texture;
    
    if (tex->vk.imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(VkDevice, tex->vk.imageView, NULL);
        tex->vk.imageView = VK_NULL_HANDLE;
    }
    
    // Note: For swapchain images, we don't destroy the VkImage itself
    // as it's owned by the swapchain
}

//---------------------------------------------------------------------------------------------
// Skeleton implementations for other functions  
//---------------------------------------------------------------------------------------------

static void vk_CreateSwapchain(void* where, void* nativeWindowHandle, uint32_t width, uint32_t height)
{
    // Skeleton implementation - would create VkSwapchainKHR
    RAZIX_RHI_LOG_WARN("[Vulkan] CreateSwapchain - Skeleton implementation");
    (void)where; (void)nativeWindowHandle; (void)width; (void)height;
}

static void vk_DestroySwapchain(rz_gfx_swapchain* sc)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] DestroySwapchain - Skeleton implementation");
    (void)sc;
}

static void vk_CreateShader(void* where)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] CreateShader - Skeleton implementation");
    (void)where;
}

static void vk_DestroyShader(void* shader)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] DestroyShader - Skeleton implementation");
    (void)shader;
}

static void vk_CreateRootSignature(void* where)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] CreateRootSignature - Skeleton implementation");
    (void)where;
}

static void vk_DestroyRootSignature(void* ptr)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] DestroyRootSignature - Skeleton implementation");
    (void)ptr;
}

static void vk_CreatePipeline(void* pipeline)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] CreatePipeline - Skeleton implementation");
    (void)pipeline;
}

static void vk_DestroyPipeline(void* pipeline)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] DestroyPipeline - Skeleton implementation");
    (void)pipeline;
}

static void vk_CreateSampler(void* where)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] CreateSampler - Skeleton implementation");
    (void)where;
}

static void vk_DestroySampler(void* sampler)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] DestroySampler - Skeleton implementation");
    (void)sampler;
}

static void vk_CreateDescriptorHeap(void* where)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] CreateDescriptorHeap - Skeleton implementation");
    (void)where;
}

static void vk_DestroyDescriptorHeap(void* heap)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] DestroyDescriptorHeap - Skeleton implementation");
    (void)heap;
}

static void vk_CreateDescriptorTable(void* where)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] CreateDescriptorTable - Skeleton implementation");
    (void)where;
}

static void vk_DestroyDescriptorTable(void* table)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] DestroyDescriptorTable - Skeleton implementation");
    (void)table;
}

// Rendering functions - skeleton implementations
static void vk_AcquireImage(rz_gfx_swapchain* sc)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] AcquireImage - Skeleton implementation");
    (void)sc;
}

static void vk_WaitOnPrevCmds(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint waitSyncPoint)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] WaitOnPrevCmds - Skeleton implementation");
    (void)syncobj; (void)waitSyncPoint;
}

static void vk_Present(const rz_gfx_swapchain* sc)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] Present - Skeleton implementation");
    (void)sc;
}

static void vk_BeginCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] BeginCmdBuf - Skeleton implementation");
    (void)cmdBuf;
}

static void vk_EndCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] EndCmdBuf - Skeleton implementation");
    (void)cmdBuf;
}

static void vk_SubmitCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] SubmitCmdBuf - Skeleton implementation");
    (void)cmdBuf;
}

static void vk_BeginRenderPass(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_renderpass* renderPass)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] BeginRenderPass - Skeleton implementation");
    (void)cmdBuf; (void)renderPass;
}

static void vk_EndRenderPass(const rz_gfx_cmdbuf* cmdBuf)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] EndRenderPass - Skeleton implementation");
    (void)cmdBuf;
}

static void vk_SetViewport(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_viewport* viewport)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] SetViewport - Skeleton implementation");
    (void)cmdBuf; (void)viewport;
}

static void vk_SetScissorRect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_rect* rect)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] SetScissorRect - Skeleton implementation");
    (void)cmdBuf; (void)rect;
}

static void vk_BindPipeline(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_pipeline* pipeline)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] BindPipeline - Skeleton implementation");
    (void)cmdBuf; (void)pipeline;
}

static void vk_BindGfxRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] BindGfxRootSig - Skeleton implementation");
    (void)cmdBuf; (void)rootSig;
}

static void vk_BindComputeRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] BindComputeRootSig - Skeleton implementation");
    (void)cmdBuf; (void)rootSig;
}

static void vk_DrawAuto(const rz_gfx_cmdbuf* cmdBuf, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] DrawAuto - Skeleton implementation");
    (void)cmdBuf; (void)vertexCount; (void)instanceCount; (void)firstVertex; (void)firstInstance;
}

static void vk_InsertImageBarrier(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* texture, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] InsertImageBarrier - Skeleton implementation");
    (void)cmdBuf; (void)texture; (void)beforeState; (void)afterState;
}

static void vk_InsertTextureReadback(const rz_gfx_texture* texture, rz_gfx_texture_readback* readback)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] InsertTextureReadback - Skeleton implementation");
    (void)texture; (void)readback;
}

static rz_gfx_syncpoint vk_SignalGPU(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint* globalSyncPoint)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] SignalGPU - Skeleton implementation");
    (void)syncobj; (void)globalSyncPoint;
    return 0;
}

static void vk_FlushGPUWork(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint* globalSyncpoint)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] FlushGPUWork - Skeleton implementation");
    (void)syncobj; (void)globalSyncpoint;
}

static void vk_ResizeSwapchain(rz_gfx_swapchain* sc, uint32_t width, uint32_t height)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] ResizeSwapchain - Skeleton implementation");
    (void)sc; (void)width; (void)height;
}

static void vk_BeginFrame(rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] BeginFrame - Skeleton implementation");
    (void)sc; (void)frameSyncobj; (void)frameSyncPoints; (void)globalSyncPoint;
}

static void vk_EndFrame(const rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    RAZIX_RHI_LOG_WARN("[Vulkan] EndFrame - Skeleton implementation");
    (void)sc; (void)frameSyncobj; (void)frameSyncPoints; (void)globalSyncPoint;
}

//---------------------------------------------------------------------------------------------
// Jump table
//---------------------------------------------------------------------------------------------

rz_rhi_api vk_rhi = {
    .GlobalCtxInit        = vk_GlobalCtxInit,         // GlobalCtxInit
    .GlobalCtxDestroy     = vk_GlobalCtxDestroy,      // GlobalCtxDestroy
    .CreateSyncobj        = vk_CreateSyncobj,         // CreateSyncobj
    .DestroySyncobj       = vk_DestroySyncobj,        // DestroySyncobj
    .CreateSwapchain      = vk_CreateSwapchain,       // CreateSwapchain
    .DestroySwapchain     = vk_DestroySwapchain,      // DestroySwapchain
    .CreateCmdPool        = vk_CreateCmdPool,         // CreateCmdPool
    .DestroyCmdPool       = vk_DestroyCmdPool,        // DestroyCmdPool
    .CreateCmdBuf         = vk_CreateCmdBuf,          // CreateCmdBuf
    .DestroyCmdBuf        = vk_DestroyCmdBuf,         // DestroyCmdBuf
    .CreateShader         = vk_CreateShader,          // CreateShader
    .DestroyShader        = vk_DestroyShader,         // DestroyShader
    .CreateRootSignature  = vk_CreateRootSignature,   // CreateRootSignature
    .DestroyRootSignature = vk_DestroyRootSignature,  // DestroyRootSignature
    .CreatePipeline       = vk_CreatePipeline,        // CreatePipeline
    .DestroyPipeline      = vk_DestroyPipeline,       // DestroyPipeline
    .CreateTexture        = vk_CreateTexture,         // CreateTexture
    .DestroyTexture       = vk_DestroyTexture,        // DestroyTexture
    .CreateSampler        = vk_CreateSampler,         // CreateSampler
    .DestroySampler       = vk_DestroySampler,        // DestroySampler
    .CreateDescriptorHeap = vk_CreateDescriptorHeap,  // CreateDescriptorHeap
    .DestroyDescriptorHeap = vk_DestroyDescriptorHeap, // DestroyDescriptorHeap
    .CreateDescriptorTable = vk_CreateDescriptorTable, // CreateDescriptorTable
    .DestroyDescriptorTable = vk_DestroyDescriptorTable, // DestroyDescriptorTable
    
    .AcquireImage       = vk_AcquireImage,          // AcquireImage
    .WaitOnPrevCmds     = vk_WaitOnPrevCmds,        // WaitOnPrevCmds
    .Present            = vk_Present,               // Present
    .BeginCmdBuf        = vk_BeginCmdBuf,           // BeginCmdBuf
    .EndCmdBuf          = vk_EndCmdBuf,             // EndCmdBuf
    .SubmitCmdBuf       = vk_SubmitCmdBuf,          // SubmitCmdBuf
    .BeginRenderPass    = vk_BeginRenderPass,       // BeginRenderPass
    .EndRenderPass      = vk_EndRenderPass,         // EndRenderPass
    .SetViewport        = vk_SetViewport,           // SetViewport
    .SetScissorRect     = vk_SetScissorRect,        // SetScissorRect
    .BindPipeline       = vk_BindPipeline,          // BindPipeline
    .BindGfxRootSig     = vk_BindGfxRootSig,        // BindGfxRootSig
    .BindComputeRootSig = vk_BindComputeRootSig,    // BindComputeRootSig
    
    .DrawAuto              = vk_DrawAuto,               // DrawAuto
    .InsertImageBarrier    = vk_InsertImageBarrier,     // InsertImageBarrier
    .InsertTextureReadback = vk_InsertTextureReadback,  // InsertTextureReadback
    
    .SignalGPU       = vk_SignalGPU,          // SignalGPU
    .FlushGPUWork    = vk_FlushGPUWork,       // FlushGPUWork
    .ResizeSwapchain = vk_ResizeSwapchain,    // ResizeSwapchain
    .BeginFrame      = vk_BeginFrame,         // BeginFrame
    .EndFrame        = vk_EndFrame,           // EndFrame
};