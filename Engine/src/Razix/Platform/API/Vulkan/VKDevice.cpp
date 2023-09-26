// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKDevice.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "VKContext.h"

    #include "Razix/Platform/API/Vulkan/VKUtilities.h"

    #include "Razix/Graphics/Renderers/RZSystemBinding.h"

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
            u32        numGPUs  = 0;
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
            u32 extCount = 0;
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
            u32 queueFamilyCount;
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
            f32               queuePriority       = 1.0f;
            for (u32 queueFamily: uniqueQueueFamilies) {
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

        u32 VKPhysicalDevice::getMemoryTypeIndex(u32 typeBits, VkMemoryPropertyFlags properties) const
        {
            for (u32 i = 0; i < m_MemoryProperties.memoryTypeCount; i++) {
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
            u32 i = 0;
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

            // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_EXT_robustness2.html
            // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDeviceRobustness2FeaturesEXT.html
            // Enable Null Descriptor writes
            VkPhysicalDeviceRobustness2FeaturesEXT robustness2Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT, .pNext = nullptr};
            // Just like DirectX12 this enables passing null descriptors to vkUpdateDescriptorSets and treats as if no descriptors were bound
            robustness2Features.nullDescriptor = VK_TRUE;

            // Query bindless extension, called Descriptor Indexing (https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VK_EXT_descriptor_indexing.html)
            VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES, .pNext = &robustness2Features};
            VkPhysicalDeviceFeatures2                  device_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &indexing_features};
            vkGetPhysicalDeviceFeatures2(getGPU(), &device_features);
            // Check if Bindless feature is supported by the GPU
            // [TAG: BINDLESS]
            // For the feature to be correctly working, we need both the possibility to partially bind a descriptor,
            // as some entries in the bindless array will be empty, and SpirV runtime descriptors.
            m_IsBindlessSupported = indexing_features.descriptorBindingPartiallyBound && indexing_features.runtimeDescriptorArray;

            if (m_IsBindlessSupported) {
                indexing_features.descriptorBindingPartiallyBound = VK_TRUE;
                indexing_features.runtimeDescriptorArray          = VK_TRUE;
            }
            device_features.pNext = &indexing_features;

            // Enable Geometry Shaders
            device_features.features.geometryShader = VK_TRUE;

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
            dynamicRenderingFeatures.pNext            = &device_features;
            dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

            // Device Create Info
            VkDeviceCreateInfo deviceCI{};
            deviceCI.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCI.pNext                   = &dynamicRenderingFeatures;
            deviceCI.queueCreateInfoCount    = static_cast<u32>(m_PhysicalDevice->m_QueueCreateInfos.size());
            deviceCI.pQueueCreateInfos       = m_PhysicalDevice->m_QueueCreateInfos.data();
            deviceCI.enabledExtensionCount   = static_cast<u32>(deviceExtensions.size());
            deviceCI.ppEnabledExtensionNames = deviceExtensions.data();
            deviceCI.enabledLayerCount       = 0;

            if (vkCreateDevice(m_PhysicalDevice->getVulkanPhysicalDevice(), &deviceCI, nullptr, &m_Device) != VK_SUCCESS) {
                RAZIX_CORE_ERROR("[Vulkan] Failed to create logical device!");
                return false;
            } else
                RAZIX_CORE_INFO("[Vulkan] Successfully created logical device!");

            // Get the queue handles using the queue index (we assume that the graphics queue also has presentation capability)
            vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Graphics, 0, &m_GraphicsQueue);
            vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Graphics, 0, &m_PresentQueue);

            //------------------------------------------------------------------------------------------------
            // Create a command pool for single time command buffers
            m_CommandPool = rzstl::CreateRef<VKCommandPool>(m_PhysicalDevice->getGraphicsQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

            //-----------------------------------------------------------------------------------------------
            // Create the Query Pools
            // Create timestamp query pool used for GPU timings.
            VkQueryPoolCreateInfo timestamp_pool_info{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO, nullptr, 0, VK_QUERY_TYPE_TIMESTAMP, k_gpu_time_queries_per_frame * 2u, 0};
            vkCreateQueryPool(m_Device, &timestamp_pool_info, nullptr, &m_timestamp_query_pool);

            // Create pipeline statistics query pool
            VkQueryPoolCreateInfo statistics_pool_info{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO, nullptr, 0, VK_QUERY_TYPE_PIPELINE_STATISTICS, 7, 0};
            statistics_pool_info.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
                                                      VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
                                                      VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
                                                      VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
                                                      VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
                                                      VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT |
                                                      VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT;
            vkCreateQueryPool(m_Device, &statistics_pool_info, nullptr, &m_pipeline_stats_query_pool);

            //------------------------------------------------------------------------------------------------
            // Create the Global Descriptor Pool, used for normal descriptor sets
            VkDescriptorPoolSize pool_sizes[] = {
                {VK_DESCRIPTOR_TYPE_SAMPLER, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, kGLOBAL_MAX_SETS},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, kGLOBAL_MAX_SETS}};
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            u32 size                             = (sizeof(pool_sizes) / sizeof(pool_sizes[0]));
            pool_info.maxSets                    = kGLOBAL_MAX_SETS * size;
            pool_info.poolSizeCount              = (u32) size;
            pool_info.pPoolSizes                 = pool_sizes;
            VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &pool_info, nullptr, &m_GlobalDescriptorPool));

            //------------------------------------------------------------------------------------------------
            // [TAG: BINDLESS]
            // Create the Descriptor Pool used by bindless, that needs update after bind flag.
            if (m_IsBindlessSupported) {
                VkDescriptorPoolSize pool_sizes_bindless[] = {
                    // We use bindless for Images, Storage images and Uniform Buffers for now
                    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, kMAX_BINDLESS_RESOURCES},
                    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, kMAX_BINDLESS_RESOURCES},
                    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, kMAX_BINDLESS_RESOURCES},
                    //{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, kMAX_BINDLESS_RESOURCES},
                    //{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, kMAX_BINDLESS_RESOURCES}
                };

                u32 bindless_pool_count = (sizeof(pool_sizes_bindless) / sizeof(pool_sizes_bindless[0]));
                // Update after bind is needed here, for each binding and in the descriptor set layout creation.
                pool_info.flags         = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;    // BINDLESS MAGIC!
                pool_info.maxSets       = kMAX_BINDLESS_RESOURCES * bindless_pool_count;
                pool_info.poolSizeCount = (u32) bindless_pool_count;
                pool_info.pPoolSizes    = pool_sizes_bindless;
                VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &pool_info, nullptr, &m_BindlessDescriptorPool));

                //------------------------------------------------------------------------------------------------
                std::vector<VkDescriptorSetLayoutBinding> vk_binding(bindless_pool_count);

                // Actual descriptor set layout
                // Images
                VkDescriptorSetLayoutBinding& image_2d_sampler_binding = vk_binding[0];
                image_2d_sampler_binding.descriptorType                = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                image_2d_sampler_binding.descriptorCount               = kMAX_BINDLESS_RESOURCES;
                image_2d_sampler_binding.binding                       = BindingTable_System::BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_2D_BINDING_IDX;
                image_2d_sampler_binding.stageFlags                    = /*VK_SHADER_STAGE_ALL*/ VK_SHADER_STAGE_FRAGMENT_BIT;
                image_2d_sampler_binding.pImmutableSamplers            = nullptr;

                VkDescriptorSetLayoutBinding& image_3d_sampler_binding = vk_binding[1];
                image_3d_sampler_binding.descriptorType                = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                image_3d_sampler_binding.descriptorCount               = kMAX_BINDLESS_RESOURCES;
                image_3d_sampler_binding.binding                       = BindingTable_System::BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_3D_BINDING_IDX;
                image_3d_sampler_binding.stageFlags                    = /*VK_SHADER_STAGE_ALL*/ VK_SHADER_STAGE_FRAGMENT_BIT;
                image_3d_sampler_binding.pImmutableSamplers            = nullptr;

                VkDescriptorSetLayoutBinding& image_cubemap_sampler_binding = vk_binding[2];
                image_cubemap_sampler_binding.descriptorType                = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                image_cubemap_sampler_binding.descriptorCount               = kMAX_BINDLESS_RESOURCES;
                image_cubemap_sampler_binding.binding                       = BindingTable_System::BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_CUBEMAP_BINDING_IDX;
                image_cubemap_sampler_binding.stageFlags                    = /*VK_SHADER_STAGE_ALL*/ VK_SHADER_STAGE_FRAGMENT_BIT;
                image_cubemap_sampler_binding.pImmutableSamplers            = nullptr;
    #if 0
                // Storage Images 
                VkDescriptorSetLayoutBinding& storage_image_binding = vk_binding[1];
                storage_image_binding.descriptorType                = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                storage_image_binding.descriptorCount               = kMAX_BINDLESS_RESOURCES;
                storage_image_binding.binding                       = BindingTable_System::BINDING_IDX_BINDLESS_RESOURCES_START + 1;
                storage_image_binding.stageFlags                    = VK_SHADER_STAGE_ALL;
                storage_image_binding.pImmutableSamplers            = nullptr;

                // Uniform Buffers
                VkDescriptorSetLayoutBinding& storage_image_binding = vk_binding[2];
                storage_image_binding.descriptorType                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                storage_image_binding.descriptorCount               = kMAX_BINDLESS_RESOURCES;
                storage_image_binding.binding                       = BindingTable_System::BINDING_IDX_BINDLESS_RESOURCES_START + 2;
                storage_image_binding.stageFlags                    = VK_SHADER_STAGE_ALL;
                storage_image_binding.pImmutableSamplers            = nullptr;
    #endif

                // Set info
                VkDescriptorSetLayoutCreateInfo layout_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
                layout_info.bindingCount                    = bindless_pool_count;
                layout_info.pBindings                       = vk_binding.data();
                layout_info.flags                           = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

                // To enable the bindless feature we need to pass some params using the pNext field and this is how it's done
                // Binding flags
                VkDescriptorBindingFlags              bindless_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
                std::vector<VkDescriptorBindingFlags> binding_flags(bindless_pool_count);

                for (size_t i = 0; i < bindless_pool_count; i++)
                    binding_flags[i] = bindless_flags;

                VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr};
                extended_info.bindingCount  = bindless_pool_count;
                extended_info.pBindingFlags = binding_flags.data();

                layout_info.pNext = &extended_info;

                VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device, &layout_info, nullptr, &m_BindlessSetLayout));
                VK_TAG_OBJECT("Bindless Set Layout", VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t) m_BindlessSetLayout)
                //------------------------------------------------------------------------------------------------
                // Bindless Descriptor Set
                VkDescriptorSetAllocateInfo alloc_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
                alloc_info.descriptorPool     = m_BindlessDescriptorPool;
                alloc_info.descriptorSetCount = 1;
                alloc_info.pSetLayouts        = &m_BindlessSetLayout;

                VkDescriptorSetVariableDescriptorCountAllocateInfoEXT count_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT};
                u32                                                   max_binding = kMAX_BINDLESS_RESOURCES - 1;
                count_info.descriptorSetCount                                     = 1;
                // This number is the max allocatable count
                count_info.pDescriptorCounts = &max_binding;

                VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device, &alloc_info, &m_BindlessDescriptorSet));
                VK_TAG_OBJECT("Bindless Set", VK_OBJECT_TYPE_DESCRIPTOR_SET, (uint64_t) m_BindlessDescriptorSet)
            }
            return true;
        }

        void VKDevice::destroy()
        {
            vkDestroyDescriptorPool(m_Device, m_GlobalDescriptorPool, nullptr);
            vkDestroyDescriptorPool(m_Device, m_BindlessDescriptorPool, nullptr);
            vkDestroyDescriptorSetLayout(m_Device, m_BindlessSetLayout, nullptr);

            vkDestroyQueryPool(m_Device, m_pipeline_stats_query_pool, nullptr);
            vkDestroyQueryPool(m_Device, m_timestamp_query_pool, nullptr);
            // Destroy the single time Command pool
            vkDestroyCommandPool(m_Device, m_CommandPool->getVKPool(), nullptr);
            // Destroy the logical device
            vkDestroyDevice(m_Device, nullptr);
        }
    }    // namespace Graphics
}    // namespace Razix
#endif