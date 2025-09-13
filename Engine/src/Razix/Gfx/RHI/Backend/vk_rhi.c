#include "vk_rhi.h"

#include "Razix/Gfx/RHI/RHI.h"

#define VOLK_IMPLEMENTATION
#include <volk.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef RAZIX_PLATFORM_LINUX
    #include <alloca.h>    // for alloca
#endif

// Type friendly defines
#define VKCONTEXT g_GfxCtx.vk
#define VKDEVICE  g_GfxCtx.vk.device
#define VKGPU     g_GfxCtx.vk.gpu

#if defined RAZIX_DEBUG
    #define CHECK_VK(x)                    vk_util_check_result((x), __func__, __FILE__, __LINE__)
    #define TAG_OBJECT(obj, objType, name) vk_util_tag_object(obj, objType, name)
#else
    #define CHECK_VK(x) (x)
    #define TAG_OBJECT(obj, objType, name)
#endif

//---------------------------------------------------------------------------------------------
// Dynamic function loading macros and function pointers
//---------------------------------------------------------------------------------------------

// Macro for loading instance-level functions
#define VK_LOAD_INSTANCE_FUNCTION(instance, func_name)                                   \
    do {                                                                                 \
        pfn_##func_name = (PFN_##func_name) vkGetInstanceProcAddr(instance, #func_name); \
        if (!pfn_##func_name) {                                                          \
            RAZIX_RHI_LOG_WARN("Failed to load instance function: %s", #func_name);      \
        }                                                                                \
    } while (0)

// Macro for loading device-level functions
#define VK_LOAD_DEVICE_FUNCTION(device, func_name)                                   \
    do {                                                                             \
        pfn_##func_name = (PFN_##func_name) vkGetDeviceProcAddr(device, #func_name); \
        if (!pfn_##func_name) {                                                      \
            RAZIX_RHI_LOG_WARN("Failed to load device function: %s", #func_name);    \
        }                                                                            \
    } while (0)

// Macro for safely calling loaded functions
#define VK_CALL_LOADED_FUNCTION(func_name, ...) \
    (pfn_##func_name ? pfn_##func_name(__VA_ARGS__) : VK_ERROR_EXTENSION_NOT_PRESENT)

//---------------------------------------------------------------------------------------------
// Constants and configuration
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// INSTANCE EXTENSIONS - Vulkan 1.3 Compatible
//---------------------------------------------------------------------------------------------

#define VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME "VK_KHR_portability_enumeration"
#define VK_EXT_METAL_SURFACE_EXTENSION_NAME           "VK_EXT_metal_surface"

static const char* s_RequiredInstanceExtensions[] = {
    // Core surface support - required on all platforms
    VK_KHR_SURFACE_EXTENSION_NAME,

// Platform-specific surface extensions
#if defined(RAZIX_PLATFORM_WINDOWS)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif

#if defined(RAZIX_PLATFORM_LINUX)
    #if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
    #endif
    #if defined(VK_USE_PLATFORM_XCB_KHR)
    VK_KHR_XCB_SURFACE_EXTENSION_NAME,
    #endif
    #if defined(VK_USE_PLATFORM_XLIB_KHR)
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
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,    // Required dependency for VK_EXT_debug_marker
#endif
};

//---------------------------------------------------------------------------------------------
// DEVICE EXTENSIONS - Vulkan 1.3 Compatible
//---------------------------------------------------------------------------------------------

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

static const char* s_RequiredDeviceExtensions[] = {
    // Core presentation support
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,

    // Modern Vulkan 1.3 features (these are promoted to core but extensions still needed for compatibility)
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,    // Core in 1.3

    // Descriptor indexing for modern bindless rendering (core in 1.2)
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,

    // Timeline semaphores for better synchronization (core in 1.2)
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,

// macOS/MoltenVK compatibility
#ifdef RAZIX_PLATFORM_MACOS
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
#endif
};

static const char* s_OptionalDeviceExtensions[] = {
    // Improved synchronization
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,    // Core in 1.3

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

//---------------------------------------------------------------------------------------------
// VALIDATION LAYERS - Unified Debug Layer
//---------------------------------------------------------------------------------------------

#ifdef RAZIX_DEBUG
static const char* s_ValidationLayers[] = {
    // Unified validation layer
    "VK_LAYER_KHRONOS_validation",
};
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define REQUIRED_INSTANCE_EXT_COUNT ARRAY_SIZE(s_RequiredInstanceExtensions)
#define REQUIRED_DEVICE_EXT_COUNT   ARRAY_SIZE(s_RequiredDeviceExtensions)

#ifdef RAZIX_DEBUG
    #define VALIDATION_LAYER_COUNT ARRAY_SIZE(s_ValidationLayers)
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

static void vk_util_tag_object(void* object, VkObjectType objectType, const char* name)
{
    VkDebugUtilsObjectNameInfoEXT nameInfo = {0};
    nameInfo.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    nameInfo.pNext                         = NULL;
    nameInfo.objectType                    = objectType;
    nameInfo.objectHandle                  = (uint64_t) (uintptr_t) object;
    nameInfo.pObjectName                   = name;
    VkResult result                        = vkSetDebugUtilsObjectNameEXT(VKDEVICE, &nameInfo);
    if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_WARN("Failed to tag Vulkan object with name: %s", name);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_util_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       pUserData)
{
    (void) pUserData;

    // Enhanced color scheme with better contrast
    const char* severityColor  = ANSI_COLOR_RESET;
    const char* severityPrefix = "";
    const char* severityLabel  = "";
    const char* borderColor    = ANSI_COLOR_GRAY;

    // Message type indicators
    const char* typePrefix = "";
    const char* typeColor  = ANSI_COLOR_CYAN;

    // Determine severity styling
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            severityColor  = ANSI_COLOR_GRAY;
            severityPrefix = "[V]";
            severityLabel  = "VERBOSE";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severityColor  = ANSI_COLOR_CYAN;
            severityPrefix = "[I]";
            severityLabel  = "INFO";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severityColor  = ANSI_COLOR_YELLOW;
            severityPrefix = "[W]";
            severityLabel  = "WARNING";
            borderColor    = ANSI_COLOR_YELLOW;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severityColor  = ANSI_COLOR_RED;
            severityPrefix = "[E]";
            severityLabel  = "ERROR";
            borderColor    = ANSI_COLOR_RED;
            break;
        default:
            severityColor  = ANSI_COLOR_MAGENTA;
            severityPrefix = "[?]";
            severityLabel  = "UNKNOWN";
            break;
    }

    // Determine message type styling
    const char* typeLabel = "";
    switch (messageType) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            typePrefix = "[GEN]";
            typeColor  = ANSI_COLOR_WHITE;
            typeLabel  = "GENERAL";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            typePrefix = "[VAL]";
            typeColor  = ANSI_COLOR_GREEN;
            typeLabel  = "VALIDATION";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            typePrefix = "[PERF]";
            typeColor  = ANSI_COLOR_BLUE;
            typeLabel  = "PERFORMANCE";
            break;
        default:
            typePrefix = "[UNK]";
            typeColor  = ANSI_COLOR_CYAN;
            typeLabel  = "UNKNOWN";
            break;
    }

    // Create a beautiful ASCII border
    printf("%s+===================================================================+%s\n",
        borderColor,
        ANSI_COLOR_RESET);

    // Header with severity and type
    printf("%s|%s %s%s%s %-8s %s%s%s %-12s %s%15s%s %s|%s\n",
        borderColor,
        ANSI_COLOR_RESET,
        severityColor,
        severityPrefix,
        ANSI_COLOR_RESET,
        severityLabel,
        typeColor,
        typePrefix,
        ANSI_COLOR_RESET,
        typeLabel,
        ANSI_COLOR_GRAY,
        _rhi_log_timestamp(),
        ANSI_COLOR_RESET,
        borderColor,
        ANSI_COLOR_RESET);

    // Separator
    printf("%s+-------------------------------------------------------------------+%s\n",
        borderColor,
        ANSI_COLOR_RESET);

    // Message ID if available
    if (pCallbackData->pMessageIdName || pCallbackData->messageIdNumber != 0) {
        printf("%s|%s %sMSG ID:%s ",
            borderColor,
            ANSI_COLOR_RESET,
            ANSI_COLOR_BOLD,
            ANSI_COLOR_RESET);

        if (pCallbackData->pMessageIdName) {
            printf("%s%s%s", ANSI_COLOR_MAGENTA, pCallbackData->pMessageIdName, ANSI_COLOR_RESET);
        }
        if (pCallbackData->messageIdNumber != 0) {
            printf(" %s(#%d)%s", ANSI_COLOR_GRAY, pCallbackData->messageIdNumber, ANSI_COLOR_RESET);
        }

        // Calculate padding
        int usedSpace = 8;    // "MSG ID: "
        if (pCallbackData->pMessageIdName) {
            usedSpace += strlen(pCallbackData->pMessageIdName);
        }
        if (pCallbackData->messageIdNumber != 0) {
            usedSpace += snprintf(NULL, 0, " (#%d)", pCallbackData->messageIdNumber);
        }

        int padding = 57 - usedSpace;
        for (int i = 0; i < padding && i >= 0; i++) printf(" ");

        printf(" %s|%s\n", borderColor, ANSI_COLOR_RESET);
    }

    // Objects involved (prettified)
    if (pCallbackData->objectCount > 0) {
        printf("%s|%s %sOBJECTS INVOLVED:%s%48s %s|%s\n",
            borderColor,
            ANSI_COLOR_RESET,
            ANSI_COLOR_BOLD,
            ANSI_COLOR_RESET,
            "",
            borderColor,
            ANSI_COLOR_RESET);

        for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
            const VkDebugUtilsObjectNameInfoEXT* obj = &pCallbackData->pObjects[i];

            // Object type to readable string with prefix
            const char* objectTypeStr = "Unknown";
            const char* objectPrefix  = "[UNK]";

            switch (obj->objectType) {
                case VK_OBJECT_TYPE_DEVICE:
                    objectTypeStr = "Device";
                    objectPrefix  = "[DEV]";
                    break;
                case VK_OBJECT_TYPE_QUEUE:
                    objectTypeStr = "Queue";
                    objectPrefix  = "[QUE]";
                    break;
                case VK_OBJECT_TYPE_COMMAND_BUFFER:
                    objectTypeStr = "CommandBuffer";
                    objectPrefix  = "[CMD]";
                    break;
                case VK_OBJECT_TYPE_BUFFER:
                    objectTypeStr = "Buffer";
                    objectPrefix  = "[BUF]";
                    break;
                case VK_OBJECT_TYPE_IMAGE:
                    objectTypeStr = "Image";
                    objectPrefix  = "[IMG]";
                    break;
                case VK_OBJECT_TYPE_PIPELINE:
                    objectTypeStr = "Pipeline";
                    objectPrefix  = "[PIP]";
                    break;
                case VK_OBJECT_TYPE_DESCRIPTOR_SET:
                    objectTypeStr = "DescriptorSet";
                    objectPrefix  = "[DSC]";
                    break;
                case VK_OBJECT_TYPE_RENDER_PASS:
                    objectTypeStr = "RenderPass";
                    objectPrefix  = "[RPS]";
                    break;
                case VK_OBJECT_TYPE_FRAMEBUFFER:
                    objectTypeStr = "Framebuffer";
                    objectPrefix  = "[FBO]";
                    break;
                case VK_OBJECT_TYPE_SHADER_MODULE:
                    objectTypeStr = "ShaderModule";
                    objectPrefix  = "[SHD]";
                    break;
                case VK_OBJECT_TYPE_INSTANCE:
                    objectTypeStr = "Instance";
                    objectPrefix  = "[INS]";
                    break;
                case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
                    objectTypeStr = "PhysicalDevice";
                    objectPrefix  = "[PHY]";
                    break;
                default: break;
            }

            printf("%s|%s  %s%s%s %-15s",
                borderColor,
                ANSI_COLOR_RESET,
                ANSI_COLOR_CYAN,
                objectPrefix,
                ANSI_COLOR_RESET,
                objectTypeStr);

            if (obj->pObjectName && strlen(obj->pObjectName) > 0) {
                printf(" %s\"%s\"%s", ANSI_COLOR_GREEN, obj->pObjectName, ANSI_COLOR_RESET);

                // Calculate remaining space for handle
                int nameLen   = strlen(obj->pObjectName) + 2;    // quotes
                int prefixLen = 6;                               // [XXX]
                int typeLen   = strlen(objectTypeStr);
                int usedSpace = 2 + prefixLen + typeLen + 1 + nameLen;    // "  " + prefix + type + " " + name
                int remaining = 65 - usedSpace;

                if (remaining > 16) {    // Enough space for handle
                    int padding = remaining - 16;
                    for (int p = 0; p < padding; p++) printf(" ");
                    printf(" %s(0x%012llx)%s", ANSI_COLOR_GRAY, (unsigned long long) obj->objectHandle, ANSI_COLOR_RESET);
                } else {
                    // Not enough space, put handle on next line
                    int totalPadding = 65 - usedSpace;
                    for (int p = 0; p < totalPadding; p++) printf(" ");
                }
            } else {
                printf(" %s<unnamed>%s", ANSI_COLOR_GRAY, ANSI_COLOR_RESET);

                // Calculate padding for handle
                int usedSpace = 2 + 6 + strlen(objectTypeStr) + 1 + 9;    // "  " + prefix + type + " " + "<unnamed>"
                int remaining = 65 - usedSpace;

                if (remaining > 16) {
                    int padding = remaining - 16;
                    for (int p = 0; p < padding; p++) printf(" ");
                    printf(" %s(0x%012llx)%s", ANSI_COLOR_GRAY, (unsigned long long) obj->objectHandle, ANSI_COLOR_RESET);
                } else {
                    int totalPadding = 65 - usedSpace;
                    for (int p = 0; p < totalPadding; p++) printf(" ");
                }
            }

            printf(" %s|%s\n", borderColor, ANSI_COLOR_RESET);
        }
    }

    // Queue labels if available
    if (pCallbackData->queueLabelCount > 0) {
        printf("%s|%s %sQUEUE LABELS:%s%52s %s|%s\n",
            borderColor,
            ANSI_COLOR_RESET,
            ANSI_COLOR_BOLD,
            ANSI_COLOR_RESET,
            "",
            borderColor,
            ANSI_COLOR_RESET);

        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++) {
            printf("%s|%s  >> %s%s%s",
                borderColor,
                ANSI_COLOR_RESET,
                ANSI_COLOR_YELLOW,
                pCallbackData->pQueueLabels[i].pLabelName,
                ANSI_COLOR_RESET);

            int usedSpace = 5 + strlen(pCallbackData->pQueueLabels[i].pLabelName);
            int padding   = 66 - usedSpace;
            for (int p = 0; p < padding; p++) printf(" ");

            printf("%s|%s\n", borderColor, ANSI_COLOR_RESET);
        }
    }

    // Command buffer labels if available
    if (pCallbackData->cmdBufLabelCount > 0) {
        printf("%s|%s %sCMD BUFFER LABELS:%s%48s %s|%s\n",
            borderColor,
            ANSI_COLOR_RESET,
            ANSI_COLOR_BOLD,
            ANSI_COLOR_RESET,
            "",
            borderColor,
            ANSI_COLOR_RESET);

        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
            printf("%s|%s  -> %s%s%s",
                borderColor,
                ANSI_COLOR_RESET,
                ANSI_COLOR_BLUE,
                pCallbackData->pCmdBufLabels[i].pLabelName,
                ANSI_COLOR_RESET);

            int usedSpace = 5 + strlen(pCallbackData->pCmdBufLabels[i].pLabelName);
            int padding   = 66 - usedSpace;
            for (int p = 0; p < padding; p++) printf(" ");

            printf("%s|%s\n", borderColor, ANSI_COLOR_RESET);
        }
    }

    // Message separator
    printf("%s+-------------------------------------------------------------------+%s\n",
        borderColor,
        ANSI_COLOR_RESET);

    // Main message (word-wrapped and formatted)
    printf("%s|%s %sMESSAGE:%s%58s %s|%s\n",
        borderColor,
        ANSI_COLOR_RESET,
        ANSI_COLOR_BOLD,
        ANSI_COLOR_RESET,
        "",
        borderColor,
        ANSI_COLOR_RESET);

    // Word wrap the message to fit nicely in the box
    const char* message       = pCallbackData->pMessage;
    const int   maxLineLength = 63;
    int         messageLen    = strlen(message);

    for (int i = 0; i < messageLen;) {
        int lineEnd = i + maxLineLength;
        if (lineEnd >= messageLen) {
            lineEnd = messageLen;
        } else {
            // Find last space to avoid breaking words
            while (lineEnd > i && message[lineEnd] != ' ' && message[lineEnd] != '\n') {
                lineEnd--;
            }
            if (lineEnd == i) lineEnd = i + maxLineLength;    // Force break if no space found
        }

        printf("%s|%s %.*s", borderColor, ANSI_COLOR_RESET, lineEnd - i, &message[i]);

        // Pad the line to maintain box alignment
        int padding = maxLineLength - (lineEnd - i);
        for (int p = 0; p < padding; p++) printf(" ");

        printf(" %s|%s\n", borderColor, ANSI_COLOR_RESET);

        i = lineEnd;
        while (i < messageLen && (message[i] == ' ' || message[i] == '\n')) i++;    // Skip spaces and newlines
    }

    // Bottom border
    printf("%s+===================================================================+%s\n",
        borderColor,
        ANSI_COLOR_RESET);

    // Add some spacing for readability
    printf("\n");

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

static VkImageLayout vk_util_translate_imagelayout_resstate(rz_gfx_resource_state state)
{
    if (state >= RZ_GFX_RESOURCE_STATE_COUNT || state == RZ_GFX_RESOURCE_STATE_UNDEFINED) {
        RAZIX_RHI_LOG_ERROR("Invalid resource state %d", state);
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }

    return vulkan_image_layout_map[state];
}

static void vk_util_print_device_info(VkPhysicalDevice physDev)
{
    if (!physDev) {
        RAZIX_RHI_LOG_INFO("[VULKAN] Invalid VkPhysicalDevice");
        return;
    }

    VkPhysicalDeviceProperties       props;
    VkPhysicalDeviceMemoryProperties memProps;
    VkPhysicalDeviceFeatures         feats;

    memset(&props, 0, sizeof(props));
    memset(&memProps, 0, sizeof(memProps));
    memset(&feats, 0, sizeof(feats));

    vkGetPhysicalDeviceProperties(physDev, &props);
    vkGetPhysicalDeviceMemoryProperties(physDev, &memProps);
    vkGetPhysicalDeviceFeatures(physDev, &feats);

    /* Device type string */
    const char* deviceType = "Unknown";
    switch (props.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: deviceType = "Discrete GPU"; break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: deviceType = "Integrated GPU"; break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: deviceType = "Virtual GPU"; break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU: deviceType = "CPU"; break;
        default: break;
    }

    /* UUID */
    char uuidStr[37];
    (void) snprintf(uuidStr, sizeof(uuidStr), "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", props.pipelineCacheUUID[0], props.pipelineCacheUUID[1], props.pipelineCacheUUID[2], props.pipelineCacheUUID[3], props.pipelineCacheUUID[4], props.pipelineCacheUUID[5], props.pipelineCacheUUID[6], props.pipelineCacheUUID[7], props.pipelineCacheUUID[8], props.pipelineCacheUUID[9], props.pipelineCacheUUID[10], props.pipelineCacheUUID[11], props.pipelineCacheUUID[12], props.pipelineCacheUUID[13], props.pipelineCacheUUID[14], props.pipelineCacheUUID[15]);

    /* Head Banner */
    RAZIX_RHI_LOG_INFO("+======================================================================+");
    RAZIX_RHI_LOG_INFO("|                           VULKAN GPU INFO                            |");
    RAZIX_RHI_LOG_INFO("+======================================================================+");

    /* Identity */
    RAZIX_RHI_LOG_INFO("| API Version        : %u.%u.%u",
        (unsigned) VK_VERSION_MAJOR(props.apiVersion),
        (unsigned) VK_VERSION_MINOR(props.apiVersion),
        (unsigned) VK_VERSION_PATCH(props.apiVersion));
    RAZIX_RHI_LOG_INFO("| Driver Version     : %u.%u.%u",
        (unsigned) VK_VERSION_MAJOR(props.driverVersion),
        (unsigned) VK_VERSION_MINOR(props.driverVersion),
        (unsigned) VK_VERSION_PATCH(props.driverVersion));
    RAZIX_RHI_LOG_INFO("| Device Name        : %s", props.deviceName);
    RAZIX_RHI_LOG_INFO("| Vendor             : %s (0x%04X)", rzRHI_GetGPUVendorName(props.vendorID), props.vendorID);
    RAZIX_RHI_LOG_INFO("| Device ID          : 0x%04X", props.deviceID);
    RAZIX_RHI_LOG_INFO("| Device Type        : %s", deviceType);
    RAZIX_RHI_LOG_INFO("| UUID               : %s", uuidStr);
    RAZIX_RHI_LOG_INFO("| LUID               : N/A");
    RAZIX_RHI_LOG_INFO("| SubSys ID          : N/A");
    RAZIX_RHI_LOG_INFO("| Revision           : N/A");
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| Memory Heaps (raw bytes)                                            |");
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    {
        uint32_t i;
        for (i = 0; i < memProps.memoryHeapCount; ++i) {
            const VkMemoryHeap* h = &memProps.memoryHeaps[i];
            RAZIX_RHI_LOG_INFO("| Heap %-2u  flags=0x%08X size=%llu",
                (unsigned) i,
                (unsigned) h->flags,
                (unsigned long long) h->size);
        }
    }
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| Limits (subset)                                                     |");
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| maxImageDimension2D      : %u", props.limits.maxImageDimension2D);
    RAZIX_RHI_LOG_INFO("| maxImageDimension3D      : %u", props.limits.maxImageDimension3D);
    RAZIX_RHI_LOG_INFO("| maxUniformBufferRange    : %u", props.limits.maxUniformBufferRange);
    RAZIX_RHI_LOG_INFO("| maxStorageBufferRange    : %llu", (unsigned long long) props.limits.maxStorageBufferRange);
    RAZIX_RHI_LOG_INFO("| maxPushConstantsSize     : %u", props.limits.maxPushConstantsSize);
    RAZIX_RHI_LOG_INFO("| maxBoundDescriptorSets   : %u", props.limits.maxBoundDescriptorSets);
    RAZIX_RHI_LOG_INFO("| maxViewports             : %u", props.limits.maxViewports);
    RAZIX_RHI_LOG_INFO("| maxComputeWorkGroupSize  : [%u, %u, %u]",
        props.limits.maxComputeWorkGroupSize[0],
        props.limits.maxComputeWorkGroupSize[1],
        props.limits.maxComputeWorkGroupSize[2]);

    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| Features (1 = supported, 0 = not)                                  |");
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| geometryShader           : %u", feats.geometryShader);
    RAZIX_RHI_LOG_INFO("| tessellationShader       : %u", feats.tessellationShader);
    RAZIX_RHI_LOG_INFO("| multiViewport            : %u", feats.multiViewport);
    RAZIX_RHI_LOG_INFO("| samplerAnisotropy        : %u", feats.samplerAnisotropy);
    RAZIX_RHI_LOG_INFO("| shaderFloat64            : %u", feats.shaderFloat64);
    RAZIX_RHI_LOG_INFO("| shaderInt64              : %u", feats.shaderInt64);
    RAZIX_RHI_LOG_INFO("| robustBufferAccess       : %u", feats.robustBufferAccess);
    RAZIX_RHI_LOG_INFO("+======================================================================+");
}

//---------------------------------------------------------------------------------------------

static bool vk_util_check_validation_layer_support(void)
{
#ifdef RAZIX_DEBUG
    uint32_t layerCount;
    CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, NULL));

    VkLayerProperties* availableLayers = malloc(layerCount * sizeof(VkLayerProperties));
    CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers));

    for (uint32_t i = 0; i < sizeof(s_ValidationLayers) / sizeof(s_ValidationLayers[0]); i++) {
        bool layerFound = false;

        for (uint32_t j = 0; j < layerCount; j++) {
            if (strcmp(s_ValidationLayers[i], availableLayers[j].layerName) == 0) {
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

static bool vk_util_check_instance_extension_support(void)
{
    uint32_t extensionCount = 0;
    CHECK_VK(vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL));

    VkExtensionProperties* availableExtensions = malloc(extensionCount * sizeof(VkExtensionProperties));
    CHECK_VK(vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, availableExtensions));

    for (uint32_t i = 0; i < sizeof(s_RequiredInstanceExtensions) / sizeof(s_RequiredInstanceExtensions[0]); i++) {
        bool extensionFound = false;

        for (uint32_t j = 0; j < extensionCount; j++) {
            if (strcmp(s_RequiredInstanceExtensions[i], availableExtensions[j].extensionName) == 0) {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound) {
            RAZIX_RHI_LOG_ERROR("Required instance extension not found: %s", s_RequiredInstanceExtensions[i]);
            free(availableExtensions);
            return false;
        }
    }

    free(availableExtensions);
    return true;
}

static VkQueueFamilyIndices vk_util_find_queue_families(VkPhysicalDevice device)
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

static bool vk_util_check_device_extension_support(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    CHECK_VK(vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL));

    VkExtensionProperties* availableExtensions = malloc(extensionCount * sizeof(VkExtensionProperties));
    CHECK_VK(vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions));

    for (uint32_t i = 0; i < sizeof(s_RequiredDeviceExtensions) / sizeof(s_RequiredDeviceExtensions[0]); i++) {
        bool extensionFound = false;

        for (uint32_t j = 0; j < extensionCount; j++) {
            if (strcmp(s_RequiredDeviceExtensions[i], availableExtensions[j].extensionName) == 0) {
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

static int vk_util_log_and_check_device_extensions(VkPhysicalDevice device, const char* deviceName)
{
    uint32_t extensionCount = 0;
    CHECK_VK(vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL));

    VkExtensionProperties* extensions = NULL;
    if (extensionCount > 0) {
        extensions = (VkExtensionProperties*) malloc(extensionCount * sizeof(VkExtensionProperties));
        if (!extensions) {
            RAZIX_RHI_LOG_ERROR("Vulkan: Memory allocation failed while enumerating extensions.");
            return 0;
        }
        CHECK_VK(vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, extensions));
    }

    RAZIX_RHI_LOG_INFO("  - Available Extensions (%u):", (unsigned) extensionCount);
    if (extensionCount == 0) {
        RAZIX_RHI_LOG_INFO("    (none)");
    } else {
        uint32_t i;
        for (i = 0; i < extensionCount; ++i) {
            RAZIX_RHI_LOG_INFO("    * %s : version %u",
                extensions[i].extensionName,
                extensions[i].specVersion);
        }
    }

    size_t requiredCount = REQUIRED_DEVICE_EXT_COUNT;
    RAZIX_RHI_LOG_INFO("  - Required Extensions (%zu):", requiredCount);
    if (requiredCount == 0) {
        RAZIX_RHI_LOG_INFO("    (none required)");
    } else {
        size_t i;
        for (i = 0; i < requiredCount; ++i)
            RAZIX_RHI_LOG_INFO("    > %s", s_RequiredDeviceExtensions[i]);
    }

    // Check missing
    size_t missingCount = 0;
    if (requiredCount > 0) {
        size_t i;
        for (i = 0; i < requiredCount; ++i) {
            const char* needed = s_RequiredDeviceExtensions[i];
            int         found  = 0;
            uint32_t    j;
            for (j = 0; j < extensionCount; ++j) {
                if (strcmp(needed, extensions[j].extensionName) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (missingCount == 0)
                    RAZIX_RHI_LOG_ERROR("  - Missing Required Extensions:");
                RAZIX_RHI_LOG_ERROR("    ! %s", needed);
                missingCount++;
            }
        }
    }

    if (extensions) free(extensions);

    if (missingCount == 0) {
        RAZIX_RHI_LOG_INFO("  - Extension Status: ALL REQUIRED PRESENT");
        return 1;
    } else {
        RAZIX_RHI_LOG_ERROR("  - Extension Status: MISSING %zu REQUIRED EXTENSION(S)", missingCount);
        return 0;
    }
}

static int vk_util_is_device_suitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceFeatures   feats;
    vkGetPhysicalDeviceProperties(device, &props);
    vkGetPhysicalDeviceFeatures(device, &feats);

    // Queue families
    VkQueueFamilyIndices indices = vk_util_find_queue_families(device);
    if (!indices.hasGraphics) {
        RAZIX_RHI_LOG_INFO("  - Rejected: No graphics queue family.");
        return 0;
    }

    // Optional feature gates can be placed here:
    // if (!feats.samplerAnisotropy) { RAZIX_RHI_LOG_INFO("  - Rejected: Missing samplerAnisotropy"); return 0; }

    // Extensions (detailed logging)
    if (!vk_util_log_and_check_device_extensions(device, props.deviceName))
        return 0;

    return 1;
}

static uint64_t vk_util_get_device_local_vram(VkPhysicalDevice device)
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(device, &memProps);

    uint64_t total = 0;
    uint32_t i;
    for (i = 0; i < memProps.memoryHeapCount; ++i) {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            total += memProps.memoryHeaps[i].size;
    }
    return total;
}

static int vk_util_score_device(VkPhysicalDevice device, const VkPhysicalDeviceProperties* props, uint64_t vramBytes)
{
    int score = 0;

    // Favor discrete GPU strongly
    if (props->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 100000;

    // Add VRAM in MB
    score += (int) (vramBytes / (1024ull * 1024ull));

    // Penalize virtual / CPU
    if (props->deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
        score -= 1000;
    else if (props->deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
        score -= 5000;

    return score;
}

static VkPhysicalDevice vk_util_pick_physical_device(void)
{
    uint32_t deviceCount = 0;
    CHECK_VK(vkEnumeratePhysicalDevices(VKCONTEXT.instance, &deviceCount, NULL));

    if (deviceCount == 0) {
        RAZIX_RHI_LOG_ERROR("Vulkan: No physical devices supporting Vulkan were found.");
        return VK_NULL_HANDLE;
    }

    VkPhysicalDevice* devices = (VkPhysicalDevice*) malloc(deviceCount * sizeof(VkPhysicalDevice));
    if (!devices) {
        RAZIX_RHI_LOG_ERROR("Vulkan: Memory allocation failure retrieving devices.");
        return VK_NULL_HANDLE;
    }
    CHECK_VK(vkEnumeratePhysicalDevices(VKCONTEXT.instance, &deviceCount, devices));

    RAZIX_RHI_LOG_INFO("Vulkan: Evaluating %u physical device(s)...", deviceCount);

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    int              bestScore  = -2147483647;

    uint32_t i;
    for (i = 0; i < deviceCount; ++i) {
        VkPhysicalDevice           device = devices[i];
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

        const char* typeStr = "Unknown";
        switch (props.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: typeStr = "Integrated"; break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: typeStr = "Discrete"; break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: typeStr = "Virtual"; break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU: typeStr = "CPU"; break;
            default: break;
        }

        uint64_t vram = vk_util_get_device_local_vram(device);

        RAZIX_RHI_LOG_INFO("----------------------------------------------------------------");
        RAZIX_RHI_LOG_INFO("  - API Version  : %u.%u.%u",
            (unsigned) VK_VERSION_MAJOR(props.apiVersion),
            (unsigned) VK_VERSION_MINOR(props.apiVersion),
            (unsigned) VK_VERSION_PATCH(props.apiVersion));
        RAZIX_RHI_LOG_INFO("  - Driver Vers. : %u.%u.%u",
            (unsigned) VK_VERSION_MAJOR(props.driverVersion),
            (unsigned) VK_VERSION_MINOR(props.driverVersion),
            (unsigned) VK_VERSION_PATCH(props.driverVersion));

        if (!vk_util_is_device_suitable(device)) {
            RAZIX_RHI_LOG_INFO("  --> Result: NOT SUITABLE");
            continue;
        }

        int score = vk_util_score_device(device, &props, vram);
        RAZIX_RHI_LOG_INFO("  - Score        : %d", score);
        RAZIX_RHI_LOG_INFO("  --> Result: SUITABLE");

        if (score > bestScore) {
            bestScore  = score;
            bestDevice = device;
        }
    }

    RAZIX_RHI_LOG_INFO("----------------------------------------------------------------");
    free(devices);

    if (bestDevice == VK_NULL_HANDLE) {
        RAZIX_RHI_LOG_ERROR("Vulkan: No suitable GPU found (after evaluating all candidates).");
    } else {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(bestDevice, &props);
        uint64_t vram = vk_util_get_device_local_vram(bestDevice);
    }

    return bestDevice;
}

static void vk_util_create_logical_device(void)
{
    VkQueueFamilyIndices indices = vk_util_find_queue_families(VKCONTEXT.gpu);
    VKCONTEXT.queueFamilyIndices = indices;

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
    createInfo.enabledExtensionCount   = sizeof(s_RequiredDeviceExtensions) / sizeof(s_RequiredDeviceExtensions[0]);
    createInfo.ppEnabledExtensionNames = s_RequiredDeviceExtensions;

#ifdef RAZIX_DEBUG
    createInfo.enabledLayerCount   = sizeof(s_ValidationLayers) / sizeof(s_ValidationLayers[0]);
    createInfo.ppEnabledLayerNames = s_ValidationLayers;
#else
    createInfo.enabledLayerCount = 0;
#endif

    CHECK_VK(vkCreateDevice(VKCONTEXT.gpu, &createInfo, NULL, &VKCONTEXT.device));

    vkGetDeviceQueue(VKCONTEXT.device, indices.graphicsFamily, 0, &VKCONTEXT.graphicsQueue);
    vkGetDeviceQueue(VKCONTEXT.device, indices.presentFamily, 0, &VKCONTEXT.presentQueue);
}

static VkExtent2D vk_util_choose_swap_extent(const VkSurfaceCapabilitiesKHR* capabilities, uint32_t width, uint32_t height)
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

static VkSwapchainSupportDetails vk_util_query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
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

static VkSurfaceFormatKHR vk_util_choose_swap_surface_format(const VkSurfaceFormatKHR* availableFormats, uint32_t formatCount)
{
    for (uint32_t i = 0; i < formatCount; i++) {
        if (availableFormats[i].format == RAZIX_SWAPCHAIN_FORMAT_VK &&
            availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

static VkPresentModeKHR vk_util_choose_swap_present_mode(const VkPresentModeKHR* availablePresentModes, uint32_t presentModeCount)
{
    for (uint32_t i = 0; i < presentModeCount; i++) {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentModes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

//---------------------------------------------------------------------------------------------
// Swapchain Image Utility Functions
//---------------------------------------------------------------------------------------------

static void vk_util_create_swapchain_images(rz_gfx_swapchain* swapchain)
{
    RAZIX_RHI_ASSERT(swapchain != NULL, "Swapchain cannot be NULL");
    RAZIX_RHI_ASSERT(swapchain->vk.swapchain != VK_NULL_HANDLE, "Vulkan swapchain must be valid");

    CHECK_VK(vkGetSwapchainImagesKHR(VKCONTEXT.device, swapchain->vk.swapchain, &swapchain->imageCount, NULL));
    swapchain->vk.images = malloc(swapchain->imageCount * sizeof(VkImage));
    CHECK_VK(vkGetSwapchainImagesKHR(VKCONTEXT.device, swapchain->vk.swapchain, &swapchain->imageCount, swapchain->vk.images));

    swapchain->vk.imageViews = malloc(swapchain->imageCount * sizeof(VkImageView));
    RAZIX_RHI_ASSERT(swapchain->vk.imageViews != NULL, "Failed to allocate memory for image views");

    for (uint32_t i = 0; i < swapchain->imageCount; i++) {
        VkImage image = swapchain->vk.images[i];
        TAG_OBJECT(image, VK_OBJECT_TYPE_IMAGE, "Swapchain Image");

        VkImageViewCreateInfo createInfo           = {0};
        createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image                           = swapchain->vk.images[i];
        createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format                          = swapchain->vk.imageFormat;
        createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel   = 0;
        createInfo.subresourceRange.levelCount     = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount     = 1;

        VkResult result = vkCreateImageView(VKCONTEXT.device, &createInfo, NULL, &swapchain->vk.imageViews[i]);
        if (result != VK_SUCCESS) {
            RAZIX_RHI_LOG_ERROR("Failed to create image view for swapchain image %u: %d", i, result);
            // Clean up previously created image views
            for (uint32_t j = 0; j < i; j++) {
                vkDestroyImageView(VKCONTEXT.device, swapchain->vk.imageViews[j], NULL);
            }
            free(swapchain->vk.imageViews);
            swapchain->vk.imageViews = NULL;
            return;
        }
        TAG_OBJECT(swapchain->vk.imageViews[i], VK_OBJECT_TYPE_IMAGE_VIEW, "Swapchain Image View");

        RAZIX_RHI_LOG_TRACE("Created image view %u for swapchain image", i);
    }

    RAZIX_RHI_LOG_INFO("Created %u image views for swapchain images", swapchain->imageCount);
}

static void vk_util_create_swapchain_textures(rz_gfx_swapchain* swapchain)
{
    RAZIX_RHI_ASSERT(swapchain != NULL, "Swapchain cannot be NULL");
    RAZIX_RHI_ASSERT(swapchain->vk.images != NULL, "Swapchain images must be retrieved first");
    RAZIX_RHI_ASSERT(swapchain->vk.imageViews != NULL, "Image views must be created first");

    // Create rz_gfx_texture objects for each swapchain image
    for (uint32_t i = 0; i < swapchain->imageCount; i++) {
        // Create texture wrapper for swapchain image
        rz_gfx_texture* texture = &swapchain->backbuffers[i];
        memset(texture, 0, sizeof(rz_gfx_texture));

        // Set up resource metadata
        texture->resource.pName                          = "$SWAPCHAIN_IMAGE$";
        texture->resource.handle                         = (rz_handle) {i, i};
        texture->resource.viewHints                      = RZ_GFX_RESOURCE_VIEW_FLAG_RTV;
        texture->resource.type                           = RZ_GFX_RESOURCE_TYPE_TEXTURE;
        texture->resource.currentState                   = RZ_GFX_RESOURCE_STATE_PRESENT;
        texture->resource.desc.textureDesc.width         = swapchain->width;
        texture->resource.desc.textureDesc.height        = swapchain->height;
        texture->resource.desc.textureDesc.depth         = 1;
        texture->resource.desc.textureDesc.arraySize     = 1;
        texture->resource.desc.textureDesc.mipLevels     = 1;
        texture->resource.desc.textureDesc.format        = RAZIX_SWAPCHAIN_FORMAT;
        texture->resource.desc.textureDesc.textureType   = RZ_GFX_TEXTURE_TYPE_2D;
        texture->resource.desc.textureDesc.resourceHints = RZ_GFX_RESOURCE_VIEW_FLAG_RTV;
        texture->resource.desc.textureDesc.pPixelData    = NULL;
        texture->vk.image                                = swapchain->vk.images[i];
        // Note: We don't set memory handle since swapchain images are managed by the swapchain
        // Create resource view for the swapchain image
        rz_gfx_resource_view* resourceView = &swapchain->backbuffersResViews[i];
        memset(resourceView, 0, sizeof(rz_gfx_resource_view));
        resourceView->resource.pName                                                = "$SWAPCHAIN_RES_VIEW$";
        resourceView->resource.handle                                               = (rz_handle) {i, i};
        resourceView->resource.type                                                 = RZ_GFX_RESOURCE_TYPE_RESOURCE_VIEW;
        resourceView->resource.desc.resourceViewDesc.descriptorType                 = RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE;
        resourceView->resource.desc.resourceViewDesc.textureViewDesc.pTexture       = texture;
        resourceView->resource.desc.resourceViewDesc.textureViewDesc.baseMip        = 0;
        resourceView->resource.desc.resourceViewDesc.textureViewDesc.baseArrayLayer = 0;
        resourceView->resource.desc.resourceViewDesc.textureViewDesc.dimension      = RAZIX_RESOURCE_VIEW_DIMENSION_FULL;
        resourceView->vk.imageView                                                  = swapchain->vk.imageViews[i];

        RAZIX_RHI_LOG_TRACE("Created texture and resource view wrapper for swapchain image %u", i);
    }

    RAZIX_RHI_LOG_INFO("Created %u texture wrappers for swapchain images", swapchain->imageCount);
}

static void vk_util_destroy_swapchain_images(rz_gfx_swapchain* swapchain)
{
    RAZIX_RHI_ASSERT(swapchain != NULL, "Swapchain cannot be NULL");

    // Clean up image views
    if (swapchain->vk.imageViews) {
        for (uint32_t i = 0; i < swapchain->imageCount; i++) {
            if (swapchain->vk.imageViews[i] != VK_NULL_HANDLE) {
                vkDestroyImageView(VKCONTEXT.device, swapchain->vk.imageViews[i], NULL);
                swapchain->vk.imageViews[i] = VK_NULL_HANDLE;
            }
        }
        free(swapchain->vk.imageViews);
        swapchain->vk.imageViews = NULL;
    }

    // Clear texture wrappers (they don't own the actual VkImage, so just zero them out)
    for (uint32_t i = 0; i < swapchain->imageCount; i++) {
        memset(&swapchain->backbuffers[i], 0, sizeof(rz_gfx_texture));
        memset(&swapchain->backbuffersResViews[i], 0, sizeof(rz_gfx_resource_view));
    }

    RAZIX_RHI_LOG_INFO("Destroyed swapchain image views and texture wrappers");
}

static void vk_util_recreate_swapchain_images(rz_gfx_swapchain* swapchain)
{
    RAZIX_RHI_ASSERT(swapchain != NULL, "Swapchain cannot be NULL");
    RAZIX_RHI_ASSERT(swapchain->vk.swapchain != VK_NULL_HANDLE, "Vulkan swapchain must be valid");

    vk_util_destroy_swapchain_images(swapchain);

    if (swapchain->vk.images) {
        free(swapchain->vk.images);
    }

    // Recreate image views and texture wrappers
    vk_util_create_swapchain_images(swapchain);
    vk_util_create_swapchain_textures(swapchain);

    CHECK_VK(vkGetSwapchainImagesKHR(VKCONTEXT.device, swapchain->vk.swapchain, &swapchain->imageCount, NULL));
    RAZIX_RHI_LOG_INFO("Recreated swapchain images for new dimensions: %ux%u, %u images",
        swapchain->width,
        swapchain->height,
        swapchain->imageCount);
}

static VkFilter vk_util_translate_filter_type(rz_gfx_texture_filter_type filter)
{
    switch (filter) {
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST:
            return VK_FILTER_NEAREST;
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR:
            return VK_FILTER_LINEAR;
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_NEAREST:
            return VK_FILTER_NEAREST;
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_NEAREST:
            return VK_FILTER_LINEAR;
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_LINEAR:
            return VK_FILTER_NEAREST;
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR:
            return VK_FILTER_LINEAR;
        default:
            RAZIX_RHI_LOG_WARN("Unknown RZ_GFX_FILTER value, defaulting to VK_FILTER_LINEAR");
            return VK_FILTER_LINEAR;
    }
}

static VkSamplerMipmapMode vk_util_translate_mipmap_filter_type(rz_gfx_texture_filter_type filter)
{
    switch (filter) {
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST:
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR:
            // These don't use mipmapping, but we need to return something
            // You might want to return VK_SAMPLER_MIPMAP_MODE_NEAREST as default
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;

        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_NEAREST:
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_NEAREST:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;

        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_LINEAR:
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;

        default:
            RAZIX_RHI_LOG_WARN("Unknown RZ_GFX_FILTER value, defaulting to VK_SAMPLER_MIPMAP_MODE_NEAREST");
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    }
}

static bool vk_util_is_mipmap_enabled_from_filter_type(rz_gfx_texture_filter_type filter)
{
    switch (filter) {
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST:
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR:
            return false;

        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_NEAREST:
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_NEAREST:
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_LINEAR:
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR:
            return true;

        default:
            return false;
    }
}

static VkSamplerAddressMode vk_util_translate_address_mode(rz_gfx_texture_address_mode address_mode)
{
    switch (address_mode) {
        case RZ_GFX_TEXTURE_ADDRESS_MODE_WRAP:
        case RZ_GFX_TEXTURE_ADDRESS_MODE_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;

        case RZ_GFX_TEXTURE_ADDRESS_MODE_CLAMP:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

        case RZ_GFX_TEXTURE_ADDRESS_MODE_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

        default:
            RAZIX_RHI_LOG_WARN("Unknown RZ_GFX_TEXTURE_ADDRESS_MODE value, defaulting to VK_SAMPLER_ADDRESS_MODE_REPEAT");
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

static VkCompareOp vk_util_translate_compare_op(rz_gfx_compare_op_type compare_op)
{
    switch (compare_op) {
        case RZ_GFX_COMPARE_OP_TYPE_NEVER:
            return VK_COMPARE_OP_NEVER;

        case RZ_GFX_COMPARE_OP_TYPE_LESS:
            return VK_COMPARE_OP_LESS;

        case RZ_GFX_COMPARE_OP_TYPE_EQUAL:
            return VK_COMPARE_OP_EQUAL;

        case RZ_GFX_COMPARE_OP_TYPE_LESS_OR_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;

        case RZ_GFX_COMPARE_OP_TYPE_GREATER:
            return VK_COMPARE_OP_GREATER;

        case RZ_GFX_COMPARE_OP_TYPE_NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;

        case RZ_GFX_COMPARE_OP_TYPE_GREATER_OR_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;

        case RZ_GFX_COMPARE_OP_TYPE_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;

        default:
            RAZIX_RHI_LOG_WARN("Unknown RZ_GFX_COMPARE_OP_TYPE value, defaulting to VK_COMPARE_OP_LESS");
            return VK_COMPARE_OP_LESS;
    }
}

VkFormat vk_util_translate_format(rz_gfx_format format)
{
    switch (format) {
        // Undefined
        case RZ_GFX_FORMAT_UNDEFINED:
            return VK_FORMAT_UNDEFINED;

        // 8-bit formats
        case RZ_GFX_FORMAT_R8_UNORM:
            return VK_FORMAT_R8_UNORM;
        case RZ_GFX_FORMAT_R8_UINT:
            return VK_FORMAT_R8_UINT;
        case RZ_GFX_FORMAT_R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;
        case RZ_GFX_FORMAT_R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case RZ_GFX_FORMAT_R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case RZ_GFX_FORMAT_B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case RZ_GFX_FORMAT_B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;

        // 16-bit formats
        case RZ_GFX_FORMAT_R16_UNORM:
            return VK_FORMAT_R16_UNORM;
        case RZ_GFX_FORMAT_R16_FLOAT:
            return VK_FORMAT_R16_SFLOAT;
        case RZ_GFX_FORMAT_R16G16_UNORM:
            return VK_FORMAT_R16G16_UNORM;
        case RZ_GFX_FORMAT_R16G16_FLOAT:
            return VK_FORMAT_R16G16_SFLOAT;
        case RZ_GFX_FORMAT_R16G16B16A16_UNORM:
            return VK_FORMAT_R16G16B16A16_UNORM;
        case RZ_GFX_FORMAT_R16G16B16A16_FLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;

        // 32-bit signed integer formats
        case RZ_GFX_FORMAT_R32_SINT:
            return VK_FORMAT_R32_SINT;
        case RZ_GFX_FORMAT_R32G32_SINT:
            return VK_FORMAT_R32G32_SINT;
        case RZ_GFX_FORMAT_R32G32B32_SINT:
            return VK_FORMAT_R32G32B32_SINT;
        case RZ_GFX_FORMAT_R32G32B32A32_SINT:
            return VK_FORMAT_R32G32B32A32_SINT;

        // 32-bit unsigned integer formats
        case RZ_GFX_FORMAT_R32_UINT:
            return VK_FORMAT_R32_UINT;
        case RZ_GFX_FORMAT_R32G32_UINT:
            return VK_FORMAT_R32G32_UINT;
        case RZ_GFX_FORMAT_R32G32B32_UINT:
            return VK_FORMAT_R32G32B32_UINT;
        case RZ_GFX_FORMAT_R32G32B32A32_UINT:
            return VK_FORMAT_R32G32B32A32_UINT;

        // 32-bit float formats
        case RZ_GFX_FORMAT_R32_FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case RZ_GFX_FORMAT_R32G32_FLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
        case RZ_GFX_FORMAT_R32G32B32_FLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case RZ_GFX_FORMAT_R32G32B32A32_FLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

#ifdef RAZIX_SUPPORT_64BIT_FORMATS
        // 64-bit unsigned integer formats
        case RZ_GFX_FORMAT_R64_UINT:
            return VK_FORMAT_R64_UINT;
        case RZ_GFX_FORMAT_R64G64_UINT:
            return VK_FORMAT_R64G64_UINT;
        case RZ_GFX_FORMAT_R64G64B64_UINT:
            return VK_FORMAT_R64G64B64_UINT;
        case RZ_GFX_FORMAT_R64G64B64A64_UINT:
            return VK_FORMAT_R64G64B64A64_UINT;

        // 64-bit signed integer formats
        case RZ_GFX_FORMAT_R64_SINT:
            return VK_FORMAT_R64_SINT;
        case RZ_GFX_FORMAT_R64G64_SINT:
            return VK_FORMAT_R64G64_SINT;
        case RZ_GFX_FORMAT_R64G64B64_SINT:
            return VK_FORMAT_R64G64B64_SINT;
        case RZ_GFX_FORMAT_R64G64B64A64_SINT:
            return VK_FORMAT_R64G64B64A64_SINT;

        // 64-bit float formats
        case RZ_GFX_FORMAT_R64_FLOAT:
            return VK_FORMAT_R64_SFLOAT;
        case RZ_GFX_FORMAT_R64G64_FLOAT:
            return VK_FORMAT_R64G64_SFLOAT;
        case RZ_GFX_FORMAT_R64G64B64_FLOAT:
            return VK_FORMAT_R64G64B64_SFLOAT;
        case RZ_GFX_FORMAT_R64G64B64A64_FLOAT:
            return VK_FORMAT_R64G64B64A64_SFLOAT;
#endif    // RAZIX_SUPPORT_64BIT_FORMATS

        // Packed formats
        case RZ_GFX_FORMAT_R11G11B10_FLOAT:
            return VK_FORMAT_B10G11R11_UFLOAT_PACK32;

        // Depth-stencil formats
        case RZ_GFX_FORMAT_D16_UNORM:
            return VK_FORMAT_D16_UNORM;
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case RZ_GFX_FORMAT_D32_FLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;

        // Block compression formats
        case RZ_GFX_FORMAT_BC1_RGBA_UNORM:
            return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case RZ_GFX_FORMAT_BC3_RGBA_UNORM:
            return VK_FORMAT_BC3_UNORM_BLOCK;
        case RZ_GFX_FORMAT_BC6_UNORM:
            return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case RZ_GFX_FORMAT_BC7_UNORM:
            return VK_FORMAT_BC7_UNORM_BLOCK;
        case RZ_GFX_FORMAT_BC7_SRGB:
            return VK_FORMAT_BC7_SRGB_BLOCK;

        default:
            RAZIX_RHI_LOG_WARN("Unknown RZ_GFX_FORMAT value: %d, defaulting to VK_FORMAT_UNDEFINED", format);
            return VK_FORMAT_UNDEFINED;
    }
}

VkAccessFlags vk_util_access_flags_translate(rz_gfx_resource_state state)
{
    switch (state) {
        case RZ_GFX_RESOURCE_STATE_UNDEFINED:
            return 0;

        case RZ_GFX_RESOURCE_STATE_COMMON:
            return VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;

        case RZ_GFX_RESOURCE_STATE_GENERIC_READ:
            return VK_ACCESS_MEMORY_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_RENDER_TARGET:
            return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        case RZ_GFX_RESOURCE_STATE_DEPTH_WRITE:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        case RZ_GFX_RESOURCE_STATE_DEPTH_READ:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_SHADER_READ:
            return VK_ACCESS_SHADER_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS:
            return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

        case RZ_GFX_RESOURCE_STATE_COPY_SRC:
            return VK_ACCESS_TRANSFER_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_COPY_DST:
            return VK_ACCESS_TRANSFER_WRITE_BIT;

        case RZ_GFX_RESOURCE_STATE_PRESENT:
            return VK_ACCESS_MEMORY_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_VERTEX_BUFFER:
            return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_INDEX_BUFFER:
            return VK_ACCESS_INDEX_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_CONSTANT_BUFFER:
            return VK_ACCESS_UNIFORM_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_INDIRECT_ARGUMENT:
            return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_RESOLVE_SRC:
            return VK_ACCESS_TRANSFER_READ_BIT;

        case RZ_GFX_RESOURCE_STATE_RESOLVE_DST:
            return VK_ACCESS_TRANSFER_WRITE_BIT;

        case RZ_GFX_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE:
            return VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

        case RZ_GFX_RESOURCE_STATE_SHADING_RATE_SOURCE:
            return VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;

        case RZ_GFX_RESOURCE_STATE_VIDEO_DECODE_READ:
        case RZ_GFX_RESOURCE_STATE_VIDEO_DECODE_WRITE:
            return 0;

        default:
            return 0;
    }
}

VkPipelineStageFlags vk_deduce_pipeline_stage_from_res_state(rz_gfx_resource_state state)
{
    switch (state) {
        case RZ_GFX_RESOURCE_STATE_UNDEFINED:
            return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        case RZ_GFX_RESOURCE_STATE_COMMON:
        case RZ_GFX_RESOURCE_STATE_GENERIC_READ:
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        case RZ_GFX_RESOURCE_STATE_RENDER_TARGET:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        case RZ_GFX_RESOURCE_STATE_DEPTH_WRITE:
        case RZ_GFX_RESOURCE_STATE_DEPTH_READ:
            return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

        case RZ_GFX_RESOURCE_STATE_SHADER_READ:
        case RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS:
            return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

        case RZ_GFX_RESOURCE_STATE_COPY_SRC:
        case RZ_GFX_RESOURCE_STATE_COPY_DST:
        case RZ_GFX_RESOURCE_STATE_RESOLVE_SRC:
        case RZ_GFX_RESOURCE_STATE_RESOLVE_DST:
            return VK_PIPELINE_STAGE_TRANSFER_BIT;

        case RZ_GFX_RESOURCE_STATE_PRESENT:
            return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        case RZ_GFX_RESOURCE_STATE_VERTEX_BUFFER:
            return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

        case RZ_GFX_RESOURCE_STATE_INDEX_BUFFER:
            return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

        case RZ_GFX_RESOURCE_STATE_CONSTANT_BUFFER:
            return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

        case RZ_GFX_RESOURCE_STATE_INDIRECT_ARGUMENT:
            return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

        case RZ_GFX_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE:
            return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

        case RZ_GFX_RESOURCE_STATE_SHADING_RATE_SOURCE:
            return VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

        case RZ_GFX_RESOURCE_STATE_VIDEO_DECODE_READ:
        case RZ_GFX_RESOURCE_STATE_VIDEO_DECODE_WRITE:
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        default:
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
}

static VkImageAspectFlags vk_util_deduce_image_aspect_flags(rz_gfx_format format)
{
    VkImageAspectFlags aspectFlags = 0;

    // Determine aspect flags based on format
    switch (format) {
        case RZ_GFX_FORMAT_D16_UNORM:
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT:
        case RZ_GFX_FORMAT_D32_FLOAT:
        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT:
            aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
            if (format == RZ_GFX_FORMAT_D24_UNORM_S8_UINT || format == RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT) {
                aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
            break;

        default:
            aspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
            break;
    }

    return aspectFlags;
}

static vk_cmdbuf vk_util_begin_singletime_cmdlist(void)
{
    vk_cmdbuf cmdBuf = {0};

    // Create a temporary command pool for single-time commands
    VkCommandPoolCreateInfo poolInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = VKCONTEXT.queueFamilyIndices.graphicsFamily,
    };

    CHECK_VK(vkCreateCommandPool(VKDEVICE, &poolInfo, NULL, &cmdBuf.cmdPool));
    TAG_OBJECT(cmdBuf.cmdPool, VK_OBJECT_TYPE_COMMAND_POOL, "Single-time Command Pool");

    VkCommandBufferAllocateInfo allocInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool        = cmdBuf.cmdPool,
        .commandBufferCount = 1};

    CHECK_VK(vkAllocateCommandBuffers(VKDEVICE, &allocInfo, &cmdBuf.cmdBuf));
    TAG_OBJECT(cmdBuf.cmdBuf, VK_OBJECT_TYPE_COMMAND_BUFFER, "Single-time Command Buffer");

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

    CHECK_VK(vkBeginCommandBuffer(cmdBuf.cmdBuf, &beginInfo));

    return cmdBuf;
}

static void vk_util_end_singletime_cmdlist(vk_cmdbuf cmdBuf)
{
    CHECK_VK(vkEndCommandBuffer(cmdBuf.cmdBuf));

    VkSubmitInfo submitInfo = {
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmdBuf.cmdBuf};

    CHECK_VK(vkQueueSubmit(VKCONTEXT.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
    vkDeviceWaitIdle(VKDEVICE);

    vkFreeCommandBuffers(VKDEVICE, cmdBuf.cmdPool, 1, &cmdBuf.cmdBuf);
    vkDestroyCommandPool(VKDEVICE, cmdBuf.cmdPool, NULL);
}

static uint32_t vk_util_find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(VKGPU, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    RAZIX_RHI_ASSERT(false, "[Vulkan] Failed to find suitable memory type!");
    return 0;
}

//---------------------------------------------------------------------------------------------

static void vk_GlobalCtxInit(void)
{
    RAZIX_RHI_LOG_INFO("Initializing Vulkan RHI backend");

    volkInitialize();

    // Check validation layer support
    if (!vk_util_check_validation_layer_support()) {
        RAZIX_RHI_LOG_ERROR("Validation lay.hers requested, but not available");
        return;
    }

    // Check instance extension support
    if (!vk_util_check_instance_extension_support()) {
        RAZIX_RHI_LOG_ERROR("Required instance extensions not available");
        return;
    }

    // Create Vulkan instance
    VkApplicationInfo appInfo  = {0};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "PerpetualPuffs";            // TODO: Get this from CtxInit
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);    // TODO: Get this from CtxInit
    appInfo.pEngineName        = "Razix Engine";
    appInfo.engineVersion      = VK_MAKE_VERSION(0, 50, 0);    // TODO: Get this from CtxInit
    appInfo.apiVersion         = RAZIX_VK_API_VERSION;

    VkInstanceCreateInfo createInfo    = {0};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledExtensionCount   = sizeof(s_RequiredInstanceExtensions) / sizeof(s_RequiredInstanceExtensions[0]);
    createInfo.ppEnabledExtensionNames = s_RequiredInstanceExtensions;

#ifdef RAZIX_DEBUG
    createInfo.enabledLayerCount   = sizeof(s_ValidationLayers) / sizeof(s_ValidationLayers[0]);
    createInfo.ppEnabledLayerNames = s_ValidationLayers;

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
    debugCreateInfo.pfnUserCallback = vk_util_debug_callback;

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

    CHECK_VK(vkCreateInstance(&createInfo, NULL, &VKCONTEXT.instance));

    volkLoadInstance(VKCONTEXT.instance);

#ifdef RAZIX_DEBUG
    // Setup debug messenger using dynamically loaded function
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {0};
    debugInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = vk_util_debug_callback;

    // Create debug messenger using dynamically loaded function
    VkResult debugResult = vkCreateDebugUtilsMessengerEXT(
        VKCONTEXT.instance,
        &debugInfo,
        NULL,
        &VKCONTEXT.debugMessenger);
    if (debugResult == VK_SUCCESS) {
        RAZIX_RHI_LOG_INFO("Vulkan debug messenger created successfully");
    } else if (debugResult == VK_ERROR_EXTENSION_NOT_PRESENT) {
        RAZIX_RHI_LOG_WARN("Debug utils extension not available, debug messaging disabled");
    } else {
        RAZIX_RHI_LOG_ERROR("Failed to create debug messenger: %d", debugResult);
    }
#endif

    // Pick physical device
    VKCONTEXT.gpu = vk_util_pick_physical_device();
    if (VKCONTEXT.gpu == VK_NULL_HANDLE) {
        RAZIX_RHI_LOG_ERROR("Failed to pick physical device");
        return;
    }

    // Get device properties and features
    vkGetPhysicalDeviceProperties(VKCONTEXT.gpu, &VKCONTEXT.deviceProperties);
    vkGetPhysicalDeviceFeatures(VKCONTEXT.gpu, &VKCONTEXT.deviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(VKCONTEXT.gpu, &VKCONTEXT.memoryProperties);

    vk_util_print_device_info(VKGPU);

    // Create logical device
    vk_util_create_logical_device();

    volkLoadDevice(VKCONTEXT.device);

    RAZIX_RHI_LOG_INFO("Vulkan RHI backend initialized successfully");
}

static void vk_GlobalCtxDestroy(void)
{
    RAZIX_RHI_LOG_INFO("Destroying Vulkan RHI backend");

    if (VKCONTEXT.device) {
        vkDestroyDevice(VKCONTEXT.device, NULL);
        VKCONTEXT.device = VK_NULL_HANDLE;
    }

#ifdef RAZIX_DEBUG
    if (VKCONTEXT.debugMessenger) {
        vkDestroyDebugUtilsMessengerEXT(
            VKCONTEXT.instance,
            VKCONTEXT.debugMessenger,
            NULL);
        VKCONTEXT.debugMessenger = VK_NULL_HANDLE;
        RAZIX_RHI_LOG_INFO("Vulkan debug messenger destroyed");
    }
#endif

    if (VKCONTEXT.instance) {
        vkDestroyInstance(VKCONTEXT.instance, NULL);
        VKCONTEXT.instance = VK_NULL_HANDLE;
    }

    volkFinalize();

    RAZIX_RHI_LOG_INFO("Vulkan RHI backend destroyed");
}

static void vk_CreateSwapchain(void* where, void* surface, uint32_t width, uint32_t height)
{
    rz_gfx_swapchain* swapchain = (rz_gfx_swapchain*) where;

    RAZIX_RHI_ASSERT(surface != NULL, "VkSurfaceKHR pointer is null, cannot create swapchain without valid surface!");
    RAZIX_RHI_ASSERT(width > 0 && height > 0, "Swapchain width and height must be greater than zero");
    VKCONTEXT.surface = *(VkSurfaceKHR*) surface;

    swapchain->width  = width;
    swapchain->height = height;

    VkSwapchainSupportDetails swapchainSupport = vk_util_query_swapchain_support(VKCONTEXT.gpu, VKCONTEXT.surface);

    VkSurfaceFormatKHR surfaceFormat = vk_util_choose_swap_surface_format(swapchainSupport.formats, swapchainSupport.formatCount);
    VkPresentModeKHR   presentMode   = vk_util_choose_swap_present_mode(swapchainSupport.presentModes, swapchainSupport.presentModeCount);
    VkExtent2D         extent        = vk_util_choose_swap_extent(&swapchainSupport.capabilities, width, height);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface                  = VKCONTEXT.surface;
    createInfo.minImageCount            = imageCount;
    createInfo.imageFormat              = surfaceFormat.format;
    createInfo.imageColorSpace          = surfaceFormat.colorSpace;
    createInfo.imageExtent              = extent;
    createInfo.imageArrayLayers         = 1;
    createInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    VkQueueFamilyIndices indices              = VKCONTEXT.queueFamilyIndices;
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

    CHECK_VK(vkCreateSwapchainKHR(VKCONTEXT.device, &createInfo, NULL, &swapchain->vk.swapchain));
    TAG_OBJECT(swapchain->vk.swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, "Vulkan Swapchain");

    swapchain->vk.imageFormat = surfaceFormat.format;
    swapchain->width          = extent.width;
    swapchain->height         = extent.height;

    // Create images and image views and texture wrappers using utility functions
    vk_util_create_swapchain_images(swapchain);
    vk_util_create_swapchain_textures(swapchain);

    // Transition swapchain images to present state
    vk_cmdbuf             cmdBuf             = vk_util_begin_singletime_cmdlist();
    VkImageMemoryBarrier* pSwapchainBarriers = (VkImageMemoryBarrier*) alloca(sizeof(VkImageMemoryBarrier) * swapchain->imageCount);
    for (uint32_t i = 0; i < swapchain->imageCount; i++) {
        VkImageMemoryBarrier imageBarrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext               = NULL,
            .srcAccessMask       = vk_util_access_flags_translate(RZ_GFX_RESOURCE_STATE_UNDEFINED),
            .dstAccessMask       = vk_util_access_flags_translate(RZ_GFX_RESOURCE_STATE_PRESENT),
            .oldLayout           = vk_util_translate_imagelayout_resstate(RZ_GFX_RESOURCE_STATE_UNDEFINED),
            .newLayout           = vk_util_translate_imagelayout_resstate(RZ_GFX_RESOURCE_STATE_PRESENT),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = swapchain->vk.images[i],
            .subresourceRange    = {
                   .aspectMask     = vk_util_deduce_image_aspect_flags(RAZIX_SWAPCHAIN_FORMAT),
                   .baseMipLevel   = 0,
                   .levelCount     = 1,
                   .baseArrayLayer = 0,
                   .layerCount     = 1},
        };

        // Temp memory but fine
        pSwapchainBarriers[i] = imageBarrier;
    }
    vkCmdPipelineBarrier(
        cmdBuf.cmdBuf,
        vk_deduce_pipeline_stage_from_res_state(RZ_GFX_RESOURCE_STATE_UNDEFINED),
        vk_deduce_pipeline_stage_from_res_state(RZ_GFX_RESOURCE_STATE_PRESENT),
        0,    // dependency flags
        0,
        NULL,    // Global Memory barriers
        0,
        NULL,    // Buffer barriers
        swapchain->imageCount,
        pSwapchainBarriers);    // Image barriers
    vk_util_end_singletime_cmdlist(cmdBuf);

    // Cleanup support details
    free(swapchainSupport.formats);
    free(swapchainSupport.presentModes);

    RAZIX_RHI_LOG_INFO("Vulkan swapchain created: %ux%u, %u images", width, height, imageCount);
}

static void vk_DestroySwapchain(rz_gfx_swapchain* sc)
{
    // Destroy swapchain images and texture wrappers using utility function
    vk_util_destroy_swapchain_images(sc);

    if (sc->vk.images) {
        free(sc->vk.images);
        sc->vk.images = NULL;
    }

    if (sc->vk.swapchain) {
        vkDestroySwapchainKHR(VKCONTEXT.device, sc->vk.swapchain, NULL);
        sc->vk.swapchain = VK_NULL_HANDLE;
    }

    if (VKCONTEXT.surface) {
        vkDestroySurfaceKHR(VKCONTEXT.instance, VKCONTEXT.surface, NULL);
        VKCONTEXT.surface = VK_NULL_HANDLE;
    }

    RAZIX_RHI_LOG_INFO("Vulkan swapchain destroyed");
}

//---------------------------------------------------------------------------------------------
// Empty stub implementations for all other RHI functions
//---------------------------------------------------------------------------------------------

static void vk_CreateSyncobj(void* where, rz_gfx_syncobj_type type)
{
    rz_gfx_syncobj* syncobj = (rz_gfx_syncobj*) where;
    syncobj->type           = type;

    if (type == RZ_GFX_SYNCOBJ_TYPE_CPU) {
        VkFenceCreateInfo fenceInfo = {0};
        fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT;
        CHECK_VK(vkCreateFence(VKCONTEXT.device, &fenceInfo, NULL, &syncobj->vk.fence));
        RAZIX_RHI_LOG_TRACE("Created CPU sync object (fence)");
        TAG_OBJECT(syncobj->vk.fence, VK_OBJECT_TYPE_FENCE, "CPU Sync Fence");
    } else if (type == RZ_GFX_SYNCOBJ_TYPE_GPU) {
        VkSemaphoreCreateInfo semaphoreInfo = {0};
        semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        CHECK_VK(vkCreateSemaphore(VKCONTEXT.device, &semaphoreInfo, NULL, &syncobj->vk.semaphore));
        RAZIX_RHI_LOG_TRACE("Created GPU sync object (semaphore)");
        TAG_OBJECT(syncobj->vk.semaphore, VK_OBJECT_TYPE_SEMAPHORE, "GPU Sync Semaphore");
    } else if (type == RZ_GFX_SYNCOBJ_TYPE_TIMELINE) {
        if (g_GraphicsFeatures.support.TimelineSemaphores) {
            VkSemaphoreTypeCreateInfo timelineCreateInfo = {0};
            timelineCreateInfo.sType                     = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
            timelineCreateInfo.semaphoreType             = VK_SEMAPHORE_TYPE_TIMELINE;
            timelineCreateInfo.initialValue              = 0;
            VkSemaphoreCreateInfo semaphoreInfo          = {0};
            semaphoreInfo.sType                          = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreInfo.pNext                          = &timelineCreateInfo;
            CHECK_VK(vkCreateSemaphore(VKCONTEXT.device, &semaphoreInfo, NULL, &syncobj->vk.semaphore));
            TAG_OBJECT(syncobj->vk.semaphore, VK_OBJECT_TYPE_SEMAPHORE, "Timeline Sync Semaphore");
            RAZIX_RHI_LOG_TRACE("Created Timeline sync object (semaphore)");
        } else {
            RAZIX_RHI_LOG_ERROR("Timeline semaphores not supported on this device");
            syncobj->vk.semaphore = VK_NULL_HANDLE;
        }
    }
}

static void vk_DestroySyncobj(rz_gfx_syncobj* syncobj)
{
    if (syncobj->type == RZ_GFX_SYNCOBJ_TYPE_CPU) {
        vkDestroyFence(VKCONTEXT.device, syncobj->vk.fence, NULL);
        syncobj->vk.fence = VK_NULL_HANDLE;
    } else if (syncobj->type == RZ_GFX_SYNCOBJ_TYPE_GPU || syncobj->type == RZ_GFX_SYNCOBJ_TYPE_TIMELINE) {
        vkDestroySemaphore(VKCONTEXT.device, syncobj->vk.semaphore, NULL);
        syncobj->vk.semaphore = VK_NULL_HANDLE;
    }
}

static void vk_CreateCmdPool(void* where)
{
    rz_gfx_cmdpool* cmdPool = (rz_gfx_cmdpool*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&cmdPool->resource.handle), "Invalid cmd pool handle, who is allocating this? ResourceManager should create a valid handle");

    VkCommandPoolCreateInfo poolInfo = {0};
    poolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext                   = NULL;
    poolInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;    // Allow individual command buffers to be reset
    switch (cmdPool->resource.desc.cmdpoolDesc.poolType) {
        case RZ_GFX_CMDPOOL_TYPE_GRAPHICS: poolInfo.queueFamilyIndex = VKCONTEXT.queueFamilyIndices.graphicsFamily; break;
        case RZ_GFX_CMDPOOL_TYPE_COMPUTE: poolInfo.queueFamilyIndex = VKCONTEXT.queueFamilyIndices.computeFamily; break;
        case RZ_GFX_CMDPOOL_TYPE_TRANSFER: poolInfo.queueFamilyIndex = VKCONTEXT.queueFamilyIndices.transferFamily; break;
        default:
            RAZIX_RHI_LOG_WARN("Unknown command pool type, defaulting to graphics");
            poolInfo.queueFamilyIndex = VKCONTEXT.queueFamilyIndices.graphicsFamily;
            break;
    }

    CHECK_VK(vkCreateCommandPool(VKDEVICE, &poolInfo, NULL, &cmdPool->vk.cmdPool));
    TAG_OBJECT(cmdPool->vk.cmdPool, VK_OBJECT_TYPE_COMMAND_POOL, cmdPool->resource.pName);
}

static void vk_DestroyCmdPool(void* cmdPool)
{
    rz_gfx_cmdpool* pool = (rz_gfx_cmdpool*) cmdPool;
    if (pool->vk.cmdPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(VKDEVICE, pool->vk.cmdPool, NULL);
        pool->vk.cmdPool = VK_NULL_HANDLE;
    }
}

static void vk_CreateCmdBuf(void* where)
{
    rz_gfx_cmdbuf* cmdBuf = (rz_gfx_cmdbuf*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&cmdBuf->resource.handle), "Invalid command buffer handle, who is allocating this? ResourceManager should create a valid handle");
    const rz_gfx_cmdpool* cmdPool = cmdBuf->resource.desc.cmdbufDesc.pool;
    RAZIX_RHI_ASSERT(cmdPool != NULL, "Command buffer must have a valid command pool");
    RAZIX_RHI_ASSERT(cmdPool->vk.cmdPool != VK_NULL_HANDLE, "Command buffer must have a valid command pool");

    // cache the command pool
    cmdBuf->vk.cmdPool = cmdPool->vk.cmdPool;

    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext                       = NULL;
    allocInfo.commandPool                 = cmdPool->vk.cmdPool;
    allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;    // Only primary command buffers for now, secondary can be added later if needed for multi-threading
    allocInfo.commandBufferCount          = 1;                                  // Allocate one command buffer at a time

    CHECK_VK(vkAllocateCommandBuffers(VKDEVICE, &allocInfo, &cmdBuf->vk.cmdBuf));
    TAG_OBJECT(cmdBuf->vk.cmdBuf, VK_OBJECT_TYPE_COMMAND_BUFFER, cmdBuf->resource.pName);
}

static void vk_DestroyCmdBuf(void* cmdBuf)
{
    rz_gfx_cmdbuf* buffer = (rz_gfx_cmdbuf*) cmdBuf;
    if (buffer->vk.cmdBuf != VK_NULL_HANDLE && buffer->vk.cmdPool != VK_NULL_HANDLE) {
        // Let's ignore freeing, command pool will take care of it
        // ***vkFreeCommandBuffers(VKDEVICE, buffer->vk.cmdPool, 1, &buffer->vk.cmdBuf);***
        buffer->vk.cmdBuf = VK_NULL_HANDLE;
    }
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
    rz_gfx_sampler* sampler = (rz_gfx_sampler*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&sampler->resource.handle), "Invalid sampler handle, who is allocating this? ResourceManager should create a valid handle");
    rz_gfx_sampler_desc* desc = &sampler->resource.desc.samplerDesc;
    RAZIX_RHI_ASSERT(desc != NULL, "Sampler description cannot be null");

    VkSamplerCreateInfo samplerInfo     = {0};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext                   = NULL;
    samplerInfo.magFilter               = vk_util_translate_filter_type(desc->magFilter);
    samplerInfo.minFilter               = vk_util_translate_filter_type(desc->minFilter);
    samplerInfo.mipmapMode              = vk_util_translate_mipmap_filter_type(desc->mipFilter);
    samplerInfo.addressModeU            = vk_util_translate_address_mode(desc->addressModeU);
    samplerInfo.addressModeV            = vk_util_translate_address_mode(desc->addressModeV);
    samplerInfo.addressModeW            = vk_util_translate_address_mode(desc->addressModeW);
    samplerInfo.anisotropyEnable        = VK_FALSE;    //desc->maxAnisotropy ? VK_TRUE : VK_FALSE;
    samplerInfo.maxAnisotropy           = (float) desc->maxAnisotropy;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable           = VK_FALSE;    // Not using comparison sampling for now
    samplerInfo.compareOp               = vk_util_translate_compare_op(desc->compareOp);
    samplerInfo.mipLodBias              = desc->mipLODBias;
    samplerInfo.minLod                  = desc->minLod;
    samplerInfo.maxLod                  = desc->maxLod;

    CHECK_VK(vkCreateSampler(VKDEVICE, &samplerInfo, NULL, &sampler->vk.sampler));
    TAG_OBJECT(sampler->vk.sampler, VK_OBJECT_TYPE_SAMPLER, sampler->resource.pName);
}

static void vk_DestroySampler(void* sampler)
{
    rz_gfx_sampler* s = (rz_gfx_sampler*) sampler;
    if (s->vk.sampler != VK_NULL_HANDLE) {
        vkDestroySampler(VKDEVICE, s->vk.sampler, NULL);
        s->vk.sampler = VK_NULL_HANDLE;
    }
}

static void vk_CreateBuffer(void* where)
{
    rz_gfx_buffer* buffer = (rz_gfx_buffer*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&buffer->resource.handle), "Invalid buffer handle, who is allocating this? ResourceManager should create a valid handle");
    rz_gfx_buffer_desc* desc = &buffer->resource.desc.bufferDesc;
    RAZIX_RHI_ASSERT(desc != NULL, "Buffer description cannot be null");
    RAZIX_RHI_ASSERT(desc->sizeInBytes > 0, "Buffer size must be greater than zero");

    // TODO: Implement buffer creation
    RAZIX_RHI_LOG_ERROR("Buffer implementation is not done yet!");
    RAZIX_RHI_ABORT();
}

static void vk_DestroyBuffer(void* buffer)
{
    rz_gfx_buffer* buf = (rz_gfx_buffer*) buffer;
    if (buf->vk.buffer != VK_NULL_HANDLE) {
        if (buf->vk.memory != VK_NULL_HANDLE) {
            vkFreeMemory(VKDEVICE, buf->vk.memory, NULL);
            buf->vk.memory = VK_NULL_HANDLE;
        }
        vkDestroyBuffer(VKDEVICE, buf->vk.buffer, NULL);
        buf->vk.buffer = VK_NULL_HANDLE;
    }
}

static void vk_CreateResourceView(void* where)
{
    rz_gfx_resource_view* view = (rz_gfx_resource_view*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&view->resource.handle), "Invalid resource view handle, who is allocating this? ResourceManager should create a valid handle");
    //  TODO: Implement resource view creation
    RAZIX_RHI_LOG_ERROR("Resource view implementation is not done yet!");
    RAZIX_RHI_ABORT();
}

static void vk_DestroyResourceView(void* view)
{
    (void) view;
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

static void vk_AcquireImage(rz_gfx_swapchain* sc, const rz_gfx_syncobj* presentSignalSyncobj)
{
    RAZIX_RHI_ASSERT(sc != NULL, "Swapchain cannot be null");
    RAZIX_RHI_ASSERT(sc->vk.swapchain != VK_NULL_HANDLE, "Vulkan swapchain is invalid");
    RAZIX_RHI_ASSERT(presentSignalSyncobj != NULL, "Present signal sync object cannot be null");
    RAZIX_RHI_ASSERT(presentSignalSyncobj->type == RZ_GFX_SYNCOBJ_TYPE_GPU, "Present signal sync object must be a GPU semaphore");

    VkResult result = vkAcquireNextImageKHR(VKCONTEXT.device, sc->vk.swapchain, UINT64_MAX, presentSignalSyncobj->vk.semaphore, VK_NULL_HANDLE, &sc->currBackBufferIdx);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RAZIX_RHI_LOG_WARN("Swapchain out of date or suboptimal, image index: %u", sc->currBackBufferIdx);
        // TODO: Handle swapchain recreation
    } else if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("Failed to acquire swapchain image: %d", result);
        return;
    }
}

static void vk_WaitOnPrevCmds(const rz_gfx_syncobj* syncobj)
{
    RAZIX_RHI_ASSERT(syncobj != NULL, "Sync object cannot be null");
    // TODO: Support waiting on multiple syncobjs

    if (syncobj->type == RZ_GFX_SYNCOBJ_TYPE_CPU) {
#if ENABLE_SYNC_LOGGING
        RAZIX_RHI_LOG_TRACE("Waiting on fence (sync point: %llu)", (unsigned long long) syncobj->waitSyncpoint);
#endif
        CHECK_VK(vkWaitForFences(VKCONTEXT.device, 1, &syncobj->vk.fence, VK_TRUE, UINT64_MAX));
        CHECK_VK(vkResetFences(VKCONTEXT.device, 1, &syncobj->vk.fence));

#if ENABLE_SYNC_LOGGING
        RAZIX_RHI_LOG_TRACE("Waited on fence (sync point: %llu)", (unsigned long long) syncobj->waitSyncpoint);
#endif
    } else if (syncobj->type == RZ_GFX_SYNCOBJ_TYPE_TIMELINE) {
        if (g_GraphicsFeatures.support.TimelineSemaphores) {
            VkSemaphoreWaitInfo waitInfo = {
                .sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
                .semaphoreCount = 1,
                .pSemaphores    = &syncobj->vk.semaphore,
                .pValues        = &syncobj->waitSyncpoint};
            CHECK_VK(vkWaitSemaphores(VKDEVICE, &waitInfo, UINT64_MAX));
        } else {
            RAZIX_RHI_LOG_ERROR("Timeline semaphores not supported on this device, cannot wait on timeline semaphore");
        }
    } else {
        RAZIX_RHI_LOG_WARN("sync object type must be either CPU/Timeline to wait on in WaitOnPrevCmds, cannot wait for GPU syncobj on CPU via this function");
    }
}

static void vk_SubmitCmdBuf(rz_gfx_submit_desc submitDesc)
{
    RAZIX_RHI_ASSERT(submitDesc.cmdCount > 0, "Command buffer count must be greater than zero");
    RAZIX_RHI_ASSERT(submitDesc.pCmdBufs != NULL, "Command buffer array cannot be NULL");
    RAZIX_RHI_ASSERT(submitDesc.pFrameSyncobj != NULL, "Frame sync object cannot be NULL");
    RAZIX_RHI_ASSERT(submitDesc.pWaitSyncobjs == NULL || submitDesc.waitSyncobjCount > 0, "Wait sync object count must be greater than zero if wait sync objects are provided");
    RAZIX_RHI_ASSERT(submitDesc.pSignalSyncobjs == NULL || submitDesc.signalSyncobjCount > 0, "Signal sync object count must be greater than zero if signal sync objects are provided");

    VkCommandBuffer* pCmdBuffers = alloca(submitDesc.cmdCount * sizeof(VkCommandBuffer));
    RAZIX_RHI_ASSERT(pCmdBuffers != NULL, "Failed to allocate stack memory for command buffers");
    for (uint32_t i = 0; i < submitDesc.cmdCount; i++)
        pCmdBuffers[i] = submitDesc.pCmdBufs[i].vk.cmdBuf;

    bool                          hasTimelineSyncobj = g_GraphicsFeatures.support.TimelineSemaphores && submitDesc.pFrameSyncobj->type == RZ_GFX_SYNCOBJ_TYPE_TIMELINE;
    VkTimelineSemaphoreSubmitInfo timelineSubmtInfo  = {0};

    uint32_t signalSemaphoreCount = submitDesc.signalSyncobjCount;
    if (hasTimelineSyncobj) {
        // We currently use timeline semaphore only for frame syncobj, that is to signal when the frame's rendering is done and wait on CPU
        // GPU synchronization using timeline semaphore is not implemented yet
        // Global tracker to tell where to signal to
        uint64_t signalValue = ++submitDesc.pFrameSyncobj->waitSyncpoint;

        timelineSubmtInfo = (VkTimelineSemaphoreSubmitInfo) {
            .sType                     = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
            .pNext                     = NULL,
            .pSignalSemaphoreValues    = &signalValue,
            .signalSemaphoreValueCount = 1,
            .pWaitSemaphoreValues      = NULL,
            .waitSemaphoreValueCount   = 0,
        };
        // Signal timeline semaphore for CPU/GPU sync in addition` to rendering done
        signalSemaphoreCount++;
    }

    VkSemaphore*          pSignalSemaphores = alloca(signalSemaphoreCount * sizeof(VkSemaphore));
    VkSemaphore*          pWaitSemaphores   = alloca(submitDesc.waitSyncobjCount * sizeof(VkSemaphore));
    VkPipelineStageFlags* pWaitStages       = alloca(submitDesc.waitSyncobjCount * sizeof(VkPipelineStageFlags));
    RAZIX_RHI_ASSERT(pSignalSemaphores != NULL, "Failed to allocate stack memory for signal semaphores");
    RAZIX_RHI_ASSERT(pWaitSemaphores != NULL, "Failed to allocate stack memory for wait semaphores");
    RAZIX_RHI_ASSERT(pWaitStages != NULL, "Failed to allocate stack memory for wait stages");

    for (uint32_t i = 0; i < submitDesc.waitSyncobjCount; i++) {
        pWaitSemaphores[i] = submitDesc.pWaitSyncobjs[i].vk.semaphore;
        pWaitStages[i]     = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;    // yeah we wait on all, can be optimized later if needed
    }

    for (uint32_t i = 0; i < submitDesc.signalSyncobjCount; i++)
        pSignalSemaphores[i] = submitDesc.pSignalSyncobjs[i].vk.semaphore;

    // If using timeline semaphore, add it to the end of signal semaphores
    if (hasTimelineSyncobj)
        pSignalSemaphores[signalSemaphoreCount - 1] = submitDesc.pFrameSyncobj->vk.semaphore;

    VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = hasTimelineSyncobj ? &timelineSubmtInfo : NULL,
        .pWaitDstStageMask    = pWaitStages,
        .waitSemaphoreCount   = submitDesc.waitSyncobjCount,
        .pWaitSemaphores      = pWaitSemaphores,
        .signalSemaphoreCount = submitDesc.signalSyncobjCount,
        .pSignalSemaphores    = pSignalSemaphores,
        .commandBufferCount   = submitDesc.cmdCount,
        .pCommandBuffers      = pCmdBuffers,
    };

    CHECK_VK(vkQueueSubmit(VKCONTEXT.graphicsQueue, 1, &submitInfo, !hasTimelineSyncobj ? submitDesc.pFrameSyncobj->vk.fence : VK_NULL_HANDLE));
}

static void vk_Present(rz_gfx_present_desc presentDesc)
{
    RAZIX_RHI_ASSERT(presentDesc.pSwapchain != NULL, "Swapchain cannot be null");
    RAZIX_RHI_ASSERT(presentDesc.pSwapchain->vk.swapchain != VK_NULL_HANDLE, "Vulkan swapchain is invalid");
    RAZIX_RHI_ASSERT(presentDesc.pWaitSyncobjs == NULL || presentDesc.waitSyncobjCount > 0, "Wait sync object count must be greater than zero if wait sync objects are provided");

    VkSemaphore* pWaitSemaphores = alloca(presentDesc.waitSyncobjCount * sizeof(VkSemaphore));
    RAZIX_RHI_ASSERT(pWaitSemaphores != NULL, "Failed to allocate stack memory for wait semaphores");

    for (uint32_t i = 0; i < presentDesc.waitSyncobjCount; i++)
        pWaitSemaphores[i] = presentDesc.pWaitSyncobjs[i].vk.semaphore;

    VkPresentInfoKHR presentInfo = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = presentDesc.waitSyncobjCount,
        .pWaitSemaphores    = pWaitSemaphores,
        .swapchainCount     = 1,
        .pSwapchains        = &presentDesc.pSwapchain->vk.swapchain,
        .pImageIndices      = &presentDesc.pSwapchain->currBackBufferIdx,
        .pResults           = NULL};

    VkResult result = vkQueuePresentKHR(VKCONTEXT.graphicsQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RAZIX_RHI_LOG_WARN("Swapchain out of date or suboptimal during present, VK_SUBOPTIMIAL_KHR means the presentation was successful and you probably resized, or requires to recreate the swapchain again because it's out of date.");
        // TODO: Handle swapchain recreation
        RAZIX_RHI_ASSERT(false, "Swapchain recreation not implemented yet!");
    } else if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("Failed to present swapchain image (VkResult): %d", result);
        return;
    }
}

static void vk_BeginCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");

    VkCommandBufferBeginInfo beginInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags            = 0,
        .pInheritanceInfo = NULL};

    VkResult result = vkBeginCommandBuffer(cmdBuf->vk.cmdBuf, &beginInfo);
    if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("Failed to begin command buffer recording: %d", result);
        return;
    }
}

static void vk_EndCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");

    VkResult result = vkEndCommandBuffer(cmdBuf->vk.cmdBuf);
    if (result != VK_SUCCESS) {
        RAZIX_RHI_LOG_ERROR("Failed to end command buffer recording: %d", result);
        return;
    }
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
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(viewport != NULL, "Viewport cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");

    VkViewport vkViewport = {
        .x        = (float) viewport->x,
        .y        = (float) viewport->y,
        .width    = (float) viewport->width,
        .height   = (float) viewport->height,
        .minDepth = (float) viewport->minDepth,
        .maxDepth = (float) viewport->maxDepth};

    vkCmdSetViewport(cmdBuf->vk.cmdBuf, 0, 1, &vkViewport);
}

static void vk_SetScissorRect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_rect* rect)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(rect != NULL, "Scissor rect cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");

    VkRect2D scissor = {
        .offset = {.x = rect->x, .y = rect->y},
        .extent = {.width = rect->width, .height = rect->height}};

    vkCmdSetScissor(cmdBuf->vk.cmdBuf, 0, 1, &scissor);
}

static void vk_BindPipeline(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_pipeline* pipeline)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(pipeline != NULL, "Pipeline cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(pipeline->vk.pipeline != VK_NULL_HANDLE, "Vulkan pipeline is invalid");

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
}

static void vk_BindGfxRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(rootSig != NULL, "Root signature cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(rootSig->vk.pipelineLayout != VK_NULL_HANDLE, "Vulkan pipeline layout is invalid");

    // In Vulkan, root signatures correspond to descriptor sets bound to pipeline layouts
    // For now, we'll just bind the pipeline layout - descriptor sets need to be bound separately
    // This is a placeholder implementation that sets up the pipeline layout binding

    // Note: Actual descriptor set binding would happen when descriptors are available
    // vkCmdBindDescriptorSets(cmdBuf->vk.cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         rootSig->vk.pipelineLayout, 0, descriptorSetCount,
    //                         pDescriptorSets, 0, nullptr);
}

static void vk_BindComputeRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(rootSig != NULL, "Root signature cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(rootSig->vk.pipelineLayout != VK_NULL_HANDLE, "Vulkan pipeline layout is invalid");

    // In Vulkan, root signatures correspond to descriptor sets bound to pipeline layouts
    // For now, we'll just bind the pipeline layout - descriptor sets need to be bound separately
    // This is a placeholder implementation that sets up the pipeline layout binding

    // Note: Actual descriptor set binding would happen when descriptors are available
    // vkCmdBindDescriptorSets(cmdBuf->vk.cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE,
    //                         rootSig->vk.pipelineLayout, 0, descriptorSetCount,
    //                         pDescriptorSets, 0, nullptr);
}

static void vk_DrawAuto(const rz_gfx_cmdbuf* cmdBuf, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(vertexCount > 0, "Vertex count must be greater than 0");
    RAZIX_RHI_ASSERT(instanceCount > 0, "Instance count must be greater than 0");

    vkCmdDraw(cmdBuf->vk.cmdBuf, vertexCount, instanceCount, firstVertex, firstInstance);
}

static void vk_DrawIndexedAuto(const rz_gfx_cmdbuf* cmdBuf, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(indexCount > 0, "Index count must be greater than 0");
    RAZIX_RHI_ASSERT(instanceCount > 0, "Instance count must be greater than 0");

    vkCmdDrawIndexed(cmdBuf->vk.cmdBuf, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

static void vk_Dispatch(const rz_gfx_cmdbuf* cmdBuf, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(groupCountX > 0 && groupCountY > 0 && groupCountZ > 0, "Group counts must be greater than 0");

    vkCmdDispatch(cmdBuf->vk.cmdBuf, groupCountX, groupCountY, groupCountZ);
}

static void vk_DrawAutoIndirect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* buffer, uint32_t offset, uint32_t drawCount)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(buffer->vk.buffer != VK_NULL_HANDLE, "Vulkan buffer is invalid");
    RAZIX_RHI_ASSERT(drawCount > 0, "Draw count must be greater than 0");

    vkCmdDrawIndirect(cmdBuf->vk.cmdBuf, buffer->vk.buffer, offset, drawCount, sizeof(VkDrawIndirectCommand));
}

static void vk_DrawIndexedAutoIndirect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* buffer, uint32_t offset, uint32_t drawCount)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(buffer->vk.buffer != VK_NULL_HANDLE, "Vulkan buffer is invalid");
    RAZIX_RHI_ASSERT(drawCount > 0, "Draw count must be greater than 0");

    vkCmdDrawIndexedIndirect(cmdBuf->vk.cmdBuf, buffer->vk.buffer, offset, drawCount, sizeof(VkDrawIndexedIndirectCommand));
}

static void vk_DispatchIndirect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* buffer, uint32_t offset, uint32_t dispatchCount)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer cannot be null");
    RAZIX_RHI_ASSERT(cmdBuf->vk.cmdBuf != VK_NULL_HANDLE, "Vulkan command buffer is invalid");
    RAZIX_RHI_ASSERT(buffer->vk.buffer != VK_NULL_HANDLE, "Vulkan buffer is invalid");
    RAZIX_RHI_ASSERT(dispatchCount > 0, "Dispatch count must be greater than 0");

    vkCmdDispatchIndirect(cmdBuf->vk.cmdBuf, buffer->vk.buffer, offset);
}

static void vk_UpdateConstantBuffer(rz_gfx_buffer_update updatedesc)
{
    RAZIX_RHI_ASSERT(updatedesc.pBuffer != NULL, "Buffer cannot be NULL");
    RAZIX_RHI_ASSERT(updatedesc.sizeInBytes > 0, "Size in bytes must be greater than zero");
    RAZIX_RHI_ASSERT(updatedesc.offset + updatedesc.sizeInBytes <= updatedesc.pBuffer->resource.desc.bufferDesc.sizeInBytes, "Update range exceeds buffer size");
    RAZIX_RHI_ASSERT(updatedesc.pData != NULL, "Data pointer cannot be NULL");
    RAZIX_RHI_ASSERT((updatedesc.pBuffer->resource.desc.bufferDesc.type & RZ_GFX_BUFFER_TYPE_CONSTANT) == RZ_GFX_BUFFER_TYPE_CONSTANT, "Buffer must be of type RZ_GFX_BUFFER_TYPE_CONSTANT to update");
    RAZIX_RHI_ASSERT(
        (updatedesc.pBuffer->resource.desc.bufferDesc.usage & (RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC | RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM)),
        "Buffer must be created with RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC or RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM usage flag");
    // TODO: When using VMA, ignore mapping/unmapping for persistent mapped buffers
    // TODO: Store the mapped pointer in the buffer struct to avoid mapping/unmapping every time for persistent mapped buffers

    //if(updatedesc.pBuffer->resource.desc.bufferDesc.usage == RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM) {
    //   // cache the mapped pointer
    //}

    void* mappedData = NULL;
    CHECK_VK(vkMapMemory(VKDEVICE, updatedesc.pBuffer->vk.memory, updatedesc.offset, updatedesc.sizeInBytes, 0, (void**) &mappedData));
    RAZIX_RHI_ASSERT(mappedData != NULL, "Failed to map constant buffer memory");
    memcpy((uint8_t*) (mappedData + updatedesc.offset), updatedesc.pData, updatedesc.sizeInBytes);
    // unmapping is not required for HOST_VISIBLE | HOST_COHERENT memory,
    // but doing it anyway for safety, we will try to prefer
    // HOST_COHERENT memory for dynamic and persistent uniform buffer
    vkUnmapMemory(VKDEVICE, updatedesc.pBuffer->vk.memory);
}

static void vk_InsertImageBarrier(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_texture* texture, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(texture != NULL, "Texture cannot be NULL");
    RAZIX_RHI_ASSERT(beforeState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "Before state cannot be undefined");
    RAZIX_RHI_ASSERT(afterState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "After state cannot be undefined");
    RAZIX_RHI_ASSERT(!(texture->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV) ||
                         (beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS || afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS),
        "UAV barriers must be used only with UAV resources. If the resource has UAV view hint, either before or after state must be UAV");
    RAZIX_RHI_ASSERT(!(beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS && afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS) ||
                         (texture->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV),
        "UAV-to-UAV barrier requires resource to have UAV view hint");

    bool isUAVBarrier = (beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS && afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS);

#if RAZIX_ENABLE_COARSE_UAV_BARRIERS
    if (beforeState == afterState) {
        // Memory barrier for UAVs, broad but simple
        VkMemoryBarrier memBarrier = {
            .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT};

        vkCmdPipelineBarrier(cmdBuf->vk.cmdBuf,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,    // src
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,    // dst
            0,
            1,
            &memBarrier,
            0,
            NULL,
            0,
            NULL);
        return;
    }
#endif    // RAZIX_ENABLE_COARSE_UAV_BARRIERS

    if (!isUAVBarrier && (beforeState == afterState))
        return;

    VkImageMemoryBarrier imageBarrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = NULL,
        .srcAccessMask       = vk_util_access_flags_translate(beforeState),
        .dstAccessMask       = vk_util_access_flags_translate(afterState),
        .oldLayout           = vk_util_translate_imagelayout_resstate(beforeState),
        .newLayout           = vk_util_translate_imagelayout_resstate(afterState),
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = texture->vk.image,
        .subresourceRange    = {
               .aspectMask     = vk_util_deduce_image_aspect_flags(texture->resource.desc.textureDesc.format),
               .baseMipLevel   = 0,
               .levelCount     = texture->resource.desc.textureDesc.mipLevels,
               .baseArrayLayer = 0,
               .layerCount     = texture->resource.desc.textureDesc.arraySize},
    };

    vkCmdPipelineBarrier(
        cmdBuf->vk.cmdBuf,
        vk_deduce_pipeline_stage_from_res_state(beforeState),
        vk_deduce_pipeline_stage_from_res_state(afterState),
        0,    // dependency flags
        0,
        NULL,    // Global Memory barriers
        0,
        NULL,    // Buffer barriers
        1,
        &imageBarrier);    // Image barriers

    // Update the current state
    texture->resource.currentState = afterState;
}

static void vk_InsertBufferBarrier(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_buffer* buffer, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer cannot be NULL");
    RAZIX_RHI_ASSERT(beforeState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "Before state cannot be undefined");
    RAZIX_RHI_ASSERT(afterState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "After state cannot be undefined");
    RAZIX_RHI_ASSERT(!(buffer->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV) ||
                         (beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS || afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS),
        "UAV barriers must be used only with UAV resources. If the resource has UAV view hint, either before or after state must be UAV");
    RAZIX_RHI_ASSERT(!(beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS && afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS) ||
                         (buffer->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV),
        "UAV-to-UAV barrier requires resource to have UAV view hint");

    bool isUAVBarrier = (beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS && afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS);

#if RAZIX_ENABLE_COARSE_UAV_BARRIERS
    if (beforeState == afterState) {
        // Memory barrier for UAVs, broad but simple
        VkMemoryBarrier memBarrier = {
            .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT};

        vkCmdPipelineBarrier(cmdBuf->vk.cmdBuf,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,    // src
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,    // dst
            0,
            1,
            &memBarrier,
            0,
            NULL,
            0,
            NULL);
        return;
    }
#endif    // RAZIX_ENABLE_COARSE_UAV_BARRIERS

    if (!isUAVBarrier && (beforeState == afterState))
        return;

    VkBufferMemoryBarrier bufferBarrier = {
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .pNext               = NULL,
        .srcAccessMask       = vk_util_access_flags_translate(beforeState),
        .dstAccessMask       = vk_util_access_flags_translate(afterState),
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer              = buffer->vk.buffer,
        .offset              = 0,
        .size                = VK_WHOLE_SIZE,
    };

    vkCmdPipelineBarrier(
        cmdBuf->vk.cmdBuf,
        vk_deduce_pipeline_stage_from_res_state(beforeState),
        vk_deduce_pipeline_stage_from_res_state(afterState),
        0,    // dependency flags
        0,
        NULL,    // Global Memory barriers
        1,
        &bufferBarrier,    // Buffer barriers
        0,
        NULL);    // Image barriers

    // Update the current state
    buffer->resource.currentState = afterState;
}

static void vk_InsertTextureReadback(const rz_gfx_texture* texture, rz_gfx_texture_readback* readback)
{
    RAZIX_RHI_ASSERT(texture != NULL, "Texture cannot be NULL");
    RAZIX_RHI_ASSERT(readback != NULL, "Readback structure cannot be NULL");

    const rz_gfx_texture_desc* desc = &texture->resource.desc.textureDesc;
    // TODO: RAZIX_RHI_ASSERT(desc->flags & RZ_GFX_TEXTURE_FLAG_ALLOW_CPU_READ, "Texture must be created with RZ_GFX_TEXTURE_FLAG_ALLOW_CPU_READ flag to readback");

    VkImage  srcImage = texture->vk.image;
    uint32_t width    = desc->width;
    uint32_t height   = desc->height;
    uint32_t size     = width * height * 4;    // Assuming 4 bytes per pixel (RGBA8)

    vk_cmdbuf cmdBuf = vk_util_begin_singletime_cmdlist();

    // Create staging buffer for readback
    VkBuffer       stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkBufferCreateInfo bufferInfo = {
        .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size        = size,
        .usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

    CHECK_VK(vkCreateBuffer(VKDEVICE, &bufferInfo, NULL, &stagingBuffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VKDEVICE, stagingBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize  = memRequirements.size,
        .memoryTypeIndex = vk_util_find_memory_type(memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)};

    CHECK_VK(vkAllocateMemory(VKDEVICE, &allocInfo, NULL, &stagingBufferMemory));
    CHECK_VK(vkBindBufferMemory(VKDEVICE, stagingBuffer, stagingBufferMemory, 0));

    VkAccessFlags      srcAccessMask = vk_util_access_flags_translate(texture->resource.currentState);
    VkImageAspectFlags aspectFlags   = vk_util_deduce_image_aspect_flags(desc->format);

    // Transition texture to transfer source layout
    VkImageMemoryBarrier barrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout           = vk_util_translate_imagelayout_resstate(texture->resource.currentState),
        .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = srcImage,
        .subresourceRange    = {
               .aspectMask     = aspectFlags,
               .baseMipLevel   = 0,
               .levelCount     = 1,
               .baseArrayLayer = 0,
               .layerCount     = 1},
        .srcAccessMask = srcAccessMask,
        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT};

    vkCmdPipelineBarrier(cmdBuf.cmdBuf,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &barrier);

    // Copy image to buffer
    VkBufferImageCopy region = {
        .bufferOffset      = 0,
        .bufferRowLength   = 0,
        .bufferImageHeight = 0,
        .imageSubresource  = {
             .aspectMask     = aspectFlags,
             .mipLevel       = 0,
             .baseArrayLayer = 0,
             .layerCount     = 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {width, height, 1}};

    vkCmdCopyImageToBuffer(cmdBuf.cmdBuf, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer, 1, &region);

    // Transition texture back to original layout
    VkImageMemoryBarrier restoreBarrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .newLayout           = vk_util_translate_imagelayout_resstate(texture->resource.currentState),
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = srcImage,
        .subresourceRange    = {
               .aspectMask     = aspectFlags,
               .baseMipLevel   = 0,
               .levelCount     = 1,
               .baseArrayLayer = 0,
               .layerCount     = 1},
        .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
        .dstAccessMask = srcAccessMask};

    vkCmdPipelineBarrier(cmdBuf.cmdBuf,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &restoreBarrier);

    // Submit and wait for completion
    vk_util_end_singletime_cmdlist(cmdBuf);

    // Map the staging buffer and copy data
    void* mappedData;
    CHECK_VK(vkMapMemory(VKDEVICE, stagingBufferMemory, 0, size, 0, &mappedData));

    readback->width  = width;
    readback->height = height;
    readback->bpp    = 32;
    // TODO: Get a malloc CB from user, and use that instead of allocating memory from RHI
    // Note: Pray to god that user will free this memory
    readback->data = malloc(size);

    if (readback->data) {
        memcpy(readback->data, mappedData, size);
    }

    vkUnmapMemory(VKDEVICE, stagingBufferMemory);

    // Cleanup staging buffer
    vkDestroyBuffer(VKDEVICE, stagingBuffer, NULL);
    vkFreeMemory(VKDEVICE, stagingBufferMemory, NULL);
}

static void vk_SignalGPU(rz_gfx_syncobj* syncobj)
{
    RAZIX_RHI_ASSERT(syncobj != NULL, "Sync object cannot be null");
    RAZIX_RHI_ASSERT(syncobj->type == RZ_GFX_SYNCOBJ_TYPE_GPU || syncobj->type == RZ_GFX_SYNCOBJ_TYPE_TIMELINE, "Sync object must be a GPU or Timeline semaphore");
    ++syncobj->waitSyncpoint;
    RAZIX_RHI_LOG_ERROR("SignalGPU doesn't make sense by it's own in Vulkan without work to submit, incrementing global sync point value as-is, use SubmitWork to sync or use it internally in DX12 backend");
}

static void vk_FlushGPUWork(rz_gfx_syncobj* syncobj)
{
    RAZIX_RHI_ASSERT(syncobj != NULL, "Sync object cannot be null");
    // We could use a vkFence and wait on it here to ensure all GPU work is done
    // or use a timeline semaphore and wait on it using vkWaitSemaphores, but for simplicity,
    // we'll just wait for the device to be idle, this works too
    vkDeviceWaitIdle(VKCONTEXT.device);
}

static void vk_ResizeSwapchain(rz_gfx_swapchain* sc, uint32_t width, uint32_t height)
{
    RAZIX_RHI_ASSERT(sc != NULL, "Swapchain cannot be null");
    RAZIX_RHI_ASSERT(width > 0, "Width must be greater than zero");
    RAZIX_RHI_ASSERT(height > 0, "Height must be greater than zero");
    (void) sc;
    (void) width;
    (void) height;
    // TODO: Implement when needed
    RAZIX_RHI_LOG_ERROR("ResizeSwapchain not implemented yet in Vulkan backend");
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
    .CreateBuffer           = vk_CreateBuffer,              // vk_CreateBuffer
    .DestroyBuffer          = vk_DestroyBuffer,             // vk_DestroyBuffer
    .CreateResourceView     = vk_CreateResourceView,        // CreateResourceView
    .DestroyResourceView    = vk_DestroyResourceView,       // DestroyResourceView
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

    .DrawAuto              = vk_DrawAuto,                   // DrawAuto
    .DrawIndexedAuto       = vk_DrawIndexedAuto,            // DrawIndexedAuto
    .Dispatch              = vk_Dispatch,                   // Dispatch
    .DrawIndirect          = vk_DrawAutoIndirect,           // DrawAutoIndirect
    .DrawIndexedIndirect   = vk_DrawIndexedAutoIndirect,    // DrawIndexedAutoIndirect
    .DispatchIndirect      = vk_DispatchIndirect,           // DispatchIndirect
    .UpdateConstantBuffer  = vk_UpdateConstantBuffer,       // UpdateConstantBuffer
    .InsertImageBarrier    = vk_InsertImageBarrier,         // InsertImageBarrier
    .InsertBufferBarrier   = vk_InsertBufferBarrier,        // InsertBufferBarrier
    .InsertTextureReadback = vk_InsertTextureReadback,      // InsertTextureReadback

    .SignalGPU       = vk_SignalGPU,          // SignalGPU
    .FlushGPUWork    = vk_FlushGPUWork,       // FlushGPUWork
    .ResizeSwapchain = vk_ResizeSwapchain,    // ResizeSwapchain
};
