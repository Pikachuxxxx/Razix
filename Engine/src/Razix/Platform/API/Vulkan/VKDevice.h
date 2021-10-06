#pragma once

#ifdef RAZIX_RENDER_API_VULKAN 

#include "Razix/Core/SmartPointers.h"
#include "Razix/Utilities/TRazixSingleton.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* The actual handle to the Physical GPU being used to process the application */
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

            bool isDeviceSuitable(VkPhysicalDevice gpu);
            /* 
             * Checks the Physical Device for the provided list of extension availability
             * 
             * @param extensionName The extension to check for, if it's supported by the GPU
             * @returns True, if the extension is supported
             */
            bool isExtensionSupported(const std::string& extensionName) const;
            uint32_t getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;
            std::string getPhysicalDeviceTypeString(VkPhysicalDeviceType type) const;

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
        class VKDevice : public TRazixSingleton<VKDevice>
        {
        public:
            VKDevice();
            ~VKDevice();

            bool Init();
            void Destroy();

            VkDevice GetDevice() const { return m_Device; };
            VkPhysicalDevice GetGPU() const { return m_PhysicalDevice->GetVulkanPhysicalDevice(); };
            const UniqueRef<VKPhysicalDevice>& GetPhysicalDevice() const { return m_PhysicalDevice; }
            VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; };
            VkQueue GetPresentQueue() const { return m_PresentQueue; };

        private:
            VkDevice                    m_Device;
            VkQueue                     m_GraphicsQueue;
            VkQueue                     m_PresentQueue;
            VkPipelineCache             m_PipelineCache;
            VkDescriptorPool            m_DescriptorPool;
            VkPhysicalDeviceFeatures    m_EnabledFeatures;
            UniqueRef<VKPhysicalDevice> m_PhysicalDevice;

        };
    }
}
#endif