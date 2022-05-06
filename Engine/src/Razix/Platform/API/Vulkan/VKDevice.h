#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Core/RZSmartPointers.h"
    #include "Razix/Platform/API/Vulkan/VKCommandPool.h"
    #include "Razix/Utilities/TRZSingleton.h"

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* The actual handle to the Physical GPU being used to process the application */
        class VKPhysicalDevice
        {
        public:
            struct QueueFamilyIndices
            {
                int32_t Graphics = -1;
                int32_t Present  = -1;

                bool isComplete()
                {
                    return Graphics > -1 && Present > -1;
                }
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

            uint32_t    getMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;
            std::string getPhysicalDeviceTypeString(VkPhysicalDeviceType type) const;

            inline VkPhysicalDevice           getVulkanPhysicalDevice() const { return m_PhysicalDevice; }
            inline QueueFamilyIndices         getQueueFamilyIndices() const { return m_QueueFamilyIndices; }
            inline int32_t                    getGraphicsQueueFamilyIndex() const { return m_QueueFamilyIndices.Graphics; }
            inline int32_t                    getPresentQueueFamilyIndex() const { return m_QueueFamilyIndices.Present; }
            inline VkPhysicalDeviceProperties getProperties() const { return m_PhysicalDeviceProperties; };

        private:
            QueueFamilyIndices                   m_QueueFamilyIndices;
            std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
            std::unordered_set<std::string>      m_SupportedExtensions;
            std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;
            VkPhysicalDevice                     m_PhysicalDevice;
            VkPhysicalDeviceFeatures             m_Features;
            VkPhysicalDeviceProperties           m_PhysicalDeviceProperties;
            VkPhysicalDeviceMemoryProperties     m_MemoryProperties;

            friend class VKDevice;

        private:
            void findQueueFamilyIndices(VkSurfaceKHR surface);
        };

        /* The logical device handle */
        //TODO: Add all get sets methods of physical GPU to VKDevice to reduce coupling and call routing/code complexity, this purely done to improve readability
        class VKDevice : public RZSingleton<VKDevice>
        {
        public:
            VKDevice();
            ~VKDevice();

            bool init();
            void destroy();

            VkDevice                     getDevice() const { return m_Device; };
            VkPhysicalDevice             getGPU() const { return m_PhysicalDevice->getVulkanPhysicalDevice(); };
            const Ref<VKPhysicalDevice>& getPhysicalDevice() const { return m_PhysicalDevice; }
            VkQueue                      getGraphicsQueue() const { return m_GraphicsQueue; };
            VkQueue                      getPresentQueue() const { return m_PresentQueue; };
            const Ref<VKCommandPool>&    getCommandPool() const { return m_CommandPool; }

        private:
            VkDevice                 m_Device;
            VkQueue                  m_GraphicsQueue;
            VkQueue                  m_PresentQueue;
            VkPipelineCache          m_PipelineCache;
            VkDescriptorPool         m_DescriptorPool;
            VkPhysicalDeviceFeatures m_EnabledFeatures;
            Ref<VKPhysicalDevice>    m_PhysicalDevice;
            Ref<VKCommandPool>       m_CommandPool;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif