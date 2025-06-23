#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Core/RZSTL/smart_pointers.h"
    #include "Razix/Platform/API/Vulkan/VKCommandPool.h"
    #include "Razix/Utilities/TRZSingleton.h"

    #if RAZIX_USE_VMA
        #include <vma/vk_mem_alloc.h>
    #endif
    #include <vulkan/vulkan.h>

// Enable any device specific extensions
// Ex. VK_KHR_RAY_TRACING etc.
static std::vector<cstr> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
    VK_KHR_MULTIVIEW_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
    #ifdef __APPLE__
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    #endif
    //VK_GOOGLE_HLSL_FUNCTIONALITY1_EXTENSION_NAME,
    //VK_GOOGLE_USER_TYPE_EXTENSION_NAME,
    // On the fly Push Descriptors similar to PushConstants
    //"VK_NV_device_diagnostic_checkpoints"
};

namespace Razix {
    namespace Gfx {

        // TODO: Move these either to RZGraphicsContext or make a new RZGPUDevice class and fix on a writing style
        constexpr u32    k_gpu_time_queries_per_frame = 32;   /* Max number of queries that can be made in a frame          */
        static const u32 kGLOBAL_MAX_SETS             = 1024; /* Max number of global descriptor resources                  */
        static const u32 kMAX_BINDLESS_RESOURCES      = 1024; /* Max Bindless resources that can be allocated by the engine */

        /* The actual handle to the Physical GPU being used to process the application */
        class VKPhysicalDevice
        {
        public:
            struct QueueFamilyIndices
            {
                int32_t Graphics     = -1;    // + Compute
                int32_t Present      = -1;
                int32_t AsyncCompute = -1;
                int32_t Transfer     = -1;

                bool isComplete() const
                {
                    return Graphics > -1 && Present > -1 && AsyncCompute > -1 && Transfer > -1;
                }
            };

        public:
            VKPhysicalDevice();
            ~VKPhysicalDevice();

            bool        isDeviceSuitable(VkPhysicalDevice gpu);
            bool        isExtensionSupported(const std::string& extensionName) const;
            u32         getMemoryTypeIndex(u32 typeBits, VkMemoryPropertyFlags properties) const;
            std::string getPhysicalDeviceTypeString(VkPhysicalDeviceType type) const;

            RAZIX_INLINE VkPhysicalDevice                 getVulkanPhysicalDevice() const { return m_PhysicalDevice; }
            RAZIX_INLINE QueueFamilyIndices               getQueueFamilyIndices() const { return m_QueueFamilyIndices; }
            RAZIX_INLINE int32_t                          getGraphicsQueueFamilyIndex() const { return m_QueueFamilyIndices.Graphics; }
            RAZIX_INLINE int32_t                          getPresentQueueFamilyIndex() const { return m_QueueFamilyIndices.Present; }
            RAZIX_INLINE int32_t                          getAsyncComputeQueueFamilyIndex() const { return m_QueueFamilyIndices.AsyncCompute; }
            RAZIX_INLINE int32_t                          getTransferQueueFamilyIndex() const { return m_QueueFamilyIndices.Transfer; }
            RAZIX_INLINE VkPhysicalDeviceProperties       getProperties() const { return m_PhysicalDeviceProperties; };
            RAZIX_INLINE VkPhysicalDeviceFeatures         getFeatures() const { return m_PhysicalDeviceFeatures; };
            RAZIX_INLINE VkPhysicalDeviceMemoryProperties getMemoryProperties() const { return m_MemoryProperties; }

        private:
            QueueFamilyIndices                   m_QueueFamilyIndices;
            std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
            std::unordered_set<std::string>      m_SupportedExtensions;
            std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;
            VkPhysicalDevice                     m_PhysicalDevice;
            VkPhysicalDeviceFeatures             m_PhysicalDeviceFeatures;
            VkPhysicalDeviceProperties           m_PhysicalDeviceProperties;
            VkPhysicalDeviceMemoryProperties     m_MemoryProperties;

            friend class VKDevice;

        private:
            void findQueueFamilyIndices(VkSurfaceKHR surface);
        };

        struct DummyVKResources
        {
            VkBuffer       dummyBuffer       = VK_NULL_HANDLE;
            VkDeviceMemory dummyBufferMemory = VK_NULL_HANDLE;

            VkImage        dummyImage       = VK_NULL_HANDLE;
            VkDeviceMemory dummyImageMemory = VK_NULL_HANDLE;
            VkImageView    dummyImageView   = VK_NULL_HANDLE;

            VkSampler dummySampler = VK_NULL_HANDLE;

            void create(VkDevice device, VkPhysicalDevice physicalDevice);
            void destroy(VkDevice device) const;
        };

        /* The logical device handle */
        //TODO: Add all get sets methods of physical GPU to VKDevice to reduce coupling and call routing/code complexity, this purely done to improve readability
        class VKDevice : public RZSingleton<VKDevice>
        {
        public:
            VKDevice() {}
            ~VKDevice() {}

            bool init();
            void destroy();

            inline const rzstl::Ref<VKPhysicalDevice>& getPhysicalDevice() const { return m_PhysicalDevice; }
            inline const rzstl::Ref<VKCommandPool>&    getSingleTimeGraphicsCommandPool() const { return m_CommandPool; }
            inline VkDevice                            getDevice() const { return m_Device; };
            inline VkPhysicalDevice                    getGPU() const { return m_PhysicalDevice->getVulkanPhysicalDevice(); };
            inline VkQueue                             getGraphicsQueue() const { return m_GraphicsQueue; };
            inline VkQueue                             getPresentQueue() const { return m_PresentQueue; };
            inline VkQueue                             getAsyncComputeQueue() const { return m_AsyncComputeQueue; };
            inline VkQueue                             getTransferQueue() const { return m_TransferQueue; };
            inline VkQueryPool                         getPipelineStatsQueryPool() const { return m_PipelineStatsQueryPool; }
            inline VkDescriptorPool                    getGlobalDescriptorPool() const { return m_GlobalDescriptorPool; }
            inline VkDescriptorPool                    getBindlessDescriptorPool() const { return m_BindlessDescriptorPool; }
            inline VkDescriptorSet                     getBindlessDescriptorSet() const { return m_BindlessDescriptorSet; }
            inline VkDescriptorSetLayout               getBindlessSetLayout() const { return m_BindlessSetLayout; }
            inline VkPhysicalDeviceProperties2         getGPUProperties2() const { return m_PhysicalDeviceProperties2; };
            inline VkBuffer                            GetDummyBuffer() const { return m_DummyResources.dummyBuffer; }
            inline VkImageView                         GetDummyImageView() const { return m_DummyResources.dummyImageView; }
            inline VkSampler                           GetDummySampler() const { return m_DummyResources.dummySampler; }
    #if RAZIX_USE_VMA
            inline VmaAllocator& getVMA() { return m_VMAllocator; }
    #endif

        private:
            VkDevice                     m_Device                    = VK_NULL_HANDLE;
            VkQueue                      m_GraphicsQueue             = VK_NULL_HANDLE;
            VkQueue                      m_PresentQueue              = VK_NULL_HANDLE;
            VkQueue                      m_AsyncComputeQueue         = VK_NULL_HANDLE;
            VkQueue                      m_TransferQueue             = VK_NULL_HANDLE;
            rzstl::Ref<VKPhysicalDevice> m_PhysicalDevice            = {};
            rzstl::Ref<VKCommandPool>    m_CommandPool               = {};
            VkDescriptorPool             m_GlobalDescriptorPool      = VK_NULL_HANDLE;
            VkDescriptorPool             m_BindlessDescriptorPool    = VK_NULL_HANDLE;
            VkDescriptorSetLayout        m_BindlessSetLayout         = VK_NULL_HANDLE;
            VkDescriptorSet              m_BindlessDescriptorSet     = VK_NULL_HANDLE;
            VkQueryPool                  m_TimestampsQueryPool       = VK_NULL_HANDLE;
            VkQueryPool                  m_PipelineStatsQueryPool    = VK_NULL_HANDLE;
            bool                         m_IsBindlessSupported       = false;
            VkPhysicalDeviceProperties2  m_PhysicalDeviceProperties2 = {};
            DummyVKResources             m_DummyResources            = {};
    #if RAZIX_USE_VMA
            VmaAllocator m_VMAllocator = VK_NULL_HANDLE;
    #endif
        private:
            void createDummyResources();
            void destroyDummyResources();
        };
    }    // namespace Gfx
}    // namespace Razix
#endif
