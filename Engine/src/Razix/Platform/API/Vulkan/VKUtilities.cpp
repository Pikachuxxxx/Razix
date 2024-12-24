// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKUtilities.h"

#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"

#include "Razix/Utilities/RZColorUtilities.h"

#include <spirv_reflect.h>

namespace Razix {
    namespace Gfx {
        namespace VKUtilities {

            void CopyDataToGPUBufferResource(const void* cpuData, VkBuffer gpuBuffer, u32 size, u32 srcOffset, u32 dstOffset)
            {
                /**
                * For anything else we copy using a staging buffer to copy to the GPU
                */
                VKBuffer transferBuffer = VKBuffer(BufferUsage::Staging, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, cpuData RZ_DEBUG_NAME_TAG_STR_E_ARG("Staging buffer to copy to Device only GPU buffer"));
                {
                    // 1.1 Copy from staging buffer to Image
                    VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer("Copy Data to GPU Buffer", glm::vec4(0.9f, 0.76f, 0.54f, 1.0f));

                    VkBufferCopy region = {};
                    region.srcOffset    = 0;
                    region.dstOffset    = 0;
                    region.size         = size;

                    vkCmdCopyBuffer(commandBuffer, transferBuffer.getBuffer(), gpuBuffer, 1, &region);

                    VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
                }
                transferBuffer.destroy();
            }

            void CopyDataToGPUTextureResource(const void* cpuData, VkImage gpuTexture, u32 width, u32 height, u64 size, u32 mipLevel /*= 0*/, u32 layersCount /*= 1*/, u32 baseArrayLayer /*= 0*/)
            {
                // Create a Staging buffer (Transfer from source) to transfer texture data from HOST memory to DEVICE memory
                VKBuffer* stagingBuffer = new VKBuffer(BufferUsage::Staging, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, static_cast<u32>(size), cpuData RZ_DEBUG_NAME_TAG_STR_E_ARG("Staging Buffer for VKTexture"));

                // 1.1 Copy from staging buffer to Image
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer("Copy Data to GPU Texture", glm::vec4(0.3f, 0.16f, 0.74f, 1.0f));

                VkBufferImageCopy region               = {};
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = 0;
                region.bufferImageHeight               = 0;
                region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel       = mipLevel;
                region.imageSubresource.baseArrayLayer = baseArrayLayer;
                region.imageSubresource.layerCount     = layersCount;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = {width, height, 1};

                vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), gpuTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);

                stagingBuffer->destroy();
                delete stagingBuffer;
            }

            //-----------------------------------------------------------------------------------
            // Texture Utility Static Functions
            //-----------------------------------------------------------------------------------

            VKImageHandles CreateImageMemoryHandles(VKCreateImageDesc desc RZ_DEBUG_NAME_TAG_E_ARG)
            {
                VKImageHandles handles = {};

                // We pass the image as reference because we need the memory for it as well
                VkImageCreateInfo imageInfo = {};
                imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.imageType         = desc.imageType;
                imageInfo.extent            = {desc.width, desc.height, desc.depth};
                imageInfo.mipLevels         = desc.mipLevels;
                imageInfo.format            = desc.format;
                imageInfo.tiling            = desc.tiling;
                imageInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
                imageInfo.usage             = desc.usage;
                imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
                imageInfo.arrayLayers       = desc.arrayLayers;
                imageInfo.flags             = desc.flags;
#if RAZIX_USE_VMA

                VmaAllocationInfo allocationInfo{};

                VmaAllocationCreateInfo vmaallocInfo = {};
                // TODO: make this selection smart or customizable by user
                //RZ_TODO("Make this selection smart or customizable by user");
                vmaallocInfo.usage = VMA_MEMORY_USAGE_AUTO;
                // We will almost never read back from GPU, and we always use a staging buffer to copy to GPU, so it's always Device Local
                vmaallocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT |
                                     VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
                vmaallocInfo.requiredFlags = desc.properties;
                //allocate the buffer
                VK_CHECK_RESULT(vmaCreateImage(VKDevice::Get().getVMA(), &imageInfo, &vmaallocInfo, &handles.image, &handles.memoryWrapper.vmaAllocation, &allocationInfo));

                VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_IMAGE, (uint64_t) handles.image);

    #ifdef RAZIX_DEBUG
                vmaSetAllocationName(VKDevice::Get().getVMA(), handles.memoryWrapper.vmaAllocation RZ_DEBUG_E_ARG_NAME.c_str());
    #endif

#else
                // Create the image
                VK_CHECK_RESULT(vkCreateImage(VKDevice::Get().getDevice(), &imageInfo, nullptr, &handles.image));
                VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_IMAGE, (uint64_t) handles.image);

                // Get the memory requirements for the image and allocate memory for it
                VkMemoryRequirements memRequirements;
                vkGetImageMemoryRequirements(VKDevice::Get().getDevice(), handles.image, &memRequirements);

                VkMemoryAllocateInfo allocInfo = {};
                allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize       = memRequirements.size;
                allocInfo.memoryTypeIndex      = VKDevice::Get().getPhysicalDevice().get()->getMemoryTypeIndex(memRequirements.memoryTypeBits, desc.properties);

                VK_CHECK_RESULT(vkAllocateMemory(VKDevice::Get().getDevice(), &allocInfo, nullptr, &handles.memoryWrapper.nativeAllocation));
                // Bind the image memory with the image
                VK_CHECK_RESULT(vkBindImageMemory(VKDevice::Get().getDevice(), handles.image, handles.memoryWrapper.nativeAllocation, 0));

                VK_TAG_OBJECT(bufferName + std::string("Memory"), VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t) handles.memoryWrapper.nativeAllocation);
#endif
                return handles;
            }

            //-----------------------------------------------------------------------------------

            VkImageView CreateImageView(VKCreateImageViewDesc desc RZ_DEBUG_NAME_TAG_E_ARG)
            {
                VkImageViewCreateInfo viewInfo           = {};
                viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image                           = desc.image;
                viewInfo.viewType                        = desc.viewType;
                viewInfo.format                          = desc.format;
                viewInfo.components                      = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
                viewInfo.subresourceRange.aspectMask     = desc.aspectMask;
                viewInfo.subresourceRange.baseMipLevel   = desc.baseMipLevel;
                viewInfo.subresourceRange.levelCount     = VK_REMAINING_MIP_LEVELS;
                viewInfo.subresourceRange.baseArrayLayer = desc.baseArrayLayer;
                viewInfo.subresourceRange.layerCount     = desc.layerCount;

                VkImageView imageView;
                VK_CHECK_RESULT(vkCreateImageView(VKDevice::Get().getDevice(), &viewInfo, nullptr, &imageView));

                VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t) imageView);

                return imageView;
            }

            //-----------------------------------------------------------------------------------

            VkSampler CreateImageSampler(VKCreateSamplerDesc desc RZ_DEBUG_NAME_TAG_E_ARG)
            {
                VkSampler           sampler;
                VkSamplerCreateInfo samplerInfo     = {};
                samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                samplerInfo.magFilter               = desc.magFilter;
                samplerInfo.minFilter               = desc.minFilter;
                samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                samplerInfo.addressModeU            = desc.modeU;
                samplerInfo.addressModeV            = desc.modeV;
                samplerInfo.addressModeW            = desc.modeW;
                samplerInfo.maxAnisotropy           = desc.maxAnisotropy;
                samplerInfo.anisotropyEnable        = desc.anisotropyEnable;
                samplerInfo.unnormalizedCoordinates = VK_FALSE;
                samplerInfo.compareEnable           = VK_TRUE;
                samplerInfo.borderColor             = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
                samplerInfo.mipLodBias              = 0.0f;
                samplerInfo.compareOp               = VK_COMPARE_OP_LESS;
                samplerInfo.minLod                  = desc.minLod;
                samplerInfo.maxLod                  = desc.maxLod;

                VK_CHECK_RESULT(vkCreateSampler(VKDevice::Get().getDevice(), &samplerInfo, nullptr, &sampler));

                VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_SAMPLER, (uint64_t) sampler);

                return sampler;
            }

            //-----------------------------------------------------------------------------------

            void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, u32 mipLevels, u32 layers)
            {
                VkFormatProperties formatProperties;
                vkGetPhysicalDeviceFormatProperties(VKDevice::Get().getGPU(), imageFormat, &formatProperties);

                if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
                    RAZIX_CORE_ERROR("Texture image format does not support linear blitting!");
                }

                VKUtilities::TransitionImageLayout(image, imageFormat, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, layers);

                for (size_t layerIdx = 0; layerIdx < layers; layerIdx++) {
                    VkCommandBuffer      commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer("Generating Mips", Utilities::GenerateHashedColor4(225u));
                    VkImageMemoryBarrier barrier{};
                    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    barrier.image                           = image;
                    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                    barrier.subresourceRange.baseArrayLayer = static_cast<u32>(layerIdx);
                    barrier.subresourceRange.layerCount     = 1;
                    barrier.subresourceRange.levelCount     = 1;

                    int32_t mipWidth  = texWidth;
                    int32_t mipHeight = texHeight;

                    for (u32 i = 1; i < mipLevels; i++) {
                        barrier.subresourceRange.baseMipLevel = i - 1;
                        barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                        barrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                        barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
                        barrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

                        vkCmdPipelineBarrier(commandBuffer,
                            VK_PIPELINE_STAGE_TRANSFER_BIT,
                            VK_PIPELINE_STAGE_TRANSFER_BIT,
                            0,
                            0,
                            nullptr,
                            0,
                            nullptr,
                            1,
                            &barrier);

                        VkImageBlit blit{};
                        blit.srcOffsets[0]                 = {0, 0, 0};
                        blit.srcOffsets[1]                 = {mipWidth, mipHeight, 1};
                        blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                        blit.srcSubresource.mipLevel       = i - 1;
                        blit.srcSubresource.baseArrayLayer = static_cast<u32>(layerIdx);
                        blit.srcSubresource.layerCount     = 1;
                        blit.dstOffsets[0]                 = {0, 0, 0};
                        blit.dstOffsets[1]                 = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
                        blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                        blit.dstSubresource.mipLevel       = i;
                        blit.dstSubresource.baseArrayLayer = static_cast<u32>(layerIdx);
                        blit.dstSubresource.layerCount     = 1;

                        vkCmdBlitImage(commandBuffer,
                            image,
                            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                            image,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            1,
                            &blit,
                            VK_FILTER_LINEAR);

                        barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                        barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                        vkCmdPipelineBarrier(commandBuffer,
                            VK_PIPELINE_STAGE_TRANSFER_BIT,
                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                            0,
                            0,
                            nullptr,
                            0,
                            nullptr,
                            1,
                            &barrier);

                        if (mipWidth > 1)
                            mipWidth /= 2;
                        if (mipHeight > 1)
                            mipHeight /= 2;
                    }

                    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
                    barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    barrier.newLayout                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
                    barrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;

                    vkCmdPipelineBarrier(commandBuffer,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        0,
                        0,
                        nullptr,
                        0,
                        nullptr,
                        1,
                        &barrier);
                    VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
                }
            }

            //-----------------------------------------------------------------------------------
            // Texture Utility Functions
            //-----------------------------------------------------------------------------------

            void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mipLevels /*= 1*/, u32 layerCount /*= 1*/)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                if (newLayout == oldLayout) {
                    RAZIX_CORE_WARN("[Vulkan] SKIPPING...Image Layout Transition... found same old and new layout! Please check your barriers again or ignore this.");
                    return;
                }

                // Begin the buffer since this done for computability with shader pipeline stages we use pipeline barrier to synchronize the transition
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer("Image Layout Transition", glm::vec4(0.25f, 0.5f, 0.75, 1.0f));

                VkImageMemoryBarrier barrier = {};
                barrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.oldLayout            = oldLayout;
                barrier.newLayout            = newLayout;
                barrier.srcQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
                barrier.image                = image;
                if (format >= 124 && format <= 130)    // All possible depth formats
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                else
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                VkPipelineStageFlags sourceStage      = 0;
                VkPipelineStageFlags destinationStage = 0;

                // set up source properties
                if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
                    barrier.srcAccessMask = 0;
                    sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
                    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL) {
                    barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                } else {
                    RAZIX_CORE_WARN("[Vulkan] Unsupported layout transition!");
                }

                // set up destination properties
                if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
                    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_GENERAL) {
                    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                } else {
                    RAZIX_CORE_WARN("[Vulkan] Unsupported layout transition!");
                }

                // Use a pipeline barrier to make sure the transition is done properly
                //vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                // Transition image layout for layer and all mips at once
                for (u32 layer = 0; layer < layerCount; layer++) {
                    for (u32 mip = 0; mip < mipLevels; mip++) {
                        barrier.subresourceRange.baseMipLevel   = mip;
                        barrier.subresourceRange.levelCount     = 1;
                        barrier.subresourceRange.baseArrayLayer = layer;
                        barrier.subresourceRange.layerCount     = 1;
                        // Use a pipeline barrier to make sure the transition is done properly
                        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
                    }
                }

                // End the buffer
                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }

            VkFormat TextureFormatToVK(const TextureFormat format, bool srgb /*= false*/)
            {
                if (srgb) {
                    switch (format) {
                        case TextureFormat::R8:
                            return VK_FORMAT_R8_SRGB;
                            break;
                        case TextureFormat::R32_UINT:
                            return VK_FORMAT_R32_UINT;
                            break;
                        case TextureFormat::R32_INT:
                            return VK_FORMAT_R32_SINT;
                            break;
                        case TextureFormat::R32F:
                            return VK_FORMAT_R32_SFLOAT;
                            break;
                        case TextureFormat::RG8:
                            return VK_FORMAT_R8G8_SRGB;
                            break;
                        case TextureFormat::RG16F:
                            return VK_FORMAT_R16G16_SFLOAT;
                            break;
                        case TextureFormat::RGB8:
                            return VK_FORMAT_R8G8B8_SRGB;
                            break;
                        case TextureFormat::RGBA8:
                            return VK_FORMAT_R8G8B8A8_SRGB;
                            break;
                        case TextureFormat::RGB16:
                            return VK_FORMAT_R16G16B16_SFLOAT;
                            break;
                        case TextureFormat::RGBA16:
                            return VK_FORMAT_R16G16B16A16_SFLOAT;
                            break;
                        case TextureFormat::RGBA16F:
                            return VK_FORMAT_R16G16B16A16_SFLOAT;
                            break;
                        case TextureFormat::RGB32:
                            return VK_FORMAT_R32G32B32_SFLOAT;
                            break;
                        case TextureFormat::RGBA32:
                            return VK_FORMAT_R32G32B32A32_SFLOAT;
                            break;
                        case TextureFormat::RGBA32F:
                            return VK_FORMAT_R32G32B32A32_SFLOAT;
                            break;
                        case TextureFormat::RGB:
                            return VK_FORMAT_R8G8B8_SRGB;
                            break;
                        case TextureFormat::RGBA:
                            return VK_FORMAT_R8G8B8A8_SRGB;
                            break;
                        case TextureFormat::BGRA8_UNORM:
                            return VK_FORMAT_B8G8R8A8_UNORM;
                            break;
                        case TextureFormat::SCREEN:
                            return VK_FORMAT_B8G8R8A8_SRGB;
                            break;
                        case TextureFormat::DEPTH16_UNORM:
                            return VK_FORMAT_D16_UNORM;
                            break;
                        case TextureFormat::DEPTH_STENCIL:
                            return VK_FORMAT_D32_SFLOAT_S8_UINT;
                            break;
                        case TextureFormat::DEPTH32F:
                            return VK_FORMAT_D32_SFLOAT;
                            break;
                        case TextureFormat::NONE:
                            return VK_FORMAT_UNDEFINED;
                            break;
                        default:
                            RAZIX_CORE_WARN("[Texture VK] Unsupported Texture format");
                            return VK_FORMAT_UNDEFINED;
                            break;
                    }
                } else {
                    switch (format) {
                        case TextureFormat::R8:
                            return VK_FORMAT_R8_UNORM;
                            break;
                        case TextureFormat::R32_UINT:
                            return VK_FORMAT_R32_UINT;
                            break;
                        case TextureFormat::R32_INT:
                            return VK_FORMAT_R32_SINT;
                            break;
                        case TextureFormat::R32F:
                            return VK_FORMAT_R32_SFLOAT;
                            break;
                        case TextureFormat::RG8:
                            return VK_FORMAT_R8G8_UINT;
                            break;
                        case TextureFormat::RG16F:
                            return VK_FORMAT_R16G16_SFLOAT;
                            break;
                            ;
                        case TextureFormat::RGB8:
                            return VK_FORMAT_R8G8B8_UNORM;
                            break;
                        case TextureFormat::RGBA8:
                            return VK_FORMAT_R8G8B8A8_UNORM;
                            break;
                        case TextureFormat::RGB16:
                            return VK_FORMAT_R16G16B16_UNORM;
                            break;
                        case TextureFormat::RGBA16:
                            return VK_FORMAT_R16G16B16A16_UNORM;
                            break;
                        case TextureFormat::RGBA16F:
                            return VK_FORMAT_R16G16B16A16_SFLOAT;
                            break;
                        case TextureFormat::RGB32:
                            return VK_FORMAT_R32G32B32_SFLOAT;
                            break;
                        case TextureFormat::RGBA32:
                            return VK_FORMAT_R32G32B32A32_SFLOAT;
                            break;
                        case TextureFormat::RGBA32F:
                            return VK_FORMAT_R32G32B32A32_SFLOAT;
                            break;
                        case TextureFormat::RGB:
                            return VK_FORMAT_R8G8B8_UNORM;
                            break;
                        case TextureFormat::RGBA:
                            return VK_FORMAT_R8G8B8A8_UNORM;
                            break;
                        case TextureFormat::BGRA8_UNORM:
                            return VK_FORMAT_B8G8R8A8_UNORM;
                            break;
                        case TextureFormat::SCREEN:
                            return VK_FORMAT_B8G8R8A8_UNORM;
                            break;
                        case TextureFormat::DEPTH16_UNORM:
                            return VK_FORMAT_D16_UNORM;
                            break;
                        case TextureFormat::DEPTH_STENCIL:
                            return VK_FORMAT_D32_SFLOAT_S8_UINT;
                            break;
                        case TextureFormat::DEPTH32F:
                            return VK_FORMAT_D32_SFLOAT;
                            break;
                        case TextureFormat::NONE:
                            return VK_FORMAT_UNDEFINED;
                            break;
                        default:
                            RAZIX_CORE_WARN("[Texture VK] Unsupported Texture format");
                            return VK_FORMAT_UNDEFINED;
                            break;
                    }
                }
            }

            VkSamplerAddressMode TextureWrapToVK(const Wrapping wrap)
            {
                switch (wrap) {
                    case Wrapping::kRepeat:
                        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
                        break;
                    case Wrapping::kMirroredRepeat:
                        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
                        break;
                    case Wrapping::kClampToEdge:
                        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                        break;
                    case Wrapping::kClampToBorder:
                        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
                        break;
                    default:
                        RAZIX_CORE_WARN("[Texture] Unsupported Wrap Mode");
                        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                        break;
                }
            }

            VkImageType TextureTypeToVK(const TextureType type)
            {
                switch (type) {
                    case TextureType::k1D:
                    case TextureType::kRW1D:
                        return VK_IMAGE_TYPE_1D;
                    case TextureType::k2D:
                    case TextureType::kRW2D:
                    case TextureType::k2DArray:
                    case TextureType::kRW2DArray:
                    case TextureType::kDepth:
                    case TextureType::kCubeMap:
                    case TextureType::kRWCubeMap:
                    case TextureType::kCubeMapArray:
                    case TextureType::kSwapchainImage:
                        return VK_IMAGE_TYPE_2D;
                    case TextureType::k3D:
                    case TextureType::kRW3D:
                        return VK_IMAGE_TYPE_3D;
                    default:
                        RAZIX_CORE_WARN("[Texture] Unsupported Texture Type");
                        return VK_IMAGE_TYPE_2D;
                        break;
                }
            }

            VkImageViewType TextureTypeToVKViewType(const TextureType type)
            {
                switch (type) {
                    case TextureType::k1D:
                    case TextureType::kRW1D:
                        return VK_IMAGE_VIEW_TYPE_1D;
                    case TextureType::k2D:
                    case TextureType::kRW2D:
                        return VK_IMAGE_VIEW_TYPE_2D;
                    case TextureType::k2DArray:
                    case TextureType::kRW2DArray:
                        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                    case TextureType::kDepth:
                        return VK_IMAGE_VIEW_TYPE_2D;
                    case TextureType::kCubeMap:
                    case TextureType::kRWCubeMap:
                        return VK_IMAGE_VIEW_TYPE_CUBE;
                    case TextureType::kCubeMapArray:
                        return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
                    case TextureType::kSwapchainImage:
                        return VK_IMAGE_VIEW_TYPE_2D;
                    case TextureType::k3D:
                    case TextureType::kRW3D:
                        return VK_IMAGE_VIEW_TYPE_3D;
                    default:
                        RAZIX_CORE_WARN("[Texture] Unsupported Texture View Type");
                        return VK_IMAGE_VIEW_TYPE_2D;
                        break;
                }
            }

            VkFilter TextureFilterToVK(const Filtering::Mode filter)
            {
                switch (filter) {
                    case Filtering::Mode::kFilterModeLinear:
                        return VK_FILTER_LINEAR;
                        break;
                    case Filtering::Mode::kFilterModeNearest:
                        return VK_FILTER_NEAREST;
                        break;
                    default:
                        RAZIX_CORE_WARN("[Texture] Unsupported TextureFilter type!");
                        return VK_FILTER_LINEAR;
                }
            }

            u32 EngineImageLayoutToVK(ImageLayout layout)
            {
                switch (layout) {
                    case ImageLayout::kNewlyCreated:
                        return VK_IMAGE_LAYOUT_UNDEFINED;
                        break;
                    case ImageLayout::kGeneric:
                        return VK_IMAGE_LAYOUT_GENERAL;
                        break;
                    case ImageLayout::kSwapchain:
                        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                        break;
                    case ImageLayout::kColorRenderTarget:
                        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                        break;
                    case ImageLayout::kDepthRenderTarget:
                        return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                        break;
                    case ImageLayout::kDepthStencilRenderTarget:
                        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                        break;
                    case ImageLayout::kDepthStencilReadOnly:
                        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                        break;
                    case ImageLayout::kShaderAttachment:
                        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        break;
                    case ImageLayout::kAttachment:
                        return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
                        break;
                    case ImageLayout::kTransferSource:
                        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                        break;
                    case ImageLayout::kTransferDestination:
                        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                        break;
                    default:
                        return VK_IMAGE_LAYOUT_UNDEFINED;
                        break;
                }
            }

            u32 EngineMemoryAcsessMaskToVK(MemoryAccessMask mask)
            {
                switch (mask) {
                    case Razix::Gfx::MemoryAccessMask::kMemoryAccessNone:
                        return VK_ACCESS_NONE;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kIndirectCommandReadBit:
                        return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kIndexBufferDataReadBit:
                        return VK_ACCESS_INDEX_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kVertexAttributeReadBit:
                        return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kUniformReadBit:
                        return VK_ACCESS_UNIFORM_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kInputAttachmentReadBit:
                        return VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kShaderReadBit:
                        return VK_ACCESS_SHADER_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kShaderWriteBit:
                        return VK_ACCESS_SHADER_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kColorAttachmentReadBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kColorAttachmentWriteBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kColorAttachmentReadWriteBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kDepthStencilAttachmentReadBit:
                        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kDepthStencilAttachmentWriteBit:
                        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kTransferReadBit:
                        return VK_ACCESS_TRANSFER_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kTransferWriteBit:
                        return VK_ACCESS_TRANSFER_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kHostReadBit:
                        return VK_ACCESS_HOST_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kHostWriteBit:
                        return VK_ACCESS_HOST_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kMemoryReadBit:
                        return VK_ACCESS_MEMORY_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kMemoryWriteBit:
                        return VK_ACCESS_MEMORY_WRITE_BIT;
                        break;
                    default:
                        return VK_ACCESS_NONE;
                        break;
                }
            }

            u32 EnginePipelineStageToVK(PipelineStage ppstage)
            {
                switch (ppstage) {
                    case Razix::Gfx::PipelineStage::kTopOfPipe:
                        return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kDrawIndirect:
                        return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kDraw:
                        return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kVertexInput:
                        return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kVertexShader:
                        return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kTessellationControlShader:
                        return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kTessellationEvaluationShader:
                        return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kGeometryShader:
                        return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kFragmentShader:
                        return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kEarlyFragmentTests:
                        return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kLateFragmentTests:
                        return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kEarlyOrLateTests:
                        return (VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
                        break;
                    case Razix::Gfx::PipelineStage::kColorAttachmentOutput:
                        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kComputeShader:
                        return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kTransfer:
                        return VK_PIPELINE_STAGE_TRANSFER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kMeshShader:
                        return VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
                        break;
                    case Razix::Gfx::PipelineStage::kTaskShader:
                        return VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT;
                        break;
                    case Razix::Gfx::PipelineStage::kBottomOfPipe:
                        return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                        break;
                    default:
                        return VK_PIPELINE_STAGE_NONE;
                        break;
                }
            }

            //-----------------------------------------------------------------------------------
            // Single Time Command Buffer Utility Functions
            //-----------------------------------------------------------------------------------

            VkCommandBuffer BeginSingleTimeCommandBuffer(const std::string commandUsage, glm::vec4 color)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                VkCommandBufferAllocateInfo allocInfo = {};
                allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                // TODO: Use a dedicated transfer only pool for copy operations, but this is a generic one and we use it for IBL too maybe this if fine.
                allocInfo.commandPool        = VKDevice::Get().getSingleTimeGraphicsCommandPool()->getVKPool();
                allocInfo.commandBufferCount = 1;

                VkCommandBuffer commandBuffer;
                VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().getDevice(), &allocInfo, &commandBuffer));

                VkCommandBufferBeginInfo beginInfo = {};
                beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));

                CmdBeginDebugUtilsLabelEXT(commandBuffer, commandUsage, color);

                return commandBuffer;
            }

            void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                CmdEndDebugUtilsLabelEXT(commandBuffer);

                VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

                VkSubmitInfo submitInfo         = {};
                submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount   = 1;
                submitInfo.pCommandBuffers      = &commandBuffer;
                submitInfo.pSignalSemaphores    = nullptr;
                submitInfo.pNext                = nullptr;
                submitInfo.pWaitDstStageMask    = nullptr;
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.waitSemaphoreCount   = 0;

                VK_CHECK_RESULT(vkQueueSubmit(VKDevice::Get().getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
                VK_CHECK_RESULT(vkQueueWaitIdle(VKDevice::Get().getGraphicsQueue()));

                vkFreeCommandBuffers(VKDevice::Get().getDevice(), VKDevice::Get().getSingleTimeGraphicsCommandPool()->getVKPool(), 1, &commandBuffer);
            }

            //-----------------------------------------------------------------------------------
            // Format Utility
            //-----------------------------------------------------------------------------------

            VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
            {
                for (VkFormat format: candidates) {
                    VkFormatProperties props;
                    vkGetPhysicalDeviceFormatProperties(VKDevice::Get().getGPU(), format, &props);

                    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                        return format;
                    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                        return format;
                    }
                }
                RAZIX_CORE_WARN("Could not find supported format");
                return VK_FORMAT_UNDEFINED;
            }

            VkFormat FindDepthFormat()
            {
                return FindSupportedFormat(
                    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
            }

            //-----------------------------------------------------------------------------------
            // Enum Conversions
            //-----------------------------------------------------------------------------------

            VkPrimitiveTopology DrawTypeToVK(Razix::Gfx::DrawType type)
            {
                switch (type) {
                    case Razix::Gfx::DrawType::Point:
                        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                        break;
                    case Razix::Gfx::DrawType::Triangle:
                        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                        break;
                    case Razix::Gfx::DrawType::Line:
                        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown Draw Type! using triangle list to draw the geometry");
                        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                        break;
                }
            }

            VkCullModeFlags CullModeToVK(Razix::Gfx::CullMode cullMode)
            {
                switch (cullMode) {
                    case Razix::Gfx::CullMode::Front:
                        return VK_CULL_MODE_FRONT_BIT;
                        break;
                    case Razix::Gfx::CullMode::Back:
                        return VK_CULL_MODE_BACK_BIT;
                        break;
                    case Razix::Gfx::CullMode::FrontBack:
                        return VK_CULL_MODE_FRONT_AND_BACK;
                        break;
                    case Razix::Gfx::CullMode::None:
                        return VK_CULL_MODE_NONE;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown Cull Mode! Using Back Face Culling by default");
                        return VK_CULL_MODE_BACK_BIT;
                        break;
                }
            }

            VkPolygonMode PolygoneModeToVK(Razix::Gfx::PolygonMode polygonMode)
            {
                switch (polygonMode) {
                    case Razix::Gfx::PolygonMode::Fill:
                        return VK_POLYGON_MODE_FILL;
                        break;
                    case Razix::Gfx::PolygonMode::Line:
                        return VK_POLYGON_MODE_LINE;
                        break;
                    case Razix::Gfx::PolygonMode::Point:
                        return VK_POLYGON_MODE_POINT;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown polygon mode! Using fill by default");
                        return VK_POLYGON_MODE_FILL;
                        break;
                }
            }

            VkBlendOp BlendOpToVK(Razix::Gfx::BlendOp blendOp)
            {
                switch (blendOp) {
                    case Razix::Gfx::BlendOp::Add:
                        return VK_BLEND_OP_ADD;
                        break;
                    case Razix::Gfx::BlendOp::Subtract:
                        return VK_BLEND_OP_SUBTRACT;
                        break;
                    case Razix::Gfx::BlendOp::ReverseSubtract:
                        return VK_BLEND_OP_REVERSE_SUBTRACT;
                        break;
                    case Razix::Gfx::BlendOp::Min:
                        return VK_BLEND_OP_MIN;
                        break;
                    case Razix::Gfx::BlendOp::Max:
                        return VK_BLEND_OP_MAX;
                        break;
                    default:
                        return VK_BLEND_OP_ADD;
                        break;
                }
                return VK_BLEND_OP_ADD;
            }

            VkBlendFactor BlendFactorToVK(Razix::Gfx::BlendFactor blendFactor)
            {
                switch (blendFactor) {
                    case Razix::Gfx::BlendFactor::Zero:
                        return VK_BLEND_FACTOR_ZERO;
                        break;
                    case Razix::Gfx::BlendFactor::One:
                        return VK_BLEND_FACTOR_ONE;
                        break;
                    case Razix::Gfx::BlendFactor::SrcColor:
                        return VK_BLEND_FACTOR_SRC_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusSrcColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::DstColor:
                        return VK_BLEND_FACTOR_DST_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusDstColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::SrcAlpha:
                        return VK_BLEND_FACTOR_SRC_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusSrcAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::DstAlpha:
                        return VK_BLEND_FACTOR_DST_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusDstAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::ConstantColor:
                        return VK_BLEND_FACTOR_CONSTANT_COLOR;
                        break;
                    case BlendFactor::OneMinusConstantColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::ConstantAlpha:
                        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusConstantAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::SrcAlphaSaturate:
                        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
                        break;
                    default:
                        return VK_BLEND_FACTOR_ONE;
                        break;
                }
                return VK_BLEND_FACTOR_ONE;
            }

            VkCompareOp CompareOpToVK(Razix::Gfx::CompareOp compareOp)
            {
                switch (compareOp) {
                    case Razix::Gfx::CompareOp::Never:
                        return VK_COMPARE_OP_NEVER;
                        break;
                    case Razix::Gfx::CompareOp::Less:
                        return VK_COMPARE_OP_LESS;
                        break;
                    case Razix::Gfx::CompareOp::Equal:
                        return VK_COMPARE_OP_EQUAL;
                        break;
                    case Razix::Gfx::CompareOp::LessOrEqual:
                        return VK_COMPARE_OP_LESS_OR_EQUAL;
                        break;
                    case Razix::Gfx::CompareOp::Greater:
                        return VK_COMPARE_OP_GREATER;
                        break;
                    case Razix::Gfx::CompareOp::NotEqual:
                        return VK_COMPARE_OP_NOT_EQUAL;
                        break;
                    case Razix::Gfx::CompareOp::GreaterOrEqual:
                        return VK_COMPARE_OP_GREATER_OR_EQUAL;
                        break;
                    case Razix::Gfx::CompareOp::Always:
                        return VK_COMPARE_OP_ALWAYS;
                        break;
                    default:
                        return VK_COMPARE_OP_LESS_OR_EQUAL;
                        break;
                }
                return VK_COMPARE_OP_LESS_OR_EQUAL;
            }

            VkShaderStageFlagBits ShaderStageToVK(Razix::Gfx::ShaderStage stage)
            {
                int result = 0;

                if ((stage & Razix::Gfx::ShaderStage::kVertex) == Razix::Gfx::ShaderStage::kVertex)
                    result |= VK_SHADER_STAGE_VERTEX_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kPixel) == Razix::Gfx::ShaderStage::kPixel)
                    result |= VK_SHADER_STAGE_FRAGMENT_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kGeometry) == Razix::Gfx::ShaderStage::kGeometry)
                    result |= VK_SHADER_STAGE_GEOMETRY_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kTesselationControl) == Razix::Gfx::ShaderStage::kTesselationControl)
                    result |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kTesselationEvaluation) == Razix::Gfx::ShaderStage::kTesselationEvaluation)
                    result |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kCompute) == Razix::Gfx::ShaderStage::kCompute)
                    result |= VK_SHADER_STAGE_COMPUTE_BIT;

                return (VkShaderStageFlagBits) result;
            }

            u32 GetStrideFromVulkanFormat(VkFormat format)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                switch (format) {
                    case VK_FORMAT_R8_SINT:
                        return sizeof(int);
                    case VK_FORMAT_R32_SFLOAT:
                        return sizeof(float);
                    case VK_FORMAT_R32G32_SFLOAT:
                        return sizeof(glm::vec2);
                    case VK_FORMAT_R32G32B32_SFLOAT:
                        return sizeof(glm::vec3);
                    case VK_FORMAT_R32G32B32A32_SFLOAT:
                        return sizeof(glm::vec4);
                    case VK_FORMAT_R32G32_SINT:
                        return sizeof(glm::ivec2);
                    case VK_FORMAT_R32G32B32_SINT:
                        return sizeof(glm::ivec3);
                    case VK_FORMAT_R32G32B32A32_SINT:
                        return sizeof(glm::ivec4);
                    case VK_FORMAT_R32G32_UINT:
                        return sizeof(glm::uvec2);
                    case VK_FORMAT_R32G32B32_UINT:
                        return sizeof(glm::uvec3);
                    case VK_FORMAT_R32G32B32A32_UINT:
                        return sizeof(glm::uvec4);    //Need uintvec?
                    case VK_FORMAT_R32_UINT:
                        return sizeof(u32);
                    default:
                        RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                        return 0;
                }

                return 0;
            }

            u32 PushBufferLayout(VkFormat format, const std::string& name, RZBufferLayout& layout)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // TODO: Add buffer layout for all supported types
                switch (format) {
                    case VK_FORMAT_R8_SINT:
                        layout.push<int>(name);
                        break;
                    case VK_FORMAT_R32_SFLOAT:
                        layout.push<float>(name);
                        break;
                    case VK_FORMAT_R32G32_SFLOAT:
                        layout.push<glm::vec2>(name);
                        break;
                    case VK_FORMAT_R32G32B32_SFLOAT:
                        layout.push<glm::vec3>(name);
                        break;
                    case VK_FORMAT_R32G32B32A32_SFLOAT:
                        layout.push<glm::vec4>(name);
                        break;
                    default:
                        RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                        return 0;
                }

                return 0;
            }

            DescriptorType VKToEngineDescriptorType(SpvReflectDescriptorType type)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                switch (type) {
                    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                        return DescriptorType::kImageSamplerCombined;
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                        return DescriptorType::kUniformBuffer;
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                        return DescriptorType::kTexture;
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                        return DescriptorType::kSampler;
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                        return DescriptorType::kRWTexture;
                        break;
                    default:
                        RAZIX_CORE_ERROR("[VULKAN] SpvReflectDescriptorType is not resolved!");
                        return DescriptorType::kNone;
                        break;
                }

                // FIXME: Make this return something like NONE and cause a ASSERT_ERROR
                // return DescriptorType::kUniformBuffer;
                RAZIX_CORE_ERROR("[VULKAN] SpvReflectDescriptorType is not resolved!");
                return DescriptorType::kNone;
            }

            VkDescriptorType DescriptorTypeToVK(Razix::Gfx::DescriptorType descriptorType)
            {
                switch (descriptorType) {
                    case Razix::Gfx::DescriptorType::kUniformBuffer:
                        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;
                    case Razix::Gfx::DescriptorType::kImageSamplerCombined:
                        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;
                    default:
                        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;
                    case Razix::Gfx::DescriptorType::kTexture:
                        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                        break;
                    case Razix::Gfx::DescriptorType::kRWTexture:
                        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                        break;
                    case Razix::Gfx::DescriptorType::kSampler:
                        return VK_DESCRIPTOR_TYPE_SAMPLER;
                    case Razix::Gfx::DescriptorType::kRWTyped:
                    case Razix::Gfx::DescriptorType::kStructured:
                    case Razix::Gfx::DescriptorType::kRWStructured:
                    case Razix::Gfx::DescriptorType::kByteAddress:
                    case Razix::Gfx::DescriptorType::kRWByteAddress:
                    case Razix::Gfx::DescriptorType::kAppendStructured:
                    case Razix::Gfx::DescriptorType::kConsumeStructured:
                    case Razix::Gfx::DescriptorType::kRWStructuredCounter:
                    case Razix::Gfx::DescriptorType::kRTAccelerationStructure:
                        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        break;
                }
            }

            //-----------------------------------------------------------------------------------
            // Debug Utils
            //-----------------------------------------------------------------------------------

            void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer, const std::string& name, glm::vec4 color)
            {
                VkDebugUtilsLabelEXT label{};
                label.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
                label.pLabelName = name.c_str();
                memcpy(label.color, &color[0], 4 * sizeof(f32));

                auto func = (PFN_vkCmdBeginDebugUtilsLabelEXT) vkGetInstanceProcAddr(VKContext::Get()->getInstance(), "vkCmdBeginDebugUtilsLabelEXT");
                if (func != nullptr)
                    func(cmdBuffer, &label);
                //else
                //    RAZIX_CORE_ERROR("CmdBeginDebugUtilsLabelEXT Function not found");
            }

            void CmdInsertDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer, const std::string& name, glm::vec4 color)
            {
                VkDebugUtilsLabelEXT label{};
                label.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
                label.pLabelName = name.c_str();
                memcpy(label.color, &color[0], 4 * sizeof(f32));

                auto func = (PFN_vkCmdInsertDebugUtilsLabelEXT) vkGetInstanceProcAddr(VKContext::Get()->getInstance(), "vkCmdInsertDebugUtilsLabelEXT");
                if (func != nullptr)
                    func(cmdBuffer, &label);
                //else
                //    RAZIX_CORE_ERROR("CmdInsertDebugUtilsLabelEXT Function not found");
            }

            void CmdEndDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer)
            {
                auto func = (PFN_vkCmdEndDebugUtilsLabelEXT) vkGetInstanceProcAddr(VKContext::Get()->getInstance(), "vkCmdEndDebugUtilsLabelEXT");
                if (func != nullptr)
                    func(cmdBuffer);
                //else
                //    RAZIX_CORE_ERROR("CmdEndDebugUtilsLabelEXT Function not found");
            }

            VkResult CreateDebugObjName(const std::string& name, VkObjectType type, uint64_t handle)
            {
                RAZIX_CORE_ASSERT((handle != 0), "NULL HANDLE DETECTED!");

                VkDebugUtilsObjectNameInfoEXT info{};
                info.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
                info.pObjectName  = name.c_str();
                info.objectType   = type;
                info.objectHandle = (uint64_t) handle;

                auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetInstanceProcAddr(VKContext::Get()->getInstance(), "vkSetDebugUtilsObjectNameEXT");
                if (func != nullptr)
                    return func(VKDevice::Get().getDevice(), &info);
                else
                    return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }    // namespace VKUtilities
    }        // namespace Gfx
}    // namespace Razix
