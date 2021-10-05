#pragma once

#ifdef RAZIX_RENDER_API_VULKAN 

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* he actual handle tho the Physical GPU being used to process the application */
        class VKPhysicalDevice
        {
            struct QueueFamilyIndices
            {
                int32_t Graphics = -1;
                int32_t Compute = -1;
                int32_t Transfer = -1;
            };

        public:
            VKPhysicalDevice();
            ~VKPhysicalDevice();

            /* 
             * Checks the Physical Device for the provided list of extension availability
             * 
             * @param extensionName The extension to check for, if it's supported by the GPU
             * 
             * @returns True, if the extension is supported
             */
            bool IsExtensionSupported(const std::string& extensionName) const;
            uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;
            std::string GetPhysicalDeviceTypeString(VkPhysicalDeviceType type) const;

            inline VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }
            inline int32_t GetGraphicsQueueFamilyIndex() const { return m_QueueFamilyIndices.Graphics; }
            inline VkPhysicalDeviceProperties GetProperties() const { return m_PhysicalDeviceProperties; };

        private:
            QueueFamilyIndices                      m_QueueFamilyIndices;
            std::vector<VkQueueFamilyProperties>    m_QueueFamilyProperties;
            std::unordered_set<std::string>         m_SupportedExtensions;
            std::vector<VkDeviceQueueCreateInfo>    m_QueueCreateInfos;
            VkPhysicalDevice                        m_PhysicalDevice;
            VkPhysicalDeviceFeatures                m_Features;
            VkPhysicalDeviceProperties              m_PhysicalDeviceProperties;
            VkPhysicalDeviceMemoryProperties        m_MemoryProperties;

            friend class VKDevice;

        private:
            QueueFamilyIndices GetQueueFamilyIndices(int flags);
        };

        /* The logical device handle */
        class VKDevice
        {

        };
    }
}
#endif