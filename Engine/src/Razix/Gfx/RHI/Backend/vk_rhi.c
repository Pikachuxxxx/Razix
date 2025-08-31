#include "vk_rhi.h"

#include "Razix/Gfx/RHI/rhi.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

#include <vulkan/vulkan.h>

// =============================================================================
// INSTANCE EXTENSIONS - Vulkan 1.3 Compatible
// =============================================================================

#define VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME   "VK_KHR_portability_enumeration"
#define VK_EXT_METAL_SURFACE_EXTENSION_NAME             "VK_EXT_metal_surface"

static const char* requiredInstanceExtensions[] = {
    // Core surface support - required on all platforms
    VK_KHR_SURFACE_EXTENSION_NAME,
    
    // Platform-specific surface extensions
#if defined(RAZIX_PLATFORM_WINDOWS)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif

#if defined(RAZIX_PLATFORM_LINUX) || defined(RAZIX_PLATFORM_UNIX)
    #if defined(VK_USE_PLATFORM_WAYLAND_KHR)
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
    #elif defined(VK_USE_PLATFORM_XCB_KHR)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
    #elif defined(VK_USE_PLATFORM_XLIB_KHR)
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
    #endif
#endif

#if defined(RAZIX_PLATFORM_MACOS)
    VK_EXT_METAL_SURFACE_EXTENSION_NAME,
    // MoltenVK requirements
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif

    // Enhanced physical device queries (promoted to core in 1.1, but extension still needed for compatibility)
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,

    // Debug utilities for development builds
#ifdef RAZIX_DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,  // Required dependency for VK_EXT_debug_marker
#endif
};

// =============================================================================
// DEVICE EXTENSIONS - Vulkan 1.3 Compatible
// =============================================================================

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

static const char* requiredDeviceExtensions[] = {
    // Core presentation support
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    
    // Modern Vulkan 1.3 features (these are promoted to core but extensions still needed for compatibility)
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,        // Core in 1.3
    
    // Descriptor indexing for modern bindless rendering (core in 1.2)
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
    
    // Timeline semaphores for better synchronization (core in 1.2)
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    
    // macOS/MoltenVK compatibility
#ifdef RAZIX_PLATFORM_MACOS
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
#endif

    // Debug markers for development builds
#ifdef RAZIX_DEBUG
    VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
#endif
};

static const char* optionalDeviceExtensions[] = {
    // Improved synchronization
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,        // Core in 1.3
    
    // Memory management (core in 1.1 but extension needed for compatibility)
    VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
    
    // FIXME: Not available on MoltenVK yet, but much needed for improved bindless rendering 
    // Buffer device address (core in 1.2)
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    
    // Extended dynamic state for modern pipelines
    VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
    VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME,
};

// =============================================================================
// VALIDATION LAYERS - Unified Debug Layer
// =============================================================================

#ifdef RAZIX_DEBUG
static const char* validationLayers[] = {
    // Unified validation layer
    "VK_LAYER_KHRONOS_validation",
};
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define REQUIRED_INSTANCE_EXT_COUNT     ARRAY_SIZE(requiredInstanceExtensions)
#define REQUIRED_DEVICE_EXT_COUNT       ARRAY_SIZE(requiredDeviceExtensions)

#ifdef RAZIX_DEBUG
#define VALIDATION_LAYER_COUNT          ARRAY_SIZE(validationLayers)
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
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       pUserData)
{
    (void) messageType;
    (void) pUserData;

    const char* severityStr = "UNKNOWN";
    const char* color       = ANSI_COLOR_RESET;

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            severityStr = "VERBOSE";
            color       = ANSI_COLOR_GRAY;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severityStr = "INFO";
            color       = ANSI_COLOR_CYAN;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severityStr = "WARNING";
            color       = ANSI_COLOR_YELLOW;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severityStr = "ERROR";
            color       = ANSI_COLOR_RED;
            break;
        default:
            break;
    }

    printf("%s[%s] [VK/DEBUG/%s] %s%s\n",
        color,
        _rhi_log_timestamp(),
        severityStr,
        pCallbackData->pMessage,
        ANSI_COLOR_RESET);

    return VK_FALSE;
}
#endif

static const VkImageLayout vulkan_image_layout_map[RZ_GFX_RESOURCE_STATE_COUNT] = {
    VK_IMAGE_LAYOUT_UNDEFINED,                                       // UNDEFINED
    VK_IMAGE_LAYOUT_GENERAL,                                         // COMMON
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                        // GENERIC READ
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                        // RENDER_TARGET
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,                // DEPTH_WRITE
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,                 // DEPTH_READ
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                        // SHADER_READ
    VK_IMAGE_LAYOUT_GENERAL,                                         // UNORDERED_ACCESS
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                            // COPY_SRC
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                            // COPY_DST
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                                 // PRESENT
    VK_IMAGE_LAYOUT_GENERAL,                                         // VERTEX_BUFFER (N/A for images)
    VK_IMAGE_LAYOUT_GENERAL,                                         // INDEX_BUFFER (N/A for images)
    VK_IMAGE_LAYOUT_GENERAL,                                         // CONSTANT_BUFFER (N/A for images)
    VK_IMAGE_LAYOUT_GENERAL,                                         // INDIRECT_ARGUMENT (N/A for images)
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                            // RESOLVE_SRC
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                            // RESOLVE_DST
    VK_IMAGE_LAYOUT_GENERAL,                                         // RAYTRACING_ACCELERATION_STRUCTURE
    VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,    // SHADING_RATE_SOURCE
    VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR,                            // VIDEO_DECODE_READ
    VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR,                            // VIDEO_DECODE_WRITE
};

static VkImageLayout vk_util_res_state_translate(rz_gfx_resource_state state)
{
    if (state >= RZ_GFX_RESOURCE_STATE_COUNT || state == RZ_GFX_RESOURCE_STATE_UNDEFINED) {
        RAZIX_RHI_LOG_ERROR("Invalid resource state %d", state);
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }

    return vulkan_image_layout_map[state];
}

// Add this function to your Vulkan context code

void vk_util_print_gpu_stats(const VkPhysicalDeviceProperties* deviceProps,
                             const VkPhysicalDeviceMemoryProperties* memProps, 
                             const VkPhysicalDeviceFeatures* deviceFeatures) {
    
    RAZIX_RHI_LOG_INFO("===============================================");
    RAZIX_RHI_LOG_INFO("           VULKAN DEVICE INFORMATION          ");
    RAZIX_RHI_LOG_INFO("===============================================");
    
    // Basic device info
    RAZIX_RHI_LOG_INFO("GPU Name        : %s", deviceProps->deviceName);
    RAZIX_RHI_LOG_INFO("Driver Version  : %u.%u.%u", 
                      VK_VERSION_MAJOR(deviceProps->driverVersion),
                      VK_VERSION_MINOR(deviceProps->driverVersion), 
                      VK_VERSION_PATCH(deviceProps->driverVersion));
    RAZIX_RHI_LOG_INFO("API Version     : %u.%u.%u", 
                      VK_VERSION_MAJOR(deviceProps->apiVersion),
                      VK_VERSION_MINOR(deviceProps->apiVersion),
                      VK_VERSION_PATCH(deviceProps->apiVersion));
    
    // Device type with fancy ASCII
    const char* deviceTypeStr = "Unknown";
    const char* deviceIcon = "?";
    switch (deviceProps->deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            deviceTypeStr = "Discrete GPU";
            deviceIcon = "[DGPU]";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            deviceTypeStr = "Integrated GPU";
            deviceIcon = "[iGPU]";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            deviceTypeStr = "Virtual GPU";
            deviceIcon = "[vGPU]";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            deviceTypeStr = "CPU";
            deviceIcon = "[CPU]";
            break;
        default:
            break;
    }
    RAZIX_RHI_LOG_INFO("Device Type     : %s %s", deviceIcon, deviceTypeStr);
    
    // Vendor ID with known vendors
    const char* vendorName = "Unknown";
    switch (deviceProps->vendorID) {
        case 0x1002: vendorName = "AMD"; break;
        case 0x10DE: vendorName = "NVIDIA"; break;
        case 0x8086: vendorName = "Intel"; break;
        case 0x13B5: vendorName = "ARM"; break;
        case 0x5143: vendorName = "Qualcomm"; break;
        case 0x1010: vendorName = "ImgTec"; break;
        default: vendorName = "Unknown"; break;
    }
    RAZIX_RHI_LOG_INFO("Vendor          : %s (ID: 0x%04X)", vendorName, deviceProps->vendorID);
    RAZIX_RHI_LOG_INFO("Device ID       : 0x%04X", deviceProps->deviceID);
    
    // Pretty print UUID (make it readable)
    RAZIX_RHI_LOG_INFO("Device UUID     : %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                      deviceProps->pipelineCacheUUID[0], deviceProps->pipelineCacheUUID[1],
                      deviceProps->pipelineCacheUUID[2], deviceProps->pipelineCacheUUID[3],
                      deviceProps->pipelineCacheUUID[4], deviceProps->pipelineCacheUUID[5],
                      deviceProps->pipelineCacheUUID[6], deviceProps->pipelineCacheUUID[7],
                      deviceProps->pipelineCacheUUID[8], deviceProps->pipelineCacheUUID[9],
                      deviceProps->pipelineCacheUUID[10], deviceProps->pipelineCacheUUID[11],
                      deviceProps->pipelineCacheUUID[12], deviceProps->pipelineCacheUUID[13],
                      deviceProps->pipelineCacheUUID[14], deviceProps->pipelineCacheUUID[15]);
    
    RAZIX_RHI_LOG_INFO("-----------------------------------------------");
    RAZIX_RHI_LOG_INFO("                DEVICE LIMITS                 ");
    RAZIX_RHI_LOG_INFO("-----------------------------------------------");
    
    // Key limits that developers care about
    const VkPhysicalDeviceLimits* limits = &deviceProps->limits;
    RAZIX_RHI_LOG_INFO("Max Texture Size     : %u x %u", limits->maxImageDimension2D, limits->maxImageDimension2D);
    RAZIX_RHI_LOG_INFO("Max 3D Texture Size  : %u x %u x %u", limits->maxImageDimension3D, limits->maxImageDimension3D, limits->maxImageDimension3D);
    RAZIX_RHI_LOG_INFO("Max Uniform Buffer   : %u KB", limits->maxUniformBufferRange / 1024);
    RAZIX_RHI_LOG_INFO("Max Storage Buffer   : %u MB", limits->maxStorageBufferRange / (1024 * 1024));
    RAZIX_RHI_LOG_INFO("Max Push Constants   : %u bytes", limits->maxPushConstantsSize);
    RAZIX_RHI_LOG_INFO("Max Descriptor Sets  : %u", limits->maxBoundDescriptorSets);
    RAZIX_RHI_LOG_INFO("Max Viewports        : %u", limits->maxViewports);
    RAZIX_RHI_LOG_INFO("Max Compute Work Grp : %u x %u x %u", 
                      limits->maxComputeWorkGroupSize[0],
                      limits->maxComputeWorkGroupSize[1], 
                      limits->maxComputeWorkGroupSize[2]);
    
    RAZIX_RHI_LOG_INFO("-----------------------------------------------");
    RAZIX_RHI_LOG_INFO("               MEMORY HEAPS                   ");
    RAZIX_RHI_LOG_INFO("-----------------------------------------------");
    
    // Memory information
    for (uint32_t i = 0; i < memProps->memoryHeapCount; i++) {
        const VkMemoryHeap* heap = &memProps->memoryHeaps[i];
        uint64_t sizeMB = heap->size / (1024 * 1024);
        const char* heapType = (heap->flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "DEVICE_LOCAL" : "HOST_VISIBLE";
        RAZIX_RHI_LOG_INFO("Heap %u [%s]  : %llu MB", i, heapType, sizeMB);
    }
    
    RAZIX_RHI_LOG_INFO("-----------------------------------------------");
    RAZIX_RHI_LOG_INFO("             KEY FEATURES                     ");
    RAZIX_RHI_LOG_INFO("-----------------------------------------------");
    
    // Important features developers need to know
    RAZIX_RHI_LOG_INFO("Geometry Shader      : %s", deviceFeatures->geometryShader ? "[YES]" : "[NO]");
    RAZIX_RHI_LOG_INFO("Tessellation         : %s", deviceFeatures->tessellationShader ? "[YES]" : "[NO]");
    RAZIX_RHI_LOG_INFO("Multi Viewport       : %s", deviceFeatures->multiViewport ? "[YES]" : "[NO]");
    RAZIX_RHI_LOG_INFO("Anisotropic Filter   : %s", deviceFeatures->samplerAnisotropy ? "[YES]" : "[NO]");
    RAZIX_RHI_LOG_INFO("64-bit Float         : %s", deviceFeatures->shaderFloat64 ? "[YES]" : "[NO]");
    RAZIX_RHI_LOG_INFO("64-bit Int           : %s", deviceFeatures->shaderInt64 ? "[YES]" : "[NO]");
    RAZIX_RHI_LOG_INFO("Robust Buffer Access : %s", deviceFeatures->robustBufferAccess ? "[YES]" : "[NO]");
    
#ifdef RAZIX_PLATFORM_MACOS
    RAZIX_RHI_LOG_INFO("-----------------------------------------------");
    RAZIX_RHI_LOG_INFO("      MOLTEN VK DETECTED - APPLE QUIRKS       ");
    RAZIX_RHI_LOG_INFO("-----------------------------------------------");
#endif
    
    RAZIX_RHI_LOG_INFO("===============================================");
}

// Call it like this in your device selection code:
// PrintVulkanDeviceInfo(&VkContext.deviceProperties, &VkContext.memoryProperties, &VkContext.deviceFeatures);

//---------------------------------------------------------------------------------------------

static bool vk_check_validation_layer_support(void)
{
#ifdef RAZIX_DEBUG
    uint32_t layerCount;
    CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, NULL));

    VkLayerProperties* availableLayers = malloc(layerCount * sizeof(VkLayerProperties));
    CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers));

    for (uint32_t i = 0; i < sizeof(validationLayers) / sizeof(validationLayers[0]); i++) {
        bool layerFound = false;

        for (uint32_t j = 0; j < layerCount; j++) {
            if (strcmp(validationLayers[i], availableLayers[j].layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            free(availableLayers);
            return false;
        }
    }

    free(availableLayers);
    return true;
#else
    return true;
#endif
}

static bool vk_check_instance_extension_support(void)
{
    uint32_t extensionCount = 0;
    CHECK_VK(vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL));

    VkExtensionProperties* availableExtensions = malloc(extensionCount * sizeof(VkExtensionProperties));
    CHECK_VK(vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, availableExtensions));

    for (uint32_t i = 0; i < sizeof(requiredInstanceExtensions) / sizeof(requiredInstanceExtensions[0]); i++) {
        bool extensionFound = false;

        for (uint32_t j = 0; j < extensionCount; j++) {
            if (strcmp(requiredInstanceExtensions[i], availableExtensions[j].extensionName) == 0) {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound) {
            RAZIX_RHI_LOG_ERROR("Required instance extension not found: %s", requiredInstanceExtensions[i]);
            free(availableExtensions);
            return false;
        }
    }

    free(availableExtensions);
    return true;
}

static VkQueueFamilyIndices vk_find_queue_families(VkPhysicalDevice device)
{
    VkQueueFamilyIndices indices = {0};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.hasGraphics    = true;
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.computeFamily = i;
            indices.hasCompute    = true;
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transferFamily = i;
            indices.hasTransfer    = true;
        }

        // For now, assume graphics queue can present
        if (indices.hasGraphics) {
            indices.presentFamily = i;
            indices.hasPresent    = true;
        }
    }

    free(queueFamilies);
    return indices;
}

static bool vk_check_device_extension_support(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    CHECK_VK(vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL));

    VkExtensionProperties* availableExtensions = malloc(extensionCount * sizeof(VkExtensionProperties));
    CHECK_VK(vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions));

    for (uint32_t i = 0; i < sizeof(requiredDeviceExtensions) / sizeof(requiredDeviceExtensions[0]); i++) {
        bool extensionFound = false;

        for (uint32_t j = 0; j < extensionCount; j++) {
            if (strcmp(requiredDeviceExtensions[i], availableExtensions[j].extensionName) == 0) {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound) {
            free(availableExtensions);
            return false;
        }
    }

    free(availableExtensions);
    return true;
}

static bool vk_is_device_suitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures   deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    VkQueueFamilyIndices indices             = vk_find_queue_families(device);
    bool                 extensionsSupported = vk_check_device_extension_support(device);

    return indices.hasGraphics && extensionsSupported;
}

static VkPhysicalDevice vk_pick_physical_device(void)
{
    uint32_t deviceCount = 0;
    CHECK_VK(vkEnumeratePhysicalDevices(VkContext.instance, &deviceCount, NULL));

    if (deviceCount == 0) {
        RAZIX_RHI_LOG_ERROR("Failed to find GPUs with Vulkan support");
        return VK_NULL_HANDLE;
    }

    VkPhysicalDevice* devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
    CHECK_VK(vkEnumeratePhysicalDevices(VkContext.instance, &deviceCount, devices));

    VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < deviceCount; i++) {
        if (vk_is_device_suitable(devices[i])) {
            selectedDevice = devices[i];
            break;
        }
    }

    if (selectedDevice == VK_NULL_HANDLE) {
        RAZIX_RHI_LOG_ERROR("Failed to find a suitable GPU");
    }

    free(devices);
    return selectedDevice;
}

static void vk_create_logical_device(void)
{
    VkQueueFamilyIndices indices = vk_find_queue_families(VkContext.gpu);
    VkContext.queueFamilyIndices = indices;

    float                   queuePriority   = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex        = indices.graphicsFamily;
    queueCreateInfo.queueCount              = 1;
    queueCreateInfo.pQueuePriorities        = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo createInfo      = {0};
    createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos       = &queueCreateInfo;
    createInfo.queueCreateInfoCount    = 1;
    createInfo.pEnabledFeatures        = &deviceFeatures;
    createInfo.enabledExtensionCount   = sizeof(requiredDeviceExtensions) / sizeof(requiredDeviceExtensions[0]);
    createInfo.ppEnabledExtensionNames = requiredDeviceExtensions;

#ifdef RAZIX_DEBUG
    createInfo.enabledLayerCount   = sizeof(validationLayers) / sizeof(validationLayers[0]);
    createInfo.ppEnabledLayerNames = validationLayers;
#else
    createInfo.enabledLayerCount = 0;
#endif

    CHECK_VK(vkCreateDevice(VkContext.gpu, &createInfo, NULL, &VkContext.device));


    vkGetDeviceQueue(VkContext.device, indices.graphicsFamily, 0, &VkContext.graphicsQueue);
    vkGetDeviceQueue(VkContext.device, indices.presentFamily, 0, &VkContext.presentQueue);
}

//---------------------------------------------------------------------------------------------

static void vk_GlobalCtxInit(void)
{
    RAZIX_RHI_LOG_INFO("Initializing Vulkan RHI backend");

    // Check validation layer support
    if (!vk_check_validation_layer_support()) {
        RAZIX_RHI_LOG_ERROR("Validation layers requested, but not available");
        return;
    }

    // Check instance extension support
    if (!vk_check_instance_extension_support()) {
        RAZIX_RHI_LOG_ERROR("Required instance extensions not available");
        return;
    }

    // Create Vulkan instance
    VkApplicationInfo appInfo  = {0};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "RazixGame";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "Razix Engine";
    appInfo.engineVersion      = VK_MAKE_VERSION(0, 50, 0); // TODO: Get this from CtxInit 
    appInfo.apiVersion         = RAZIX_VK_API_VERSION;

    VkInstanceCreateInfo createInfo    = {0};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledExtensionCount   = sizeof(requiredInstanceExtensions) / sizeof(requiredInstanceExtensions[0]);
    createInfo.ppEnabledExtensionNames = requiredInstanceExtensions;

#ifdef RAZIX_DEBUG
    createInfo.enabledLayerCount   = sizeof(validationLayers) / sizeof(validationLayers[0]);
    createInfo.ppEnabledLayerNames = validationLayers;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    debugCreateInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = vk_debug_callback;

    createInfo.pNext = &debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
    createInfo.pNext             = NULL;
#endif
    
#ifdef RAZIX_PLATFORM_MACOS
    // MoltenVK requires portability enumeration because Apple decided to be special
    // and not implement Vulkan natively. This flag tells Vulkan to include 
    // non-conformant implementations (like MoltenVK translating to Metal) 
    // in the device enumeration. Because apparently following standards is optional
    // when you're Apple. -_-
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    CHECK_VK(vkCreateInstance(&createInfo, NULL, &VkContext.instance));

#ifdef RAZIX_DEBUG
    // Setup debug messenger
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {0};
    debugInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = vk_debug_callback;

    // FIXME: Use dynamic loading to get proc address
    //CHECK_VK(vkCreateDebugUtilsMessengerEXT(VkContext.instance, &debugInfo, NULL, &VkContext.debugMessenger));
#endif

    // Pick physical device
    VkContext.gpu = vk_pick_physical_device();
    if (VkContext.gpu == VK_NULL_HANDLE) {
        RAZIX_RHI_LOG_ERROR("Failed to pick physical device");
        return;
    }

    // Get device properties and features
    vkGetPhysicalDeviceProperties(VkContext.gpu, &VkContext.deviceProperties);
    vkGetPhysicalDeviceFeatures(VkContext.gpu, &VkContext.deviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(VkContext.gpu, &VkContext.memoryProperties);

    vk_util_print_gpu_stats(&VkContext.deviceProperties, &VkContext.memoryProperties, &VkContext.deviceFeatures);

    // Create logical device
    vk_create_logical_device();

    RAZIX_RHI_LOG_INFO("Vulkan RHI backend initialized successfully");
}

static void vk_GlobalCtxDestroy(void)
{
    RAZIX_RHI_LOG_INFO("Destroying Vulkan RHI backend");

    if (VkContext.device) {
        vkDestroyDevice(VkContext.device, NULL);
        VkContext.device = VK_NULL_HANDLE;
    }

#ifdef RAZIX_DEBUG
    if (VkContext.debugMessenger) {
        // FIXME: Use dynamic loading to get proc address
        // vkDestroyDebugUtilsMessengerEXT(VkContext.instance, VkContext.debugMessenger, NULL);
        VkContext.debugMessenger = VK_NULL_HANDLE;
    }
#endif

    if (VkContext.instance) {
        vkDestroyInstance(VkContext.instance, NULL);
        VkContext.instance = VK_NULL_HANDLE;
    }

    RAZIX_RHI_LOG_INFO("Vulkan RHI backend destroyed");
}

static VkSwapchainSupportDetails vk_query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    VkSwapchainSupportDetails details = {0};

    CHECK_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities));

    CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, NULL));
    if (details.formatCount != 0) {
        details.formats = malloc(details.formatCount * sizeof(VkSurfaceFormatKHR));
        CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, details.formats));
    }

    CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, NULL));
    if (details.presentModeCount != 0) {
        details.presentModes = malloc(details.presentModeCount * sizeof(VkPresentModeKHR));
        CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, details.presentModes));
    }

    return details;
}

static VkSurfaceFormatKHR vk_choose_swap_surface_format(const VkSurfaceFormatKHR* availableFormats, uint32_t formatCount)
{
    for (uint32_t i = 0; i < formatCount; i++) {
        if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

static VkPresentModeKHR vk_choose_swap_present_mode(const VkPresentModeKHR* availablePresentModes, uint32_t presentModeCount)
{
    for (uint32_t i = 0; i < presentModeCount; i++) {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentModes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D vk_choose_swap_extent(const VkSurfaceCapabilitiesKHR* capabilities, uint32_t width, uint32_t height)
{
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    } else {
        VkExtent2D actualExtent = {width, height};

        if (actualExtent.width < capabilities->minImageExtent.width) {
            actualExtent.width = capabilities->minImageExtent.width;
        } else if (actualExtent.width > capabilities->maxImageExtent.width) {
            actualExtent.width = capabilities->maxImageExtent.width;
        }

        if (actualExtent.height < capabilities->minImageExtent.height) {
            actualExtent.height = capabilities->minImageExtent.height;
        } else if (actualExtent.height > capabilities->maxImageExtent.height) {
            actualExtent.height = capabilities->maxImageExtent.height;
        }

        return actualExtent;
    }
}

static void vk_CreateSwapchain(void* where, void* surface, uint32_t width, uint32_t height)
{
    rz_gfx_swapchain* swapchain = (rz_gfx_swapchain*) where;
    memset(swapchain, 0, sizeof(rz_gfx_swapchain));

    RAZIX_RHI_ASSERT(surface != NULL, "VkSurfaceKHR pointer is null, cannot create swapchain without valid surface!");
    VkContext.surface = *(VkSurfaceKHR*)surface;

    swapchain->width  = width;
    swapchain->height = height;

    VkSwapchainSupportDetails swapchainSupport = vk_query_swapchain_support(VkContext.gpu, VkContext.surface);

    VkSurfaceFormatKHR surfaceFormat = vk_choose_swap_surface_format(swapchainSupport.formats, swapchainSupport.formatCount);
    VkPresentModeKHR   presentMode   = vk_choose_swap_present_mode(swapchainSupport.presentModes, swapchainSupport.presentModeCount);
    VkExtent2D         extent        = vk_choose_swap_extent(&swapchainSupport.capabilities, width, height);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface                  = VkContext.surface;
    createInfo.minImageCount            = imageCount;
    createInfo.imageFormat              = surfaceFormat.format;
    createInfo.imageColorSpace          = surfaceFormat.colorSpace;
    createInfo.imageExtent              = extent;
    createInfo.imageArrayLayers         = 1;
    createInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkQueueFamilyIndices indices              = VkContext.queueFamilyIndices;
    uint32_t             queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices   = NULL;
    }

    createInfo.preTransform   = swapchainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode    = presentMode;
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = VK_NULL_HANDLE;

    CHECK_VK(vkCreateSwapchainKHR(VkContext.device, &createInfo, NULL, &swapchain->vk.swapchain));

    swapchain->vk.imageFormat = surfaceFormat.format;
    swapchain->vk.extent      = extent;

    // Get swapchain images
    CHECK_VK(vkGetSwapchainImagesKHR(VkContext.device, swapchain->vk.swapchain, &swapchain->vk.imageCount, NULL));
    swapchain->vk.images = malloc(swapchain->vk.imageCount * sizeof(VkImage));
    CHECK_VK(vkGetSwapchainImagesKHR(VkContext.device, swapchain->vk.swapchain, &swapchain->vk.imageCount, swapchain->vk.images));

    swapchain->imageCount = swapchain->vk.imageCount;

    // Cleanup support details
    free(swapchainSupport.formats);
    free(swapchainSupport.presentModes);

    RAZIX_RHI_LOG_INFO("Vulkan swapchain created: %ux%u, %u images", width, height, imageCount);
}

static void vk_DestroySwapchain(rz_gfx_swapchain* sc)
{
    if (sc->vk.images) {
        free(sc->vk.images);
        sc->vk.images = NULL;
    }

    if (sc->vk.swapchain) {
        vkDestroySwapchainKHR(VkContext.device, sc->vk.swapchain, NULL);
        sc->vk.swapchain = VK_NULL_HANDLE;
    }

    if (VkContext.surface) {
        vkDestroySurfaceKHR(VkContext.instance, VkContext.surface, NULL);
        VkContext.surface = VK_NULL_HANDLE;
    }

    RAZIX_RHI_LOG_INFO("Vulkan swapchain destroyed");
}

//---------------------------------------------------------------------------------------------
// Empty stub implementations for all other RHI functions
//---------------------------------------------------------------------------------------------

static void vk_CreateSyncobj(void* where, rz_gfx_syncobj_type type)
{
    (void) where;
    (void) type;
    // TODO: Implement when needed
}

static void vk_DestroySyncobj(rz_gfx_syncobj* syncobj)
{
    (void) syncobj;
    // TODO: Implement when needed
}

static void vk_CreateCmdPool(void* where)
{
    (void) where;
    // TODO: Implement when needed
}

static void vk_DestroyCmdPool(void* cmdPool)
{
    (void) cmdPool;
    // TODO: Implement when needed
}

static void vk_CreateCmdBuf(void* where)
{
    (void) where;
    // TODO: Implement when needed
}

static void vk_DestroyCmdBuf(void* cmdBuf)
{
    (void) cmdBuf;
    // TODO: Implement when needed
}

static void vk_CreateShader(void* where)
{
    (void) where;
    // TODO: Implement when needed
}

static void vk_DestroyShader(void* shader)
{
    (void) shader;
    // TODO: Implement when needed
}

static void vk_CreateRootSignature(void* where)
{
    (void) where;
    // TODO: Implement when needed
}

static void vk_DestroyRootSignature(void* ptr)
{
    (void) ptr;
    // TODO: Implement when needed
}

static void vk_CreatePipeline(void* pipeline)
{
    (void) pipeline;
    // TODO: Implement when needed
}

static void vk_DestroyPipeline(void* pipeline)
{
    (void) pipeline;
    // TODO: Implement when needed
}

static void vk_CreateTexture(void* where)
{
    (void) where;
    // TODO: Implement when needed
}

static void vk_DestroyTexture(void* texture)
{
    (void) texture;
    // TODO: Implement when needed
}

static void vk_CreateSampler(void* where)
{
    (void) where;
    // TODO: Implement when needed
}

static void vk_DestroySampler(void* sampler)
{
    (void) sampler;
    // TODO: Implement when needed
}

static void vk_CreateDescriptorHeap(void* where)
{
    (void) where;
    // TODO: Implement when needed
}

static void vk_DestroyDescriptorHeap(void* heap)
{
    (void) heap;
    // TODO: Implement when needed
}

static void vk_CreateDescriptorTable(void* where)
{
    (void) where;
    // TODO: Implement when needed
}

static void vk_DestroyDescriptorTable(void* table)
{
    (void) table;
    // TODO: Implement when needed
}

static void vk_AcquireImage(rz_gfx_swapchain* sc)
{
    assert(sc != NULL && "Swapchain cannot be null");
    assert(sc->vk.swapchain != VK_NULL_HANDLE && "Vulkan swapchain is invalid");

    VkResult result = vkAcquireNextImageKHR(
        VkContext.device,
        sc->vk.swapchain,
        UINT64_MAX,        // No timeout
        VK_NULL_HANDLE,    // No semaphore for now
        VK_NULL_HANDLE,    // No fence for now
        &sc->vk.currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RAZIX_RHI_LOG_WARN("Swapchain out of date or suboptimal, image index: %u", sc->vk.currentImageIndex);
        // TODO: Handle swapchain recreation
    } else if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("Failed to acquire swapchain image: %d", result);
        return;
    }

    RAZIX_RHI_LOG_TRACE("Acquired swapchain image index: %u", sc->vk.currentImageIndex);
}

static void vk_WaitOnPrevCmds(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint waitSyncPoint)
{
    assert(syncobj != NULL && "Sync object cannot be null");

    // Basic fence waiting implementation
    if (syncobj->vk.fence != VK_NULL_HANDLE) {
        VkResult result = vkWaitForFences(VkContext.device, 1, &syncobj->vk.fence, VK_TRUE, UINT64_MAX);
        if (result != VK_SUCCESS) {
            RAZIX_RHI_LOG_ERROR("Failed to wait for fence: %d", result);
            return;
        }
        RAZIX_RHI_LOG_TRACE("Waited on fence (sync point: %llu)", (unsigned long long) waitSyncPoint);
    } else {
        RAZIX_RHI_LOG_TRACE("No fence to wait on (sync point: %llu)", (unsigned long long) waitSyncPoint);
    }
}

static void vk_Present(const rz_gfx_swapchain* sc)
{
    assert(sc != NULL && "Swapchain cannot be null");
    assert(sc->vk.swapchain != VK_NULL_HANDLE && "Vulkan swapchain is invalid");

    VkPresentInfoKHR presentInfo = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 0,    // No semaphores for now
        .pWaitSemaphores    = NULL,
        .swapchainCount     = 1,
        .pSwapchains        = &sc->vk.swapchain,
        .pImageIndices      = &sc->vk.currentImageIndex,
        .pResults           = NULL    // Optional
    };

    VkResult result = vkQueuePresentKHR(VkContext.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RAZIX_RHI_LOG_WARN("Swapchain out of date or suboptimal during present");
        // TODO: Handle swapchain recreation
    } else if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("Failed to present swapchain image: %d", result);
        return;
    }

    RAZIX_RHI_LOG_TRACE("Presented swapchain image index: %u", sc->vk.currentImageIndex);
}

static void vk_BeginCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    assert(cmdBuf != NULL && "Command buffer cannot be null");
    assert(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE && "Vulkan command buffer is invalid");

    VkCommandBufferBeginInfo beginInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,    // Assuming one-time usage for most cases
        .pInheritanceInfo = NULL};

    VkResult result = vkBeginCommandBuffer(cmdBuf->vk.cmdBuf, &beginInfo);
    if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("Failed to begin command buffer recording: %d", result);
        return;
    }

    // Mark as recording (if struct supports it)
    // cmdBuf->vk.isRecording = true;  // Uncomment if vk_cmdbuf has this field

    RAZIX_RHI_LOG_TRACE("Command buffer recording started");
}

static void vk_EndCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    assert(cmdBuf != NULL && "Command buffer cannot be null");
    assert(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE && "Vulkan command buffer is invalid");

    VkResult result = vkEndCommandBuffer(cmdBuf->vk.cmdBuf);
    if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("Failed to end command buffer recording: %d", result);
        return;
    }

    // Mark as not recording (if struct supports it)
    // cmdBuf->vk.isRecording = false;  // Uncomment if vk_cmdbuf has this field

    RAZIX_RHI_LOG_TRACE("Command buffer recording ended");
}

static void vk_SubmitCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    (void) cmdBuf;
    // TODO: Implement when needed
}

static void vk_BeginRenderPass(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_renderpass* renderPass)
{
    // TODO: Implement when needed
}

static void vk_EndRenderPass(const rz_gfx_cmdbuf* cmdBuf)
{
    // TODO: Implement when needed
}

static void vk_SetViewport(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_viewport* viewport)
{
    assert(cmdBuf != NULL && "Command buffer cannot be null");
    assert(viewport != NULL && "Viewport cannot be null");
    assert(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE && "Vulkan command buffer is invalid");

    VkViewport vkViewport = {
        .x        = (float) viewport->x,
        .y        = (float) viewport->y,
        .width    = (float) viewport->width,
        .height   = (float) viewport->height,
        .minDepth = (float) viewport->minDepth / 65535.0f,    // Convert from uint32 to normalized float
        .maxDepth = (float) viewport->maxDepth / 65535.0f     // Convert from uint32 to normalized float
    };

    vkCmdSetViewport(cmdBuf->vk.cmdBuf, 0, 1, &vkViewport);

    RAZIX_RHI_LOG_TRACE("Viewport set: %.1f,%.1f %.1fx%.1f depth[%.3f-%.3f]",
        vkViewport.x,
        vkViewport.y,
        vkViewport.width,
        vkViewport.height,
        vkViewport.minDepth,
        vkViewport.maxDepth);
}

static void vk_SetScissorRect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_rect* rect)
{
    assert(cmdBuf != NULL && "Command buffer cannot be null");
    assert(rect != NULL && "Scissor rect cannot be null");
    assert(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE && "Vulkan command buffer is invalid");

    VkRect2D scissor = {
        .offset = {.x = rect->x, .y = rect->y},
        .extent = {.width = rect->width, .height = rect->height}};

    vkCmdSetScissor(cmdBuf->vk.cmdBuf, 0, 1, &scissor);

    RAZIX_RHI_LOG_TRACE("Scissor rect set: %d,%d %dx%d",
        rect->x,
        rect->y,
        rect->width,
        rect->height);
}

static void vk_BindPipeline(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_pipeline* pipeline)
{
    assert(cmdBuf != NULL && "Command buffer cannot be null");
    assert(pipeline != NULL && "Pipeline cannot be null");
    assert(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE && "Vulkan command buffer is invalid");
    assert(pipeline->vk.pipeline != VK_NULL_HANDLE && "Vulkan pipeline is invalid");

    // Determine the pipeline bind point based on pipeline type
    VkPipelineBindPoint bindPoint;
    if (pipeline->resource.desc.pipelineDesc.type == RZ_GFX_PIPELINE_TYPE_GRAPHICS) {
        bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    } else if (pipeline->resource.desc.pipelineDesc.type == RZ_GFX_PIPELINE_TYPE_COMPUTE) {
        bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    } else {
        RAZIX_RHI_LOG_ERROR("Unknown pipeline type: %d", pipeline->resource.desc.pipelineDesc.type);
        return;
    }

    vkCmdBindPipeline(cmdBuf->vk.cmdBuf, bindPoint, pipeline->vk.pipeline);

    RAZIX_RHI_LOG_TRACE("Pipeline bound (type: %s)",
        bindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS ? "Graphics" : "Compute");
}

static void vk_BindGfxRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    assert(cmdBuf != NULL && "Command buffer cannot be null");
    assert(rootSig != NULL && "Root signature cannot be null");
    assert(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE && "Vulkan command buffer is invalid");
    assert(rootSig->vk.pipelineLayout != VK_NULL_HANDLE && "Vulkan pipeline layout is invalid");

    // In Vulkan, root signatures correspond to descriptor sets bound to pipeline layouts
    // For now, we'll just bind the pipeline layout - descriptor sets need to be bound separately
    // This is a placeholder implementation that sets up the pipeline layout binding

    RAZIX_RHI_LOG_TRACE("Graphics root signature bound (pipeline layout ready)");

    // Note: Actual descriptor set binding would happen when descriptors are available
    // vkCmdBindDescriptorSets(cmdBuf->vk.cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         rootSig->vk.pipelineLayout, 0, descriptorSetCount,
    //                         pDescriptorSets, 0, nullptr);
}

static void vk_BindComputeRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    assert(cmdBuf != NULL && "Command buffer cannot be null");
    assert(rootSig != NULL && "Root signature cannot be null");
    assert(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE && "Vulkan command buffer is invalid");
    assert(rootSig->vk.pipelineLayout != VK_NULL_HANDLE && "Vulkan pipeline layout is invalid");

    // In Vulkan, root signatures correspond to descriptor sets bound to pipeline layouts
    // For now, we'll just bind the pipeline layout - descriptor sets need to be bound separately
    // This is a placeholder implementation that sets up the pipeline layout binding

    RAZIX_RHI_LOG_TRACE("Compute root signature bound (pipeline layout ready)");

    // Note: Actual descriptor set binding would happen when descriptors are available
    // vkCmdBindDescriptorSets(cmdBuf->vk.cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE,
    //                         rootSig->vk.pipelineLayout, 0, descriptorSetCount,
    //                         pDescriptorSets, 0, nullptr);
}

static void vk_DrawAuto(const rz_gfx_cmdbuf* cmdBuf, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    assert(cmdBuf != NULL && "Command buffer cannot be null");
    assert(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE && "Vulkan command buffer is invalid");
    assert(vertexCount > 0 && "Vertex count must be greater than 0");
    assert(instanceCount > 0 && "Instance count must be greater than 0");

    vkCmdDraw(cmdBuf->vk.cmdBuf, vertexCount, instanceCount, firstVertex, firstInstance);

    RAZIX_RHI_LOG_TRACE("Draw command: %u vertices, %u instances (start: %u, %u)",
        vertexCount,
        instanceCount,
        firstVertex,
        firstInstance);
}

static void vk_InsertImageBarrier(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_texture* texture, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    (void) cmdBuf;
    (void) texture;
    (void) beforeState;
    (void) afterState;
    // TODO: Implement when needed
}

static void vk_InsertTextureReadback(const rz_gfx_texture* texture, rz_gfx_texture_readback* readback)
{
    (void) texture;
    (void) readback;
    // TODO: Implement when needed
}

static rz_gfx_syncpoint vk_SignalGPU(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint* globalSyncPoint)
{
    (void) syncobj;
    (void) globalSyncPoint;
    // TODO: Implement when needed
    rz_gfx_syncpoint result = {0};
    return result;
}

static void vk_FlushGPUWork(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint* globalSyncpoint)
{
    (void) syncobj;
    (void) globalSyncpoint;
    // TODO: Implement when needed
}

static void vk_ResizeSwapchain(rz_gfx_swapchain* sc, uint32_t width, uint32_t height)
{
    (void) sc;
    (void) width;
    (void) height;
    // TODO: Implement when needed
}

static void vk_BeginFrame(rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    // TODO: Implement when needed
}

static void vk_EndFrame(const rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    // TODO: Implement when needed
}

//---------------------------------------------------------------------------------------------
// Jump table
//---------------------------------------------------------------------------------------------

rz_rhi_api vk_rhi = {
    .GlobalCtxInit          = vk_GlobalCtxInit,             // GlobalCtxInit
    .GlobalCtxDestroy       = vk_GlobalCtxDestroy,          // GlobalCtxDestroy
    .CreateSyncobj          = vk_CreateSyncobj,             // CreateSyncobj
    .DestroySyncobj         = vk_DestroySyncobj,            // DestroySyncobj
    .CreateSwapchain        = vk_CreateSwapchain,           // CreateSwapchain
    .DestroySwapchain       = vk_DestroySwapchain,          // DestroySwapchain
    .CreateCmdPool          = vk_CreateCmdPool,             // CreateCmdPool
    .DestroyCmdPool         = vk_DestroyCmdPool,            // DestroyCmdPool
    .CreateCmdBuf           = vk_CreateCmdBuf,              // CreateCmdBuf
    .DestroyCmdBuf          = vk_DestroyCmdBuf,             // DestroyCmdBuf
    .CreateShader           = vk_CreateShader,              // CreateShader
    .DestroyShader          = vk_DestroyShader,             // DestroyShader
    .CreateRootSignature    = vk_CreateRootSignature,       // CreateRootSignature
    .DestroyRootSignature   = vk_DestroyRootSignature,      // DestroyRootSignature
    .CreatePipeline         = vk_CreatePipeline,            // CreatePipeline
    .DestroyPipeline        = vk_DestroyPipeline,           // DestroyPipeline
    .CreateTexture          = vk_CreateTexture,             // CreateTexture
    .DestroyTexture         = vk_DestroyTexture,            // DestroyTexture
    .CreateSampler          = vk_CreateSampler,             // CreateSampler
    .DestroySampler         = vk_DestroySampler,            // DestroySampler
    .CreateDescriptorHeap   = vk_CreateDescriptorHeap,      // CreateDescriptorHeap
    .DestroyDescriptorHeap  = vk_DestroyDescriptorHeap,     // DestroyDescriptorHeap
    .CreateDescriptorTable  = vk_CreateDescriptorTable,     // CreateDescriptorTable
    .DestroyDescriptorTable = vk_DestroyDescriptorTable,    // DestroyDescriptorTable

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

    .DrawAuto              = vk_DrawAuto,                 // DrawAuto
    .InsertImageBarrier    = vk_InsertImageBarrier,       // InsertImageBarrier
    .InsertTextureReadback = vk_InsertTextureReadback,    // InsertTextureReadback

    .SignalGPU       = vk_SignalGPU,          // SignalGPU
    .FlushGPUWork    = vk_FlushGPUWork,       // FlushGPUWork
    .ResizeSwapchain = vk_ResizeSwapchain,    // ResizeSwapchain
    .BeginFrame      = vk_BeginFrame,         // BeginFrame
    .EndFrame        = vk_EndFrame,           // EndFrame
};
