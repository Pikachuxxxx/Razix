#include "rzxpch.h"
#include "VKContext.h"

#ifdef RAZIX_RENDER_API_VULKAN

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RazixVersion.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#define VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace Razix {
    namespace Graphics {

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Load the functions dynamically to create the DebugUtilsMessenger
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            else
                return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr)
                func(instance, debugMessenger, pAllocator);
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        VKContext::VKContext(RZWindow* windowHandle) : m_Window(windowHandle) {
            RAZIX_CORE_ASSERT(windowHandle, "[Vulkan] Window Handle is NULL!");
        }

        void VKContext::Init() {
            // Create the Vulkan instance to interface with the Vulkan library
            createInstance();

            // Create the Logical Device
            VKDevice::Get().init();


        }

        void VKContext::Destroy() {
            // Destroy the logical device
            VKDevice::Get().destroy();
            // Destroy the surface
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            // Destroy the debug manager
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugCallbackHandle, nullptr);
            // Destroy the instance at last
            vkDestroyInstance(m_Instance, nullptr);
        }

        void VKContext::createInstance() {

            // Vulkan Application Info
            VkApplicationInfo appInfo{};
            appInfo.sType                       = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName            = RZApplication::Get().getAppName().c_str();
            appInfo.applicationVersion          = VK_MAKE_VERSION(1, 0, 0); // TODO: Add this feature later! once we add it to the Application class
            appInfo.pEngineName                 = "Razix Engine";
            appInfo.engineVersion               = VK_MAKE_VERSION(RazixVersion.getVersionMajor(), RazixVersion.getVersionMinor(), RazixVersion.getVersionPatch());
            appInfo.apiVersion                  = VK_API_VERSION_1_1;

            // Instance Create Info
            VkInstanceCreateInfo instanceCI{};
            instanceCI.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCI.pApplicationInfo         = &appInfo;

            // To track if there is any issue with instance creation we supply the pNext with the `VkDebugUtilsMessengerCreateInfoEXT`
            m_DebugCI = {};
            m_DebugCI.sType                     = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            m_DebugCI.messageSeverity           = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            m_DebugCI.messageType               = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            m_DebugCI.pfnUserCallback           = debugCallback;

            instanceCI.pNext                    = (VkDebugUtilsMessengerCreateInfoEXT*) &m_DebugCI;

            // Get the Required Instance Layers from the app/engine
            m_RequiredInstanceLayerNames        = getRequiredLayers();
            // Get the Required Instance aka Global Extension (also applicable for the device and application)
            m_RequiredInstanceExtensionNames    = getRequiredExtensions();

            // Get the Instance Layers and Extensions
            instanceCI.enabledLayerCount        = static_cast<uint32_t>(m_RequiredInstanceLayerNames.size());
            instanceCI.ppEnabledLayerNames      = m_RequiredInstanceLayerNames.data();
            instanceCI.enabledExtensionCount    = static_cast<uint32_t>(m_RequiredInstanceExtensionNames.size());
            instanceCI.ppEnabledExtensionNames  = m_RequiredInstanceExtensionNames.data();

            if(VK_CHECK_RESULT(vkCreateInstance(&instanceCI, nullptr, &m_Instance)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create Instance!");
            else RAZIX_CORE_TRACE("[Vulkan] Succesfully created Instance!");

            // Create the debug utils 
            setupDebugMessenger();

            // Create the WSI surface
            createSurface((GLFWwindow*)m_Window->GetNativeWindow());
        }

        std::vector<const char*> VKContext::getRequiredLayers() {
            std::vector<const char*> layers;
            if (m_EnabledValidationLayer)
                layers.emplace_back(VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME);
            return layers;
        }

        std::vector<const char*> VKContext::getRequiredExtensions() {

            // First we are sending in the list of desired extensions by GLFW to interface with the WPI
            uint32_t glfwExtensionsCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
            RAZIX_CORE_TRACE("[Vulkan] GLFW loaded extensions count : {0}", glfwExtensionsCount);

            // This is just for information and Querying purpose
#ifdef RAZIX_DEBUGgg
            // Get the total list of supported Extension by Vulkan
            uint32_t supportedExtensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
            std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

            RAZIX_CORE_TRACE("Available Instance Extensions:\n");
            for (const auto& extension : supportedExtensions) {
                RAZIX_CORE_TRACE("\t {0} \n", extension.extensionName);
            }

            RAZIX_CORE_TRACE("GLFW Requested Extensions are : \n");
            for (uint32_t i = 0; i < glfwExtensionsCount; i++) {
                RAZIX_CORE_TRACE("\t");
                int j = 0;
                while (*(glfwExtensions[i] + j) != NULL) {
                    std::cout << *(glfwExtensions[i] + j);
                    j++;
                }
                std::cout << std::endl;
            }
#endif        

            // Bundle all the required extensions into a vector and return it
            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

            // Add any custom extension from the list of supported extensions that you need and are not included by GLFW
            if (m_EnabledValidationLayer)
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            
            return extensions;
        }

        void VKContext::setupDebugMessenger() {
            if (CreateDebugUtilsMessengerEXT(m_Instance, &m_DebugCI, nullptr, &m_DebugCallbackHandle) != VK_SUCCESS)
                RAZIX_CORE_ERROR("[Vulkan] Failed to create debug messenger!");
            else RAZIX_CORE_TRACE("[Vulkan] Succesfully created debug messenger!");
        }

        void VKContext::createSurface(GLFWwindow* window) {
            if (glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create surface!");
            else RAZIX_CORE_TRACE("[Vulkan] Succesfully created surface!");
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL VKContext::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
            // Select prefix depending on flags passed to the callback
            // Note that multiple flags may be set for a single validation message
            // Error that may result in undefined behavior
            // TODO: Add option to choose minimum severity level and use <=> to select levels
            // TODO: Formate the message id and stuff for colors etc

            if (!message_severity)
                return VK_FALSE;

            if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
                std::cout << "\033[1;31m ***************************************************************** \033[0m" << std::endl;
                std::cout << "\033[1;32m[VULKAN] \033[1;31m - Validation ERROR : \033[0m \nmessage ID : " << callback_data->messageIdNumber << "\nID Name : " << callback_data->pMessageIdName << "\nMessage : " << callback_data->pMessage << std::endl;
                std::cout << "\033[1;31m ***************************************************************** \033[0m" << std::endl;
            };
            // Warnings may hint at unexpected / non-spec API usage
            if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                std::cout << "\033[1;33m ***************************************************************** \033[0m" << std::endl;
                std::cout << "\033[1;32m[VULKAN] \033[1;33m - Validation WARNING : \033[0m \nmessage ID : " << callback_data->messageIdNumber << "\nID Name : " << callback_data->pMessageIdName << "\nMessage : " << callback_data->pMessage << std::endl;
                std::cout << "\033[1;33m ***************************************************************** \033[0m" << std::endl;
            };
            // Informal messages that may become handy during debugging
            if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
                std::cout << "\033[1;36m ***************************************************************** \033[0m" << std::endl;
                std::cout << "\033[1;32m[VULKAN] \033[1;36m - Validation INFO : \033[0m \nmessage ID : " << callback_data->messageIdNumber << "\nID Name : " << callback_data->pMessageIdName << "\nMessage : " << callback_data->pMessage << std::endl;
                std::cout << "\033[1;36m ***************************************************************** \033[0m" << std::endl;
            }
            // Diagnostic info from the Vulkan loader and layers
            // Usually not helpful in terms of API usage, but may help to debug layer and loader problems
            // if(message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            // {
            //     std::cout << "\033[1;35m*****************************************************************" << std::endl;
            //     std::cout << "\033[1;32m[VULKAN] \033[1;35m - DEBUG : \033[0m \nmessage ID : " << callback_data->messageIdNumber << "\nID Name : " << callback_data->pMessageIdName << "\nMessage : " << callback_data->pMessage  << std::endl;
            //     std::cout << "\033[1;35m*****************************************************************" << std::endl;
            // }

            return VK_FALSE;
        }
    }
}
#endif