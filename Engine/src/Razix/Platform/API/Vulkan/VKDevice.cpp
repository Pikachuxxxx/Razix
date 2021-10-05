#include "rzxpch.h"
#include "VKDevice.h"

#ifdef RAZIX_RENDER_API_VULKAN 


namespace Razix {
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Physical Device
        //-----------------------------------------------------------------------------------

        VKPhysicalDevice::VKPhysicalDevice() {

            // Query the number of GPUs available
            uint32_t numGPUs = 0;

        }

        VKPhysicalDevice::~VKPhysicalDevice() {

        }

        bool VKPhysicalDevice::IsExtensionSupported(const std::string& extensionName) const {
            return true;
        }

        uint32_t VKPhysicalDevice::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const {
            return 0;
        }

        std::string VKPhysicalDevice::GetPhysicalDeviceTypeString(VkPhysicalDeviceType type) const {
            switch (type) {
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:             return "OTHER";
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:    return "INTEGRATED GPU";
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:      return "DISCRETE GPU";
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:       return "VIRTUAL GPU";
                case VK_PHYSICAL_DEVICE_TYPE_CPU:               return "CPU";
                default:                                        return "UNKNOWN";
            }
        }

        Razix::Graphics::VKPhysicalDevice::QueueFamilyIndices VKPhysicalDevice::GetQueueFamilyIndices(int flags) {
            QueueFamilyIndices indices;
            return indices;
        }

        //-----------------------------------------------------------------------------------
        // Logical Device
        //-----------------------------------------------------------------------------------
    }
}

#endif