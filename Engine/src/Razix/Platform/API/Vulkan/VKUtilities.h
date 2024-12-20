#pragma once

#include <vulkan/vulkan.h>

#include "Razix/Gfx/RHI/API/RZTexture.h"

#include <glm/glm.hpp>
#include <spirv_reflect.h>

#define VK_CHECK_RESULT(x) VK_ERROR_REPORT(x)

#define VK_ERROR_REPORT(x) Razix::Gfx::VKUtilities::VulkanCheckErrorStatus(x, __func__, __FILE__, __LINE__)

namespace Razix {
    namespace Gfx {

        // Forward Declarations for reducing cyclic dependency
        enum class DrawType;
        enum class CullMode;
        enum class PolygonMode;
        enum class BlendOp;
        enum class BlendFactor;
        enum class CompareOp;
        enum class DescriptorType : u32;
        enum ShaderStage : u32;
        enum ImageLayout : u32;
        enum PipelineStage : u32;
        enum MemoryAccessMask : u32;
        class RZBufferLayout;

        namespace VKUtilities {

            // Defines to help with debugging (can be disabled in prod builds)
            // Vulkan Debug Markers

            void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer, const std::string& name, glm::vec4 color);

            void CmdInsertDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer, const std::string& name, glm::vec4 color);

            void CmdEndDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer);

            // Vulkan Tagging
            VkResult CreateDebugObjName(const std::string& name, VkObjectType type, uint64_t handle);

#ifndef RAZIX_DISTRIBUTION

    #define VK_BEGIN_MARKER(cmdBuf, name, labelColor) VKUtilities::CmdBeginDebugUtilsLabelEXT(cmdBuf, name, labelColor);

    #define VK_INSERT_MARKER(cmdBuf, name, labelColor) VKUtilities::CmdInsertDebugUtilsLabelEXT(cmdBuf, name, labelColor);

    #define VK_END_MARKER(cmdBuf) VKUtilities::CmdEndDebugUtilsLabelEXT(cmdBuf);

    #define VK_TAG_OBJECT(name, type, handle) VKUtilities::CreateDebugObjName(name, type, handle);
#else

    #define VK_BEGIN_MARKER(cmdBuf, name, labelColor)

    #define VK_INSERT_MARKER(cmdBuf, name, labelColor)

    #define VK_END_MARKER(cmdBuf)

    #define VK_TAG_OBJECT(name, Type, handle)

#endif
            //-----------------------------------------------------------------------------------
            // VkResult enums and their error descriptions map
            std::unordered_map<VkResult, std::string> ErrorDescriptions = {
                {VK_SUCCESS, "Command successfully completed"},
                {VK_NOT_READY, "A fence or query has not yet completed"},
                {VK_TIMEOUT, "A wait operation has not completed in the specified time"},
                {VK_EVENT_SET, "An event is signaled"},
                {VK_EVENT_RESET, "An event is unsignalled"},
                {VK_INCOMPLETE, "A return array was too small for the result"},
                {VK_SUBOPTIMAL_KHR, "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully."},
                {VK_THREAD_IDLE_KHR, "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call."},
                {VK_THREAD_DONE_KHR, "A deferred operation is not complete but there is no work remaining to assign to additional threads."},
                {VK_OPERATION_DEFERRED_KHR, "A deferred operation was requested and at least some of the work was deferred."},
                {VK_OPERATION_NOT_DEFERRED_KHR, "A deferred operation was requested and no operations were deferred."},
                {VK_PIPELINE_COMPILE_REQUIRED_EXT, "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed."},
                {VK_ERROR_OUT_OF_HOST_MEMORY, "A host memory allocation has failed."},
                {VK_ERROR_OUT_OF_DEVICE_MEMORY, "A device memory allocation has failed."},
                {VK_ERROR_INITIALIZATION_FAILED, "Initialization of an object could not be completed for implementation-specific reasons."},
                {VK_ERROR_DEVICE_LOST, "The logical or physical device has been lost. See Lost Device"},
                {VK_ERROR_MEMORY_MAP_FAILED, "Mapping of a memory object has failed."},
                {VK_ERROR_LAYER_NOT_PRESENT, "A requested layer is not present or could not be loaded."},
                {VK_ERROR_EXTENSION_NOT_PRESENT, "A requested extension is not supported."},
                {VK_ERROR_FEATURE_NOT_PRESENT, "A requested feature is not supported."},
                {VK_ERROR_INCOMPATIBLE_DRIVER, "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons."},
                {VK_ERROR_TOO_MANY_OBJECTS, "Too many objects of the type have already been created."},
                {VK_ERROR_FORMAT_NOT_SUPPORTED, "A requested format is not supported on this device."},
                {VK_ERROR_FRAGMENTED_POOL, "A pool allocation has failed due to fragmentation of the pool's memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation."},
                {VK_ERROR_SURFACE_LOST_KHR, "A surface is no longer available."},
                {VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again."},
                {VK_ERROR_OUT_OF_DATE_KHR, "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface."},
                {VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image."},
                {VK_ERROR_INVALID_SHADER_NV, "One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled."},
                {VK_ERROR_OUT_OF_POOL_MEMORY, "A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead."},
                {VK_ERROR_INVALID_EXTERNAL_HANDLE, "An external handle is not a valid handle of the specified type."},
                {VK_ERROR_FRAGMENTATION, "A descriptor pool creation has failed due to fragmentation."},
                {VK_ERROR_INVALID_DEVICE_ADDRESS_EXT, "A buffer creation failed because the requested address is not available."},
                {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "A buffer creation or memory allocation failed because the requested address is not available. A shader group handle assignment failed because the requested shader group handle information is no longer valid."},
                {VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application's control."},
                {VK_ERROR_UNKNOWN, "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred."}};

            bool VulkanCheckErrorStatus(VkResult x, cstr function, cstr file, int line)
            {
                if (x != VK_SUCCESS) {
                    //std::cout << "\033[1;31;49m **Vulkan Function Call Error** Description : \033[0m" << ErrorDescriptions[x] << " \033[2;90;49m [at Line : " << line << " in File : " << file << "\033[0m]" << std::endl;
                    RAZIX_CORE_ERROR("[Vulkan] VKResult Error :: Description : {0} (by Function : {1} at Line : {2} in File : {3})", ErrorDescriptions[x], function, line, file);
                    //RAZIX_CORE_ERROR("[Vulkan] VKResult Error :: (by Function : {0} at Line : {1} in File : {2})", function, line, file);
                    return true;
                } else
                    return false;
            }

            //-----------------------------------------------------------------------------------
            // CPU to GPU staging copy utilities
            /**
             * Copies data from CPU to GPU only visible buffer using a staging buffer
             */
            void CopyDataToGPUBufferResource(const void* cpuData, VkBuffer gpuBuffer, u32 size, u32 srcOffset = 0, u32 dstOffset = 0);
            void CopyDataToGPUTextureResource(const void* cpuData, VkImage gpuTexture, u32 width, u32 height, u64 size, u32 mipLevel = 0, u32 layersCount = 1, u32 baseArrayLayer = 0);

            //-----------------------------------------------------------------------------------
            // Texture/Image utility Functions
            //-----------------------------------------------------------------------------------

            void CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG);
#if RAZIX_USE_VMA
            void CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VmaAllocation& vmaAllocation, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG);
#endif

            VkImageView CreateImageView(VkImage image, VkFormat format, u32 mipLevels, VkImageViewType viewType, VkImageAspectFlags aspectMask, u32 layerCount, u32 baseArrayLayer = 0, u32 baseMipLevel = 0 RZ_DEBUG_NAME_TAG_E_ARG RZ_DEBUG_NAME_TAG_STR_S_ARG(= "someImageView! NAME IT !!! LAZY ASS MF#$"));

            VkSampler CreateImageSampler(VkFilter magFilter = VK_FILTER_LINEAR, VkFilter minFilter = VK_FILTER_LINEAR, f32 minLod = 0.0f, f32 maxLod = 1.0f, bool anisotropyEnable = false, f32 maxAnisotropy = 1.0f, VkSamplerAddressMode modeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE RZ_DEBUG_NAME_TAG_E_ARG RZ_DEBUG_NAME_TAG_STR_S_ARG(= "someImageSampler! NAME IT !!! LAZY ASS MF#$"));

            void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, u32 mipLevels, u32 layers);

            void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mipLevels = 1, u32 layerCount = 1);

            VkFormat TextureFormatToVK(const TextureFormat format, bool srgb = false);

            VkSamplerAddressMode TextureWrapToVK(const Wrapping wrap);

            VkImageType TextureTypeToVK(const TextureType type);

            VkImageViewType TextureTypeToVKViewType(const TextureType type);

            VkFilter TextureFilterToVK(const Filtering::Mode filter);

            u32 EngineImageLayoutToVK(ImageLayout layout);

            u32 EngineMemoryAcsessMaskToVK(MemoryAccessMask mask);

            u32 EnginePipelineStageToVK(PipelineStage ppstage);

            //-----------------------------------------------------------------------------------
            // Single Time Command Buffer utility functions
            //-----------------------------------------------------------------------------------

            VkCommandBuffer BeginSingleTimeCommandBuffer(const std::string commandUsage, glm::vec4 color);
            /* Ends the recording of the single time command buffer, submits and waits until execution is done */
            void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer);

            //-----------------------------------------------------------------------------------
            // Format Utility
            //-----------------------------------------------------------------------------------
            VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
            VkFormat FindDepthFormat();

            //-----------------------------------------------------------------------------------
            // Enum Conversions
            //-----------------------------------------------------------------------------------

            // PipelineInfo

            VkPrimitiveTopology DrawTypeToVK(Razix::Gfx::DrawType type);

            VkCullModeFlags CullModeToVK(Razix::Gfx::CullMode cullMode);

            VkPolygonMode PolygoneModeToVK(Razix::Gfx::PolygonMode polygonMode);

            VkBlendOp BlendOpToVK(Razix::Gfx::BlendOp blendOp);

            VkBlendFactor BlendFactorToVK(Razix::Gfx::BlendFactor blendFactor);

            VkCompareOp CompareOpToVK(Razix::Gfx::CompareOp compareOp);

            VkDescriptorType DescriptorTypeToVK(Razix::Gfx::DescriptorType descriptorType);

            VkShaderStageFlagBits ShaderStageToVK(Razix::Gfx::ShaderStage stage);

            DescriptorType VKToEngineDescriptorType(SpvReflectDescriptorType type);

            u32 GetStrideFromVulkanFormat(VkFormat format);

            u32 PushBufferLayout(VkFormat format, const std::string& name, RZBufferLayout& layout);

        }    // namespace VKUtilities
    }        // namespace Gfx
}    // namespace Razix
