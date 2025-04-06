// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKContext.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Core/App/RZApplication.h"
    #include "Razix/Core/Profiling/RZProfiling.h"
    #include "Razix/Core/RZEngine.h"
    #include "Razix/Core/Version/RazixVersion.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"
    #include "Razix/Utilities/RZStringUtilities.h"

    #include <glfw/glfw3.h>
    #include <vulkan/vulkan.h>
    #ifdef RAZIX_PLATFORM_WINDOWS
        #define VK_USE_PLATFORM_WIN32_KHR
        #include <vulkan/vulkan_win32.h>
    #endif

    #define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace Razix {
    namespace Gfx {

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
        {
    #ifndef RAZIX_GOLD_MASTER

            // ENABLE THIS WHEN DOING A RENDER DOC CAPTURE! -> API validation is to be disabled and will be done by the config file we pass a different config file for RDC runs
            //return VK_FALSE;

            if (!RZEngine::Get().getGlobalEngineSettings().EnableAPIValidation) return VK_FALSE;

            std::string severity;
            if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
                severity = "ERROR";
            } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                severity = "WARNING";
            } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
                severity = "INFO";
            } else {
                severity = "VERBOSE";
            }

            std::ostringstream logMessage;
            logMessage << "\n==============================================================\n";
            logMessage << "Vulkan Validation Message\n";

            const int labelWidth = 12;
            logMessage << std::left << std::setw(labelWidth) << "Severity"
                       << " : " << severity << "\n";
            logMessage << std::left << std::setw(labelWidth) << "Message ID"
                       << " : " << callback_data->messageIdNumber << "\n";
            logMessage << std::left << std::setw(labelWidth) << "ID Name"
                       << " : " << callback_data->pMessageIdName << "\n";

            if (callback_data->objectCount > 0) {
                logMessage << std::left << std::setw(labelWidth) << "Objects"
                           << " :\n";
                for (uint32_t i = 0; i < callback_data->objectCount; ++i) {
                    logMessage << "  " << std::left << std::setw(labelWidth - 2) << ("Object " + std::to_string(i)) << " :\n";
                    logMessage << "    " << std::left << std::setw(labelWidth - 4) << "Handle"
                               << " : 0x" << std::hex << callback_data->pObjects[i].objectHandle << std::dec << "\n";
                    logMessage << "    " << std::left << std::setw(labelWidth - 4) << "Name"
                               << " : " << (callback_data->pObjects[i].pObjectName ? callback_data->pObjects[i].pObjectName : "N/A") << "\n";
                    logMessage << "    " << std::left << std::setw(labelWidth - 4) << "Type"
                               << " : " << VKUtilities::VulkanObjectTypeString(callback_data->pObjects[i].objectType) << "\n";
                }
            }

            std::string message = callback_data->pMessage;
            auto        parts   = Utilities::SplitString(message, '|');
            logMessage << std::left << std::setw(labelWidth) << "Description"
                       << " :\n";

            if (parts.size() >= 3) {
                logMessage << "  " << std::left << std::setw(labelWidth - 2) << "Error Code"
                           << " : " << parts[0].substr(0, parts[0].find_last_of(']') + 1) << "\n";

                logMessage << "  " << std::left << std::setw(labelWidth - 2) << "MessageID"
                           << " : " << parts[1].substr(parts[1].find("MessageID = ")) << "\n";

                logMessage << "  " << std::left << std::setw(labelWidth - 2) << "Details"
                           << " : " << parts[2] << "\n";
            } else {
                logMessage << "  " << message << "\n";
            }

            if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
                RAZIX_CORE_ERROR("{}", logMessage.str());
            } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                RAZIX_CORE_WARN("{}", logMessage.str());
            } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
                RAZIX_CORE_INFO("{}", logMessage.str());
            } else {
                RAZIX_CORE_TRACE("{}", logMessage.str());
            }

    #endif
            return VK_FALSE;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Load the functions dynamically to create the DebugUtilsMessenger
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            else
                return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr)
                func(instance, debugMessenger, pAllocator);
            else
                RAZIX_CORE_ERROR("DestroyDebugUtilsMessengerEXT Function not found");
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        VKContext::VKContext(RZWindow* windowHandle)
            : m_Window(windowHandle)
        {
            RAZIX_CORE_ASSERT(windowHandle, "[Vulkan] Window Handle is NULL!");
        }

        void VKContext::Init()
        {
            // Create the Vulkan instance to interface with the Vulkan library
            createInstance();

            if (RZApplication::Get().getAppType() == AppType::kGame) {
                SetupDeviceAndSC();
            }

            PostGraphicsContextInit();
        }

        void VKContext::DestroyContext()
        {
            // Destroy the swapchain
            m_Swapchain->Destroy();
            // Destroy the logical device
            VKDevice::Get().destroy();
            // Destroy the surface
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            // Destroy the debug manager
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugCallbackHandle, nullptr);
            // Destroy the instance at last
            vkDestroyInstance(m_Instance, nullptr);
        }

        void VKContext::Wait()
        {
            vkDeviceWaitIdle(VKDevice::Get().getDevice());
        }

        void VKContext::SetupDeviceAndSC()
        {
            // Create the Logical Device
            VKDevice::Get().init();

            // Create the swapchain (will be auto initialized)
            m_Swapchain = rzstl::CreateUniqueRef<VKSwapchain>(m_Window->getWidth(), m_Window->getHeight());

    #ifndef RAZIX_GOLD_MASTER
        #if RZ_PROFILER_OPTICK
            auto device         = VKDevice::Get().getDevice();
            auto physicalDevice = VKDevice::Get().getGPU();
            auto queuefam       = VKDevice::Get().getGraphicsQueue();
            u32  numQueues      = VKDevice::Get().getPhysicalDevice()->getGraphicsQueueFamilyIndex();
            OPTICK_GPU_INIT_VULKAN(&device, &physicalDevice, &queuefam, &numQueues, 1, nullptr);
        #endif    // RZ_PROFILER_OPTICK

    #endif    // RAZIX_GOLD_MASTER

    #if RAZIX_USE_VMA
            // Now create the Vulkan Memory Allocator (VMA)
            //initialize the memory allocator
            VmaAllocatorCreateInfo allocatorInfo = {};
            allocatorInfo.physicalDevice         = VKDevice::Get().getPhysicalDevice()->getVulkanPhysicalDevice();
            allocatorInfo.device                 = VKDevice::Get().getDevice();
            allocatorInfo.instance               = m_Instance;
            if (VK_CHECK_RESULT(vmaCreateAllocator(&allocatorInfo, &VKDevice::Get().getVMA())))
                RAZIX_CORE_ERROR("[VMA] Failed to create VMA allocator!");
            else
                RAZIX_CORE_TRACE("[VMA] Succesfully created VMA allocator!");
    #endif

            //-----------------------------------------------------
            // Get some memory properties of the selected physical device
            auto gpuMemProps = VKDevice::Get().getPhysicalDevice()->getMemoryProperties();

            // Calculate total VRAM by summing up memory heap sizes
            VkDeviceSize totalVRAM = 0;
            for (uint32_t i = 0; i < gpuMemProps.memoryHeapCount; ++i) {
                VkMemoryHeap memoryHeap = gpuMemProps.memoryHeaps[i];
                // Consider only heaps with device-local memory flag
                if ((memoryHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0) {
                    totalVRAM += memoryHeap.size;
                }
            }

            // Print total VRAM size
            RAZIX_CORE_INFO("Total GPU VRAM : {0} Gib", totalVRAM / (f32) (1024 * 1024 * 1024));
            RZEngine::Get().GetStatistics().TotalGPUMemory = (f32) totalVRAM / (f32) (1024 * 1024 * 1024);

            // TODO: Use the memory type to find it's corresponding heap and print it's capacity + filter for specific memory types and cross check with total VRAM and mappable VRAM

    #if RAZIX_USE_VMA
            //-----------------------------------------------------
            // Total statistics of VMA
            VmaTotalStatistics totalStats{};
            vmaCalculateStatistics(VKDevice::Get().getVMA(), &totalStats);
            // Print only the total stats
            VmaBudget* pBudgets = new VmaBudget[gpuMemProps.memoryHeapCount];
            vmaGetHeapBudgets(VKDevice::Get().getVMA(), pBudgets);

            for (u32 i = 0; i < gpuMemProps.memoryHeapCount; i++) {
                f32 memory = ((f32) pBudgets[i].budget / (f32) (1024 * 1024 * 1024));
                RAZIX_CORE_INFO("Heap Idx : {0}, budget : {1} Gib", i, memory);
            }

            // Get statistics string
            char* statsString;
            vmaBuildStatsString(VKDevice::Get().getVMA(), &statsString, VK_TRUE);

            // Print statistics
            printf("Memory statistics:\n%s\n", statsString);

            // Free the allocated stats string
            vmaFreeStatsString(VKDevice::Get().getVMA(), statsString);
    #endif
        }

        void VKContext::createInstance()
        {
            // Vulkan Application Info
            VkApplicationInfo appInfo{};
            appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName   = RZApplication::Get().getAppName().c_str();
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);    // TODO: Add this feature later! once we add it to the Application class
            appInfo.pEngineName        = "Razix Engine";
            appInfo.engineVersion      = VK_MAKE_VERSION(RazixVersion.getVersionMajor(), RazixVersion.getVersionMinor(), RazixVersion.getVersionPatch());
            appInfo.apiVersion         = VK_API_VERSION_1_3;

            // Instance Create Info
            VkInstanceCreateInfo instanceCI{};
            instanceCI.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCI.pApplicationInfo = &appInfo;
    #ifdef __APPLE__
            instanceCI.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif

            // To track if there is any issue with instance creation we supply the pNext with the `VkDebugUtilsMessengerCreateInfoEXT`
            m_DebugCI                 = {};
            m_DebugCI.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            m_DebugCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            m_DebugCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            m_DebugCI.pfnUserCallback = debugCallback;

            instanceCI.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &m_DebugCI;

            // Get the Required Instance Layers from the app/engine
            m_RequiredInstanceLayerNames = getRequiredLayers();
            // Get the Required Instance aka Global Extension (also applicable for the device and application)
            m_RequiredInstanceExtensionNames = getRequiredExtensions();

            // Get the Instance Layers and Extensions
            instanceCI.enabledLayerCount       = static_cast<u32>(m_RequiredInstanceLayerNames.size());
            instanceCI.ppEnabledLayerNames     = m_RequiredInstanceLayerNames.data();
            instanceCI.enabledExtensionCount   = static_cast<u32>(m_RequiredInstanceExtensionNames.size());
            instanceCI.ppEnabledExtensionNames = m_RequiredInstanceExtensionNames.data();

            if (VK_CHECK_RESULT(vkCreateInstance(&instanceCI, nullptr, &m_Instance)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create Instance!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Succesfully created Instance!");

            // Create the debug utils
            setupDebugMessenger();

            if (RZApplication::Get().getAppType() == AppType::kGame) {
                CreateSurface((GLFWwindow*) m_Window->GetNativeWindow());
            }
        }

        std::vector<cstr> VKContext::getRequiredLayers() const
        {
            std::vector<cstr> layers;
            if (m_EnabledValidationLayer) {
                layers.emplace_back(VK_LAYER_KHRONOS_VALIDATION_NAME);
            }
            return layers;
        }

        std::vector<cstr> VKContext::getRequiredExtensions() const
        {
            // First we are sending in the list of desired extensions by GLFW to interface with the WPI
            u32   glfwExtensionsCount = 0;
            cstr* glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
            RAZIX_CORE_TRACE("[Vulkan] GLFW loaded extensions count : {0}", glfwExtensionsCount);

            // This is just for information and Querying purpose
    #ifdef RAZIX_DEBUG
            // Get the total list of supported Extension by Vulkan
            u32 supportedExtensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
            std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

            RAZIX_CORE_TRACE("Available Instance Extensions:\n");
            for (const auto& extension: supportedExtensions) {
                RAZIX_CORE_TRACE("\t {0} \n", extension.extensionName);
            }

            RAZIX_CORE_TRACE("GLFW Requested Extensions are : \n");
            for (u32 i = 0; i < glfwExtensionsCount; i++) {
                RAZIX_CORE_TRACE("\t");
                int j = 0;
                while (*(glfwExtensions[i] + j) != 0) {
                    std::cout << *(glfwExtensions[i] + j);
                    j++;
                }
                std::cout << std::endl;
            }
    #endif

            // Bundle all the required extensions into a vector and return it
            std::vector<cstr> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
    #ifdef RAZIX_PLATFORM_WINDOWS
            extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #elif defined RAZIX_PLATFORM_MACOS
            extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif
            extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

            // instance extension
            extensions.push_back("VK_EXT_debug_report");

            // Add any custom extension from the list of supported extensions that you need and are not included by GLFW
            if (m_EnabledValidationLayer)
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            return extensions;
        }

        void VKContext::setupDebugMessenger()
        {
            if (CreateDebugUtilsMessengerEXT(m_Instance, &m_DebugCI, nullptr, &m_DebugCallbackHandle) != VK_SUCCESS)
                RAZIX_CORE_ERROR("[Vulkan] Failed to create debug messenger!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Succesfully created debug messenger!");
        }

        void VKContext::CreateSurface(void* window)
        {
            if (glfwCreateWindowSurface(m_Instance, (GLFWwindow*) window, nullptr, &m_Surface))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create surface!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Succesfully created surface!");
        }
    }    // namespace Gfx
}    // namespace Razix
#endif
