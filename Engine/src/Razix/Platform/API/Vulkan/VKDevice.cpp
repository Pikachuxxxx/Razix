// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKDevice.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "VKContext.h"

namespace Razix {
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Physical Device
        //-----------------------------------------------------------------------------------
        // TODO: Decouple the constructor into separate functions

        VKPhysicalDevice::VKPhysicalDevice()
            : m_PhysicalDevice(VK_NULL_HANDLE)
        {
            // Query the number of GPUs available
            uint32_t   numGPUs  = 0;
            VkInstance instance = VKContext::Get()->getInstance();
            vkEnumeratePhysicalDevices(instance, &numGPUs, nullptr);
            RAZIX_CORE_ASSERT(!(numGPUs == 0), "[Vulkan] No Suitable GPUs found!");
            RAZIX_CORE_INFO("[Vulkan] GPUs found         : {0}", numGPUs);
            // Now that we know the number of available GPUs get their list
            std::vector<VkPhysicalDevice> physicalDevices(numGPUs);
            vkEnumeratePhysicalDevices(instance, &numGPUs, physicalDevices.data());

            // Select the best GPU (select it if it's the only one)
            for (VkPhysicalDevice gpu: physicalDevices) {
                if (isDeviceSuitable(gpu) || numGPUs == 1) {
                    m_PhysicalDevice = gpu;
                    break;
                }
            }
            RAZIX_CORE_ASSERT(!(m_PhysicalDevice == VK_NULL_HANDLE), "[Vulkan] No GPU is selected!");

            // Get the memory properties
            vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);

            // Print the GPU details
            RAZIX_CORE_INFO("[Vulkan] Vulkan API Version : {0}.{1}.{2}", VK_VERSION_MAJOR(m_PhysicalDeviceProperties.apiVersion), VK_VERSION_MINOR(m_PhysicalDeviceProperties.apiVersion), VK_VERSION_PATCH(m_PhysicalDeviceProperties.apiVersion));
            RAZIX_CORE_INFO("[Vulkan] GPU Name           : {0}", std::string(m_PhysicalDeviceProperties.deviceName));
            RAZIX_CORE_INFO("[Vulkan] Vendor ID          : {0}", std::to_string(m_PhysicalDeviceProperties.vendorID));
            RAZIX_CORE_INFO("[Vulkan] Device Type        : {0}", std::string(getPhysicalDeviceTypeString(m_PhysicalDeviceProperties.deviceType)));
            RAZIX_CORE_INFO("[Vulkan] Driver Version     : {0}.{1}.{2}", VK_VERSION_MAJOR(m_PhysicalDeviceProperties.driverVersion), VK_VERSION_MINOR(m_PhysicalDeviceProperties.driverVersion), VK_VERSION_PATCH(m_PhysicalDeviceProperties.driverVersion));

            // Verify the supported device extension supported by the GPU
            uint32_t extCount = 0;
            vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extCount, nullptr);
            if (extCount > 0) {
                std::vector<VkExtensionProperties> extensions(extCount);
                if (vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS) {
                    RAZIX_CORE_TRACE("Selected physical device has {0} extensions", extensions.size());
                    for (const auto& ext: extensions) {
                        m_SupportedExtensions.emplace(ext.extensionName);
                        RAZIX_CORE_TRACE("  {0}", ext.extensionName);
                    }
                }
            }

            // Query Queue information + store it for the Physical Device
            uint32_t queueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
            m_QueueFamilyProperties.resize(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

            // Queue families
            // Desired queues need to be requested upon logical device creation
            // Due to differing queue family configurations of Vulkan implementations this can be a bit tricky, especially if the application
            // requests different queue types
            // Get queue family indices for the requested queue family types
            // Note that the indices may overlap depending on the implementation
            findQueueFamilyIndices(VKContext::Get()->getSurface());

            //! BUG: I guess in Distribution mode the set has 2 elements or something is happening such that the queue priority for other element is nan and not 0 as we have provided
            std::set<int32_t> uniqueQueueFamilies = {m_QueueFamilyIndices.Graphics, m_QueueFamilyIndices.Present};
            float             queuePriority       = 1.0f;
            for (uint32_t queueFamily: uniqueQueueFamilies) {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount       = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                m_QueueCreateInfos.push_back(queueCreateInfo);
            }
        }

        VKPhysicalDevice::~VKPhysicalDevice()
        {
        }

        bool VKPhysicalDevice::isDeviceSuitable(VkPhysicalDevice gpu)
        {
            vkGetPhysicalDeviceProperties(gpu, &m_PhysicalDeviceProperties);
            // See if it's a Discrete GPU if so use it, also save the device features while checking it's compatibility
            // TODO: Add a scoring mechanism for the GPU rating
            if (m_PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                return true;
            }
            return false;
        }

        bool VKPhysicalDevice::isExtensionSupported(const std::string& extensionName) const
        {
            return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
        }

        uint32_t VKPhysicalDevice::getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const
        {
            for (uint32_t i = 0; i < m_MemoryProperties.memoryTypeCount; i++) {
                if ((typeBits & 1) == 1) {
                    if ((m_MemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
                        return i;
                }
                typeBits >>= 1;
            }

            RAZIX_CORE_ASSERT(false, "[Vulkan] Could not find a suitable memory type!");
            return UINT32_MAX;
        }

        std::string VKPhysicalDevice::getPhysicalDeviceTypeString(VkPhysicalDeviceType type) const
        {
            switch (type) {
                case VK_PHYSICAL_DEVICE_TYPE_OTHER: return "OTHER";
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "INTEGRATED GPU";
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "DISCRETE GPU";
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "VIRTUAL GPU";
                case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU";
                default: return "UNKNOWN";
            }
        }

        void VKPhysicalDevice::findQueueFamilyIndices(VkSurfaceKHR surface)
        {
            // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
            uint32_t i = 0;
            for (const auto& queue: m_QueueFamilyProperties) {
                if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    m_QueueFamilyIndices.Graphics = i;

                // Check for presentation support
                VkBool32 presentationSupported = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, surface, &presentationSupported);

                if (presentationSupported)
                    m_QueueFamilyIndices.Present = i;

                if (m_QueueFamilyIndices.isComplete())
                    break;

                i++;
            }
        }

        //-----------------------------------------------------------------------------------
        // Logical Device
        //-----------------------------------------------------------------------------------

        VKDevice::VKDevice()
        {
        }

        VKDevice::~VKDevice()
        {
        }

        bool VKDevice::init()
        {
            // Create the Physical device
            m_PhysicalDevice = rzstl::CreateRef<VKPhysicalDevice>();

            // Create the Logical device
            // Get the device features of the selected GPU and Enable whatever features we need
            VkPhysicalDeviceFeatures physicalDeviceFeatures;
            vkGetPhysicalDeviceFeatures(m_PhysicalDevice->getVulkanPhysicalDevice(), &physicalDeviceFeatures);
            physicalDeviceFeatures.geometryShader = VK_TRUE;

            if (m_PhysicalDevice->isExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
                deviceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            if (m_PhysicalDevice->isExtensionSupported("VK_EXT_debug_marker"))
                deviceExtensions.push_back("VK_EXT_debug_marker");

    #if defined(RAZIX_PLATFORM_MACOS)
            // https://vulkan.lunarg.com/doc/view/1.2.162.0/mac/1.2-extensions/vkspec.html#VUID-VkDeviceCreateInfo-pProperties-04451
            if (m_PhysicalDevice->isExtensionSupported("VK_KHR_portability_subset")) {
                deviceExtensions.push_back("VK_KHR_portability_subset");
            }
    #endif

            // Enable Dynamic Rendering
            VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures{};
            dynamicRenderingFeatures.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
            dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

            // Device Create Info
            VkDeviceCreateInfo deviceCI{};
            deviceCI.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCI.pNext                   = &dynamicRenderingFeatures;
            deviceCI.queueCreateInfoCount    = static_cast<uint32_t>(m_PhysicalDevice->m_QueueCreateInfos.size());
            deviceCI.pQueueCreateInfos       = m_PhysicalDevice->m_QueueCreateInfos.data();
            deviceCI.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
            deviceCI.ppEnabledExtensionNames = deviceExtensions.data();
            deviceCI.pEnabledFeatures        = &physicalDeviceFeatures;
            deviceCI.enabledLayerCount       = 0;

            if (vkCreateDevice(m_PhysicalDevice->getVulkanPhysicalDevice(), &deviceCI, nullptr, &m_Device) != VK_SUCCESS) {
                RAZIX_CORE_ERROR("[Vulkan] Failed to create logical device!");
                return false;
            } else
                RAZIX_CORE_TRACE("[Vulkan] Successfully created logical device!");

            // Get the queue handles using the queue index (we assume that the graphics queue also has presentation capability)
            vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Graphics, 0, &m_GraphicsQueue);
            vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Graphics, 0, &m_PresentQueue);

            // Create a command pool for single time command buffers
            m_CommandPool = rzstl::CreateRef<VKCommandPool>(m_PhysicalDevice->getGraphicsQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

            return true;
        }

        void VKDevice::destroy()
        {
            // Destroy the single time Command pool
            vkDestroyCommandPool(m_Device, m_CommandPool->getVKPool(), nullptr);
            // Destroy the logical device
            vkDestroyDevice(m_Device, nullptr);
        }
    }    // namespace Graphics
}    // namespace Razix

#endif