#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "RZSTL/smart_pointers.h"
    #include "Razix/Platform/API/Vulkan/VKCommandPool.h"
    #include "Razix/Utilities/TRZSingleton.h"

    #include <vma/vk_mem_alloc.h>
    #include <vulkan/vulkan.h>

    #define VK_KHR_dynamic_rendering_NAME "VK_KHR_dynamic_rendering"
    #define VK_KHR_push_descriptor_NAME   "VK_KHR_push_descriptor"

// Enable any device specific extensions
// Ex. VK_KHR_RAY_TRACING etc.
static std::vector<cstr> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_dynamic_rendering_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
    VK_KHR_MULTIVIEW_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    //VK_GOOGLE_HLSL_FUNCTIONALITY1_EXTENSION_NAME,
    //VK_GOOGLE_USER_TYPE_EXTENSION_NAME,
    VK_KHR_push_descriptor_NAME,
    // On the fly Push Descriptors similar to PushConstants
    //"VK_NV_device_diagnostic_checkpoints"
};

namespace Razix {
    namespace Graphics {

        // TODO: Move these either to RZGraphicsContext or make a new RZGPUDevice class
        constexpr u32    k_gpu_time_queries_per_frame = 32;   /* Max number of queries that can be made in a frame          */
        static const u32 kGLOBAL_MAX_SETS             = 1024; /* Max number of global descriptor resources                  */
        static const u32 kMAX_BINDLESS_RESOURCES      = 1024; /* Max Bindless resources that can be allocated by the engine */

        /* The actual handle to the Physical GPU being used to process the application */
        class VKPhysicalDevice
        {
        public:
            struct QueueFamilyIndices
            {
                int32_t Graphics = -1;
                int32_t Present  = -1;
                int32_t Compute  = -1;
                //int32_t Transfer = -1; // TODO: To be supported

                bool isComplete()
                {
                    return Graphics > -1 && Present > -1 && Compute > -1;
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

            RAZIX_INLINE VkPhysicalDevice                 getVulkanPhysicalDevice() const { return m_PhysicalDevice; }
            RAZIX_INLINE QueueFamilyIndices               getQueueFamilyIndices() const { return m_QueueFamilyIndices; }
            RAZIX_INLINE int32_t                          getGraphicsQueueFamilyIndex() const { return m_QueueFamilyIndices.Graphics; }
            RAZIX_INLINE int32_t                          getPresentQueueFamilyIndex() const { return m_QueueFamilyIndices.Present; }
            RAZIX_INLINE int32_t                          getComputeQueueFamilyIndex() const { return m_QueueFamilyIndices.Compute; }
            RAZIX_INLINE VkPhysicalDeviceProperties       getProperties() const { return m_PhysicalDeviceProperties; };
            RAZIX_INLINE VkPhysicalDeviceMemoryProperties getMemoryProperties() const { return m_MemoryProperties; }

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
            VKDevice() {}
            ~VKDevice() {}

            bool init();
            void destroy();

            /* Gets the actual physical device GPU adapter */
            RAZIX_INLINE const rzstl::Ref<VKPhysicalDevice>& getPhysicalDevice() const { return m_PhysicalDevice; }
            /* Get the graphics command pool for single time command buffers. DON'T USE THIS FOR FRAME RELATED OPERATIONS. */
            RAZIX_INLINE const rzstl::Ref<VKCommandPool>& getSingleTimeGraphicsCommandPool() const { return m_CommandPool; }
            RAZIX_INLINE VkDevice                         getDevice() const { return m_Device; };
            RAZIX_INLINE VkPhysicalDevice                 getGPU() const { return m_PhysicalDevice->getVulkanPhysicalDevice(); };
            RAZIX_INLINE VkQueue                          getGraphicsQueue() const { return m_GraphicsQueue; };
            RAZIX_INLINE VkQueue                          getPresentQueue() const { return m_PresentQueue; };
            RAZIX_INLINE VkQueue                          getComputeQueue() const { return m_ComputeQueue; };
            RAZIX_INLINE VkQueryPool                      getPipelineStatsQueryPool() const { return m_PipelineStatsQueryPool; }
            RAZIX_INLINE VkDescriptorPool                 getGlobalDescriptorPool() const { return m_GlobalDescriptorPool; }
            RAZIX_INLINE VkDescriptorPool                 getBindlessDescriptorPool() const { return m_BindlessDescriptorPool; }
            RAZIX_INLINE VkDescriptorSet                  getBindlessDescriptorSet() const { return m_BindlessDescriptorSet; }
            RAZIX_INLINE VkDescriptorSetLayout            getBindlessSetLayout() const { return m_BindlessSetLayout; }
            RAZIX_INLINE bool                             isBindlessSupported() const { return m_IsBindlessSupported; }
            /* Gets the Vulkan Memory Allocator */
            RAZIX_INLINE VmaAllocator& getVMA() { return m_VMAllocator; }

        private:
            VkDevice                     m_Device                 = VK_NULL_HANDLE; /* Vulkan handle to abstracted device                                               */
            VkQueue                      m_GraphicsQueue          = VK_NULL_HANDLE; /* GPU queue on which graphics commands are submitted                               */
            VkQueue                      m_PresentQueue           = VK_NULL_HANDLE; /* GPU queue on which presentation commands are submitted                           */
            VkQueue                      m_ComputeQueue           = VK_NULL_HANDLE; /* GPU queue on which compute commands are submitted                                */
            rzstl::Ref<VKPhysicalDevice> m_PhysicalDevice         = {};             /* List of available GPUs on the machine                                            */
            rzstl::Ref<VKCommandPool>    m_CommandPool            = {};             /* Global Command pool from which the command buffers are allocated from            */
            VkDescriptorPool             m_GlobalDescriptorPool   = VK_NULL_HANDLE; /* Global descriptor pool from which normal descriptor sets are allocated from      */
            VkDescriptorPool             m_BindlessDescriptorPool = VK_NULL_HANDLE; /* Global descriptor pool from which bindless descriptor sets are allocated from    */
            VkDescriptorSetLayout        m_BindlessSetLayout      = VK_NULL_HANDLE; /* Global set layout for Bindless descriptor set                                    */
            VkDescriptorSet              m_BindlessDescriptorSet  = VK_NULL_HANDLE; /* Global Bindless descriptor set to which bindless textures are mapped to          */
            VkQueryPool                  m_TimestampsQueryPool    = VK_NULL_HANDLE; /* Query pool for allocating timestamps                                             */
            VkQueryPool                  m_PipelineStatsQueryPool = VK_NULL_HANDLE; /* Query pool for allocating pipeline stats                                         */
            bool                         m_IsBindlessSupported    = false;          /* Whether or not Bindless is supported on the machine                              */
            VmaAllocator                 m_VMAllocator            = VK_NULL_HANDLE; /* Vulkan Memory Allocator for managing GPU heap                                    */
        };
    }    // namespace Graphics
}    // namespace Razix
#endif