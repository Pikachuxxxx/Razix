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
// Function pointer declarations for dynamic loading
//---------------------------------------------------------------------------------------------

// Core Vulkan functions
PFN_vkCreateInstance                    vkCreateInstance;
PFN_vkDestroyInstance                   vkDestroyInstance;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties  vkEnumerateInstanceLayerProperties;
PFN_vkGetInstanceProcAddr               vkGetInstanceProcAddr;

// Instance-level functions
PFN_vkEnumeratePhysicalDevices          vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceProperties       vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceFeatures         vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkCreateDevice                      vkCreateDevice;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;

// Device-level functions
PFN_vkDestroyDevice                     vkDestroyDevice;
PFN_vkGetDeviceQueue                    vkGetDeviceQueue;

// Surface functions
PFN_vkDestroySurfaceKHR                 vkDestroySurfaceKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;

// Swapchain functions
PFN_vkCreateSwapchainKHR                vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR               vkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR             vkGetSwapchainImagesKHR;

#ifdef RAZIX_DEBUG
// Debug functions
PFN_vkCreateDebugUtilsMessengerEXT      vkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT     vkDestroyDebugUtilsMessengerEXT;
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
PFN_vkCreateWin32SurfaceKHR             vkCreateWin32SurfaceKHR;
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
// Dynamic loading utilities
//---------------------------------------------------------------------------------------------

#ifdef RAZIX_PLATFORM_WINDOWS
#include <windows.h>
static HMODULE vulkan_lib = NULL;

static bool vk_load_vulkan_library(void)
{
    vulkan_lib = LoadLibraryA("vulkan-1.dll");
    if (!vulkan_lib) {
        RAZIX_RHI_LOG_ERROR("Failed to load vulkan-1.dll");
        return false;
    }
    
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkan_lib, "vkGetInstanceProcAddr");
    if (!vkGetInstanceProcAddr) {
        RAZIX_RHI_LOG_ERROR("Failed to get vkGetInstanceProcAddr");
        return false;
    }
    
    return true;
}

static void vk_unload_vulkan_library(void)
{
    if (vulkan_lib) {
        FreeLibrary(vulkan_lib);
        vulkan_lib = NULL;
    }
}
#else
#include <dlfcn.h>
static void* vulkan_lib = NULL;

static bool vk_load_vulkan_library(void)
{
    vulkan_lib = dlopen("libvulkan.so.1", RTLD_NOW);
    if (!vulkan_lib) {
        vulkan_lib = dlopen("libvulkan.so", RTLD_NOW);
    }
    
    if (!vulkan_lib) {
        RAZIX_RHI_LOG_ERROR("Failed to load Vulkan library: %s", dlerror());
        return false;
    }
    
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_lib, "vkGetInstanceProcAddr");
    if (!vkGetInstanceProcAddr) {
        RAZIX_RHI_LOG_ERROR("Failed to get vkGetInstanceProcAddr");
        return false;
    }
    
    return true;
}

static void vk_unload_vulkan_library(void)
{
    if (vulkan_lib) {
        dlclose(vulkan_lib);
        vulkan_lib = NULL;
    }
}
#endif

static void vk_load_global_functions(void)
{
    vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(NULL, "vkCreateInstance");
    vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
}

static void vk_load_instance_functions(VkInstance instance)
{
    vkDestroyInstance = (PFN_vkDestroyInstance)vkGetInstanceProcAddr(instance, "vkDestroyInstance");
    vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices");
    vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties");
    vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures");
    vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    vkCreateDevice = (PFN_vkCreateDevice)vkGetInstanceProcAddr(instance, "vkCreateDevice");
    vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)vkGetInstanceProcAddr(instance, "vkEnumerateDeviceExtensionProperties");
    vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties");
    
    // Surface functions
    vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR");
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
    
#ifdef VK_USE_PLATFORM_WIN32_KHR
    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
#endif

#ifdef RAZIX_DEBUG
    vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
#endif
}

static void vk_load_device_functions(VkDevice device)
{
    vkDestroyDevice = (PFN_vkDestroyDevice)vkGetInstanceProcAddr(VkContext.instance, "vkDestroyDevice");
    vkGetDeviceQueue = (PFN_vkGetDeviceQueue)vkGetInstanceProcAddr(VkContext.instance, "vkGetDeviceQueue");
    vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)vkGetInstanceProcAddr(VkContext.instance, "vkCreateSwapchainKHR");
    vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)vkGetInstanceProcAddr(VkContext.instance, "vkDestroySwapchainKHR");
    vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetInstanceProcAddr(VkContext.instance, "vkGetSwapchainImagesKHR");
}

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
        default:
            break;
    }
    
    printf("%s[%s] [VK/DEBUG/%s] %s%s\n", 
           color, _rhi_log_timestamp(), severityStr, 
           pCallbackData->pMessage, ANSI_COLOR_RESET);
    
    return VK_FALSE;
}
#endif

//---------------------------------------------------------------------------------------------
// Context creation and management
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
            indices.hasGraphics = true;
        }
        
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.computeFamily = i;
            indices.hasCompute = true;
        }
        
        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transferFamily = i;
            indices.hasTransfer = true;
        }
        
        // For now, assume graphics queue can present
        if (indices.hasGraphics) {
            indices.presentFamily = i;
            indices.hasPresent = true;
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
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    VkQueueFamilyIndices indices = vk_find_queue_families(device);
    bool extensionsSupported = vk_check_device_extension_support(device);
    
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
    
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    
    VkPhysicalDeviceFeatures deviceFeatures = {0};
    
    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = sizeof(requiredDeviceExtensions) / sizeof(requiredDeviceExtensions[0]);
    createInfo.ppEnabledExtensionNames = requiredDeviceExtensions;
    
#ifdef RAZIX_DEBUG
    createInfo.enabledLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
    createInfo.ppEnabledLayerNames = validationLayers;
#else
    createInfo.enabledLayerCount = 0;
#endif
    
    CHECK_VK(vkCreateDevice(VkContext.gpu, &createInfo, NULL, &VkContext.device));
    
    vk_load_device_functions(VkContext.device);
    
    vkGetDeviceQueue(VkContext.device, indices.graphicsFamily, 0, &VkContext.graphicsQueue);
    vkGetDeviceQueue(VkContext.device, indices.presentFamily, 0, &VkContext.presentQueue);
}

static void vk_GlobalCtxInit(void)
{
    RAZIX_RHI_LOG_INFO("Initializing Vulkan RHI backend");
    
    // Initialize Vulkan library
    if (!vk_load_vulkan_library()) {
        RAZIX_RHI_LOG_ERROR("Failed to load Vulkan library");
        return;
    }
    
    vk_load_global_functions();
    
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
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Razix Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Razix";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = sizeof(requiredInstanceExtensions) / sizeof(requiredInstanceExtensions[0]);
    createInfo.ppEnabledExtensionNames = requiredInstanceExtensions;
    
#ifdef RAZIX_DEBUG
    createInfo.enabledLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
    createInfo.ppEnabledLayerNames = validationLayers;
    
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
    
    CHECK_VK(vkCreateInstance(&createInfo, NULL, &VkContext.instance));
    
    vk_load_instance_functions(VkContext.instance);
    
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
    
    CHECK_VK(vkCreateDebugUtilsMessengerEXT(VkContext.instance, &debugInfo, NULL, &VkContext.debugMessenger));
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
    
    RAZIX_RHI_LOG_INFO("Selected GPU: %s", VkContext.deviceProperties.deviceName);
    
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
        vkDestroyDebugUtilsMessengerEXT(VkContext.instance, VkContext.debugMessenger, NULL);
        VkContext.debugMessenger = VK_NULL_HANDLE;
    }
#endif
    
    if (VkContext.instance) {
        vkDestroyInstance(VkContext.instance, NULL);
        VkContext.instance = VK_NULL_HANDLE;
    }
    
    vk_unload_vulkan_library();
    
    RAZIX_RHI_LOG_INFO("Vulkan RHI backend destroyed");
}

//---------------------------------------------------------------------------------------------
// Swapchain implementation  
//---------------------------------------------------------------------------------------------

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

static void vk_CreateSwapchain(void* where, void* nativeWindowHandle, uint32_t width, uint32_t height)
{
    rz_gfx_swapchain* swapchain = (rz_gfx_swapchain*)where;
    memset(swapchain, 0, sizeof(rz_gfx_swapchain));
    
    swapchain->width = width;
    swapchain->height = height;
    
    // Create surface (platform-specific)
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {0};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = *(HWND*)nativeWindowHandle;
    surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
    
    CHECK_VK(vkCreateWin32SurfaceKHR(VkContext.instance, &surfaceCreateInfo, NULL, &VkContext.surface));
#else
    // TODO: Add support for other platforms
    RAZIX_RHI_LOG_ERROR("Platform surface creation not implemented");
    return;
#endif
    
    VkSwapchainSupportDetails swapchainSupport = vk_query_swapchain_support(VkContext.gpu, VkContext.surface);
    
    VkSurfaceFormatKHR surfaceFormat = vk_choose_swap_surface_format(swapchainSupport.formats, swapchainSupport.formatCount);
    VkPresentModeKHR presentMode = vk_choose_swap_present_mode(swapchainSupport.presentModes, swapchainSupport.presentModeCount);
    VkExtent2D extent = vk_choose_swap_extent(&swapchainSupport.capabilities, width, height);
    
    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = VkContext.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    VkQueueFamilyIndices indices = VkContext.queueFamilyIndices;
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};
    
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = NULL;
    }
    
    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    
    CHECK_VK(vkCreateSwapchainKHR(VkContext.device, &createInfo, NULL, &swapchain->vk.swapchain));
    
    swapchain->vk.imageFormat = surfaceFormat.format;
    swapchain->vk.extent = extent;
    
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
    (void)where; (void)type;
    // TODO: Implement when needed
}

static void vk_DestroySyncobj(rz_gfx_syncobj* syncobj)
{
    (void)syncobj;
    // TODO: Implement when needed
}

static void vk_CreateCmdPool(void* where)
{
    (void)where;
    // TODO: Implement when needed
}

static void vk_DestroyCmdPool(void* cmdPool)
{
    (void)cmdPool;
    // TODO: Implement when needed
}

static void vk_CreateCmdBuf(void* where)
{
    (void)where;
    // TODO: Implement when needed
}

static void vk_DestroyCmdBuf(void* cmdBuf)
{
    (void)cmdBuf;
    // TODO: Implement when needed
}

static void vk_CreateShader(void* where)
{
    (void)where;
    // TODO: Implement when needed
}

static void vk_DestroyShader(void* shader)
{
    (void)shader;
    // TODO: Implement when needed
}

static void vk_CreateRootSignature(void* where)
{
    (void)where;
    // TODO: Implement when needed
}

static void vk_DestroyRootSignature(void* ptr)
{
    (void)ptr;
    // TODO: Implement when needed
}

static void vk_CreatePipeline(void* pipeline)
{
    (void)pipeline;
    // TODO: Implement when needed
}

static void vk_DestroyPipeline(void* pipeline)
{
    (void)pipeline;
    // TODO: Implement when needed
}

static void vk_CreateTexture(void* where)
{
    (void)where;
    // TODO: Implement when needed
}

static void vk_DestroyTexture(void* texture)
{
    (void)texture;
    // TODO: Implement when needed
}

static void vk_CreateSampler(void* where)
{
    (void)where;
    // TODO: Implement when needed
}

static void vk_DestroySampler(void* sampler)
{
    (void)sampler;
    // TODO: Implement when needed
}

static void vk_CreateDescriptorHeap(void* where)
{
    (void)where;
    // TODO: Implement when needed
}

static void vk_DestroyDescriptorHeap(void* heap)
{
    (void)heap;
    // TODO: Implement when needed
}

static void vk_CreateDescriptorTable(void* where)
{
    (void)where;
    // TODO: Implement when needed
}

static void vk_DestroyDescriptorTable(void* table)
{
    (void)table;
    // TODO: Implement when needed
}

static void vk_AcquireImage(rz_gfx_swapchain* sc)
{
    (void)sc;
    // TODO: Implement when needed
}

static void vk_WaitOnPrevCmds(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint waitSyncPoint)
{
    (void)syncobj; (void)waitSyncPoint;
    // TODO: Implement when needed
}

static void vk_Present(const rz_gfx_swapchain* sc)
{
    (void)sc;
    // TODO: Implement when needed
}

static void vk_BeginCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    (void)cmdBuf;
    // TODO: Implement when needed
}

static void vk_EndCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    (void)cmdBuf;
    // TODO: Implement when needed
}

static void vk_SubmitCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    (void)cmdBuf;
    // TODO: Implement when needed
}

static void vk_BeginRenderPass(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_renderpass* renderPass)
{
    (void)cmdBuf; (void)renderPass;
    // TODO: Implement when needed
}

static void vk_EndRenderPass(const rz_gfx_cmdbuf* cmdBuf)
{
    (void)cmdBuf;
    // TODO: Implement when needed
}

static void vk_SetViewport(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_viewport* viewport)
{
    (void)cmdBuf; (void)viewport;
    // TODO: Implement when needed
}

static void vk_SetScissorRect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_rect* rect)
{
    (void)cmdBuf; (void)rect;
    // TODO: Implement when needed
}

static void vk_BindPipeline(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_pipeline* pipeline)
{
    (void)cmdBuf; (void)pipeline;
    // TODO: Implement when needed
}

static void vk_BindGfxRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    (void)cmdBuf; (void)rootSig;
    // TODO: Implement when needed
}

static void vk_BindComputeRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    (void)cmdBuf; (void)rootSig;
    // TODO: Implement when needed
}

static void vk_DrawAuto(const rz_gfx_cmdbuf* cmdBuf, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    (void)cmdBuf; (void)vertexCount; (void)instanceCount; (void)firstVertex; (void)firstInstance;
    // TODO: Implement when needed
}

static void vk_InsertImageBarrier(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* texture, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    (void)cmdBuf; (void)texture; (void)beforeState; (void)afterState;
    // TODO: Implement when needed
}

static void vk_InsertTextureReadback(const rz_gfx_texture* texture, rz_gfx_texture_readback* readback)
{
    (void)texture; (void)readback;
    // TODO: Implement when needed
}

static rz_gfx_syncpoint vk_SignalGPU(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint* globalSyncPoint)
{
    (void)syncobj; (void)globalSyncPoint;
    // TODO: Implement when needed
    rz_gfx_syncpoint result = {0};
    return result;
}

static void vk_FlushGPUWork(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint* globalSyncpoint)
{
    (void)syncobj; (void)globalSyncpoint;
    // TODO: Implement when needed
}

static void vk_ResizeSwapchain(rz_gfx_swapchain* sc, uint32_t width, uint32_t height)
{
    (void)sc; (void)width; (void)height;
    // TODO: Implement when needed
}

static void vk_BeginFrame(rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    (void)sc; (void)frameSyncobj; (void)frameSyncPoints; (void)globalSyncPoint;
    // TODO: Implement when needed
}

static void vk_EndFrame(const rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    (void)sc; (void)frameSyncobj; (void)frameSyncPoints; (void)globalSyncPoint;
    // TODO: Implement when needed
}

//---------------------------------------------------------------------------------------------
// Jump table
//---------------------------------------------------------------------------------------------

rz_rhi_api vk_rhi = {
    .GlobalCtxInit        = vk_GlobalCtxInit,           // GlobalCtxInit
    .GlobalCtxDestroy     = vk_GlobalCtxDestroy,        // GlobalCtxDestroy
    .CreateSyncobj        = vk_CreateSyncobj,           // CreateSyncobj
    .DestroySyncobj       = vk_DestroySyncobj,          // DestroySyncobj
    .CreateSwapchain      = vk_CreateSwapchain,         // CreateSwapchain
    .DestroySwapchain     = vk_DestroySwapchain,        // DestroySwapchain
    .CreateCmdPool        = vk_CreateCmdPool,           // CreateCmdPool
    .DestroyCmdPool       = vk_DestroyCmdPool,          // DestroyCmdPool
    .CreateCmdBuf         = vk_CreateCmdBuf,            // CreateCmdBuf
    .DestroyCmdBuf        = vk_DestroyCmdBuf,           // DestroyCmdBuf
    .CreateShader         = vk_CreateShader,            // CreateShader
    .DestroyShader        = vk_DestroyShader,           // DestroyShader
    .CreateRootSignature  = vk_CreateRootSignature,     // CreateRootSignature
    .DestroyRootSignature = vk_DestroyRootSignature,    // DestroyRootSignature
    .CreatePipeline       = vk_CreatePipeline,          // CreatePipeline
    .DestroyPipeline      = vk_DestroyPipeline,         // DestroyPipeline
    .CreateTexture        = vk_CreateTexture,           // CreateTexture
    .DestroyTexture       = vk_DestroyTexture,          // DestroyTexture
    .CreateSampler        = vk_CreateSampler,           // CreateSampler
    .DestroySampler       = vk_DestroySampler,          // DestroySampler
    .CreateDescriptorHeap = vk_CreateDescriptorHeap,    // CreateDescriptorHeap
    .DestroyDescriptorHeap = vk_DestroyDescriptorHeap,  // DestroyDescriptorHeap
    .CreateDescriptorTable = vk_CreateDescriptorTable,  // CreateDescriptorTable
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