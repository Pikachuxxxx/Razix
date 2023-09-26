#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Core/RZSmartPointers.h"
    #include "Razix/Platform/API/Vulkan/VKCommandPool.h"
    #include "Razix/Utilities/TRZSingleton.h"

    #include <vulkan/vulkan.h>

    #define VK_KHR_dynamic_rendering_NAME "VK_KHR_dynamic_rendering"

// Enable any device specific extensions
// Ex. VK_KHR_RAY_TRACING etc.
static std::vector<cstr> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_dynamic_rendering_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
    VK_KHR_MULTIVIEW_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
    "VK_KHR_push_descriptor" // On the fly Push Descriptors similar to PushConstants
    //"VK_NV_device_diagnostic_checkpoints"
};

namespace Razix {
    namespace Graphics {

        // TODO: Move these either to RZGraphicsContext or make a new RZGPUDevice class
        constexpr u32    k_gpu_time_queries_per_frame = 32;   /* Max number of queries that can be made in a frame          */
        static const u32 kGLOBAL_MAX_SETS             = 1024; /* Max number of global descriptor resources                  */
        static const u32 kMAX_BINDLESS_RESOURCES      = 1024; /* Max Bindless resources that can be allocated by the engine */

        /* The actual handle to the Physical GPU being used to process the application */
        class VKPhysicalDevice : public RZRoot
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

            u32         getMemoryTypeIndex(u32 typeBits, VkMemoryPropertyFlags properties) const;
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

            inline VkDevice                            getDevice() const { return m_Device; };
            inline VkPhysicalDevice                    getGPU() const { return m_PhysicalDevice->getVulkanPhysicalDevice(); };
            inline const rzstl::Ref<VKPhysicalDevice>& getPhysicalDevice() const { return m_PhysicalDevice; }
            inline VkQueue                             getGraphicsQueue() const { return m_GraphicsQueue; };
            inline VkQueue                             getPresentQueue() const { return m_PresentQueue; };
            inline const rzstl::Ref<VKCommandPool>&    getCommandPool() const { return m_CommandPool; }
            inline VkQueryPool                         getPipelineStatsQueryPool() const { return m_pipeline_stats_query_pool; }
            inline VkDescriptorPool                    getGlobalDescriptorPool() const { return m_GlobalDescriptorPool; }
            inline VkDescriptorPool                    getBindlessDescriptorPool() const { return m_BindlessDescriptorPool; }
            inline VkDescriptorSet                     getBindlessDescriptorSet() const { return m_BindlessDescriptorSet; }
            inline VkDescriptorSetLayout               getBindlessSetLayout() const { return m_BindlessSetLayout; }
            inline bool                                isBindlessSupported() const { return m_IsBindlessSupported; }

        private:
            VkDevice                     m_Device;
            VkQueue                      m_GraphicsQueue;
            VkQueue                      m_PresentQueue;
            VkPipelineCache              m_PipelineCache;
            VkPhysicalDeviceFeatures     m_EnabledFeatures;
            rzstl::Ref<VKPhysicalDevice> m_PhysicalDevice;
            rzstl::Ref<VKCommandPool>    m_CommandPool;            /* Global Command pool from which the command buffers are allocated from           */
            VkDescriptorPool             m_GlobalDescriptorPool;   /* Global descriptor pool from which normal descriptor sets are allocated from     */
            VkDescriptorPool             m_BindlessDescriptorPool; /* Global descriptor pool from which bindless descriptor sets are allocated from   */
            VkDescriptorSetLayout        m_BindlessSetLayout;
            VkDescriptorSet              m_BindlessDescriptorSet; /* Global Bindless descriptor set to which bindless textures are mapped to */
            VkQueryPool                  m_timestamp_query_pool      = VK_NULL_HANDLE;
            VkQueryPool                  m_pipeline_stats_query_pool = VK_NULL_HANDLE;
            bool                         m_IsBindlessSupported       = false;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif