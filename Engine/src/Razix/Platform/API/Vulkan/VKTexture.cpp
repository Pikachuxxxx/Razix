// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKTexture.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include "Razix/Utilities/RZLoadImage.h"

#include <vendor/stb/stb_image_write.h>

namespace Razix {
    namespace Gfx {

        //-----------------------------------------------------------------------------------
        // Texture Utility Static Functions
        //-----------------------------------------------------------------------------------

        void VKTexture::CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG)
        {
            // We pass the image as reference because we need the memory for it as well
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType         = imageType;
            imageInfo.extent            = {width, height, depth};
            imageInfo.mipLevels         = mipLevels;
            imageInfo.format            = format;
            imageInfo.tiling            = tiling;
            imageInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage             = usage;
            imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.arrayLayers       = arrayLayers;

            imageInfo.flags = flags;

            // Create the image
            VK_CHECK_RESULT(vkCreateImage(VKDevice::Get().getDevice(), &imageInfo, nullptr, &image));

            // Get the memory requirements for the image and allocate memory for it
            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(VKDevice::Get().getDevice(), image, &memRequirements);

            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize       = memRequirements.size;
            allocInfo.memoryTypeIndex      = VKDevice::Get().getPhysicalDevice().get()->getMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

            VK_CHECK_RESULT(vkAllocateMemory(VKDevice::Get().getDevice(), &allocInfo, nullptr, &imageMemory));
            // Bind the image memory with the image
            VK_CHECK_RESULT(vkBindImageMemory(VKDevice::Get().getDevice(), image, imageMemory, 0));

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_IMAGE, (uint64_t) image);
            VK_TAG_OBJECT(bufferName + std::string("Memory"), VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t) imageMemory);
        }

#if RAZIX_USE_VMA
        void VKTexture::CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VmaAllocation& vmaAllocation, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG)
        {
            // We pass the image as reference because we need the memory for it as well
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType         = imageType;
            imageInfo.extent            = {width, height, depth};
            imageInfo.mipLevels         = mipLevels;
            imageInfo.format            = format;
            imageInfo.tiling            = tiling;
            imageInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage             = usage;
            imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.arrayLayers       = arrayLayers;
            imageInfo.flags             = flags;

            VmaAllocationInfo allocationInfo{};

            VmaAllocationCreateInfo vmaallocInfo = {};
            // TODO: make this selection smart or customizable by user
            //RZ_TODO("Make this selection smart or customizable by user");
            vmaallocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            // We will almost never read back from GPU, and we always use s staging buffer to copy to GPU, so it's always Device Local
            vmaallocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT |
                                 VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
            vmaallocInfo.requiredFlags = properties;
            //allocate the buffer
            VK_CHECK_RESULT(vmaCreateImage(VKDevice::Get().getVMA(), &imageInfo, &vmaallocInfo, &image, &vmaAllocation, &allocationInfo));

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_IMAGE, (uint64_t) image);

    #ifdef RAZIX_DEBUG
            vmaSetAllocationName(VKDevice::Get().getVMA(), vmaAllocation RZ_DEBUG_E_ARG_NAME.c_str());
    #endif

            // TODO: Get allocated memory stats from which pool etc. and attach that to RZMemoryManager
            //Memory::RZMemAllocInfo memAllocInfo{};
        }
#endif

        //-----------------------------------------------------------------------------------

        VkImageView VKTexture::CreateImageView(VkImage image, VkFormat format, u32 mipLevels, VkImageViewType viewType, VkImageAspectFlags aspectMask, u32 layerCount, u32 baseArrayLayer, u32 baseMipLevel /*= 0*/ RZ_DEBUG_NAME_TAG_E_ARG)
        {
            VkImageViewCreateInfo viewInfo           = {};
            viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image                           = image;
            viewInfo.viewType                        = viewType;
            viewInfo.format                          = format;
            viewInfo.components                      = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
            viewInfo.subresourceRange.aspectMask     = aspectMask;
            viewInfo.subresourceRange.baseMipLevel   = baseMipLevel;
            viewInfo.subresourceRange.levelCount     = VK_REMAINING_MIP_LEVELS;
            viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
            viewInfo.subresourceRange.layerCount     = layerCount;

            VkImageView imageView;
            VK_CHECK_RESULT(vkCreateImageView(VKDevice::Get().getDevice(), &viewInfo, nullptr, &imageView));

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t) imageView);

            return imageView;
        }

        //-----------------------------------------------------------------------------------

        VkSampler VKTexture::CreateImageSampler(VkFilter magFilter /*= VK_FILTER_LINEAR*/, VkFilter minFilter /*= VK_FILTER_LINEAR*/, f32 minLod /*= 0.0f*/, f32 maxLod /*= 1.0f*/, bool anisotropyEnable /*= false*/, f32 maxAnisotropy /*= 1.0f*/, VkSamplerAddressMode modeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode modeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode modeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/ RZ_DEBUG_NAME_TAG_E_ARG)
        {
            VkSampler           sampler;
            VkSamplerCreateInfo samplerInfo     = {};
            samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter               = magFilter;
            samplerInfo.minFilter               = minFilter;
            samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.addressModeU            = modeU;
            samplerInfo.addressModeV            = modeV;
            samplerInfo.addressModeW            = modeW;
            samplerInfo.maxAnisotropy           = maxAnisotropy;
            samplerInfo.anisotropyEnable        = anisotropyEnable;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable           = VK_TRUE;
            samplerInfo.borderColor             = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            samplerInfo.mipLodBias              = 0.0f;
            samplerInfo.compareOp               = VK_COMPARE_OP_LESS;
            samplerInfo.minLod                  = minLod;
            samplerInfo.maxLod                  = maxLod;

            VK_CHECK_RESULT(vkCreateSampler(VKDevice::Get().getDevice(), &samplerInfo, nullptr, &sampler));

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_SAMPLER, (uint64_t) sampler);

            return sampler;
        }

        //-----------------------------------------------------------------------------------

        void VKTexture::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, u32 mipLevels, u32 layers /* = 1*/)
        {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(VKDevice::Get().getGPU(), imageFormat, &formatProperties);

            if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
                RAZIX_CORE_ERROR("Texture image format does not support linear blitting!");
            }

            for (size_t layerIdx = 0; layerIdx < layers; layerIdx++) {
                VkCommandBuffer      commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();
                VkImageMemoryBarrier barrier{};
                barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.image                           = image;
                barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseArrayLayer = static_cast<u32>(layerIdx);
                barrier.subresourceRange.layerCount     = layers;
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
                    blit.srcSubresource.layerCount     = layers;
                    blit.dstOffsets[0]                 = {0, 0, 0};
                    blit.dstOffsets[1]                 = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
                    blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                    blit.dstSubresource.mipLevel       = i;
                    blit.dstSubresource.baseArrayLayer = 0;
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
        // VKTexture
        //-----------------------------------------------------------------------------------

        VKTexture::VKTexture(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Desc = desc;

            setResourceViewHints(m_Desc.initResourceViewHints);

            // Build a render target texture here if the data is nullptr
            initializeBackendHandles(m_Desc RZ_DEBUG_E_ARG_NAME);

            //RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0}", m_Desc.name);
        }

        // Special swapchain class helper initializer
        VKTexture::VKTexture(VkImage image, VkImageView imageView)
            : m_Image(image), m_ImageSampler(VK_NULL_HANDLE)
        {
            // This way of creating usually means one this, it's a SWAPCHAIN IMAGE
            m_Desc.type   = TextureType::kSwapchainImage;
            m_Desc.format = TextureFormat::SCREEN;
            m_ImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            // (0, 0) since swapchain has no layers and mips
            m_ResourceViews[0][0]
                .rtv = imageView;
        }

        //-------------------------------------------------------------------------------------------

        RAZIX_CLEANUP_RESOURCE_IMPL(VKTexture)
        {
            if (m_Desc.type == TextureType::kSwapchainImage)
                return;

            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            for (u32 l = 1; l < m_Desc.layers; l++) {
                for (u32 m = 1; m < m_TotalMipLevels; m++) {
                    auto uav = m_ResourceViews[l][m].uav;
                    if (uav != VK_NULL_HANDLE)
                        vkDestroyImageView(VKDevice::Get().getDevice(), uav, nullptr);
                }
            }

#if !RAZIX_USE_VMA
            if (m_Image != VK_NULL_HANDLE)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);
#else
            vmaDestroyImage(VKDevice::Get().getVMA(), m_Image, m_VMAAllocation);
#endif
            m_CurrentMipRenderingLevel = 0;
        }

        //-------------------------------------------------------------------------------------------

        void VKTexture::Resize(u32 width, u32 height)
        {
            m_Desc.width  = width;
            m_Desc.height = height;

            DestroyResource();

            initializeBackendHandles(m_Desc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));
        }

        int32_t VKTexture::ReadPixels(u32 x, u32 y)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Works only on Texture2D type for now!

            VKBuffer m_TransferBuffer = VKBuffer(BufferUsage::ReadBack, VK_BUFFER_USAGE_TRANSFER_DST_BIT, m_Desc.width * m_Desc.height * RAZIX_TEXTURE_BITS_PER_PIXEL, nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("Transfer RT Buffer"));

            // Change the image layout from shader read only optimal to transfer source
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            {
                // 1.1 Copy from staging buffer to Image
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                // TODO: Support layers and depth
                VkBufferImageCopy region               = {};
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = m_Desc.width;
                region.bufferImageHeight               = m_Desc.width;
                region.imageSubresource.aspectMask     = m_AspectBit;
                region.imageSubresource.mipLevel       = m_CurrentMipRenderingLevel;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = {m_Desc.width, m_Desc.height, 1};

                vkCmdCopyImageToBuffer(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_TransferBuffer.getBuffer(), 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            void* data = new char[m_Desc.width * m_Desc.height * RAZIX_TEXTURE_BITS_PER_PIXEL];
            m_TransferBuffer.map();
            data = m_TransferBuffer.getMappedRegion();
            // Read and return the pixel value
            int32_t pixel_value = ((int32_t*) data)[(x) + (m_Desc.width * y)];
            m_TransferBuffer.unMap();
            delete[] data;

            return pixel_value;
        }

        void VKTexture::GenerateMips()
        {
            m_Desc.enableMips = true;
            m_TotalMipLevels  = static_cast<u32>(std::floor(std::log2(std::max(m_Desc.width, m_Desc.height)))) + 1;

            VKTexture::GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_Desc.width, m_Desc.height, m_TotalMipLevels);
        }

        void VKTexture::UploadToBindlessSet()
        {
            // Now if Bindless is available Bind to it
            if (VKDevice::Get().isBindlessSupported()) {
                RZDescriptor descriptor = {};

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = m_ImageLayout;
                imageInfo.imageView   = getSRVImageView();
                // No combined image samplers

                u32 bindingIdx = BindingTable_System::BINDING_IDX_BINDLESS_RESOURCES_START;

                switch (m_Desc.type) {
                    case TextureType::k2D:
                    case TextureType::k2DArray:
                        bindingIdx = BindingTable_System::BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_2D_BINDING_IDX;
                        break;
                    case TextureType::k3D:
                        bindingIdx = BindingTable_System::BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_3D_BINDING_IDX;
                        break;
                    case TextureType::kCubeMap:
                    case TextureType::kCubeMapArray:
                        bindingIdx = BindingTable_System::BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_CUBEMAP_BINDING_IDX;
                        break;
                    default:
                        break;
                }

                VkWriteDescriptorSet writeDescriptorSet{};
                writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.dstSet          = VKDevice::Get().getBindlessDescriptorSet();
                writeDescriptorSet.descriptorType  = VKUtilities::DescriptorTypeToVK(DescriptorType::kTexture);    // for R/W write texture this will be a UAV, if a RWCubeMap
                writeDescriptorSet.dstBinding      = bindingIdx;
                writeDescriptorSet.dstArrayElement = getHandle().getIndex();    // RZTexturePool index is allocated to this as the binding index in the global_texture_xxxyyy_array
                writeDescriptorSet.pImageInfo      = &imageInfo;
                writeDescriptorSet.descriptorCount = 1;    // Single Texture array

                vkUpdateDescriptorSets(VKDevice::Get().getDevice(), 1, &writeDescriptorSet, 0, nullptr);
            }
        }

        void VKTexture::transitonImageLayoutToSRV()
        {
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_ImageLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_TotalMipLevels, m_Desc.layers);
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        void VKTexture::transitonImageLayoutToUAV()
        {
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_ImageLayout, VK_IMAGE_LAYOUT_GENERAL, m_TotalMipLevels, m_Desc.layers);
            m_ImageLayout = VK_IMAGE_LAYOUT_GENERAL;
        }

        //-------------------------------------------------------------------------------------------

        void VKTexture::initializeBackendHandles(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            if (!desc.filePath.empty())
                loadImageDataFromFile();

            RAZIX_ASSERT_MESSAGE(desc.width && desc.height, "[VULKAN] cannot create texture with null width/height.");

            m_TotalMipLevels = 1;
            if (m_Desc.enableMips)
                m_TotalMipLevels = RZTexture::CalculateMipMapCount(desc.width, desc.height);

            VkImageUsageFlagBits usageBit = {};
            if (m_Desc.format == TextureFormat::DEPTH32F || m_Desc.format == TextureFormat::DEPTH16_UNORM || m_Desc.format == TextureFormat::DEPTH_STENCIL)
                usageBit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usageBit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            VkImageCreateFlags flags{0};
            if (desc.type == TextureType::kCubeMap || desc.type == TextureType::kCubeMapArray || desc.type == TextureType::kRWCubeMap)
                flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            // https://www.reddit.com/r/vulkan/comments/pc87in/whats_the_correct_way_to_create_a_2d_image_array/
            // TODO: Investigate if this is necessary and enable it later
            // VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT

            // if the texture is a RW resource we add an extra flag to enable writing to it from shaders, we also check if it can be a UAV, did the user specify the enable flag
            if ((m_ResourceViewHint & kUAV) == kUAV) usageBit = VkImageUsageFlagBits(VK_IMAGE_USAGE_STORAGE_BIT | usageBit);

// Create the Vulkan Image and it's memory and Bind them together
// We use a simple optimal tiling options
#if !RAZIX_USE_VMA
            VKTexture::CreateImage(m_Desc.width, m_Desc.height, m_Desc.depth, m_TotalMipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VKUtilities::TextureTypeToVK(m_Desc.type), VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, m_Desc.layers, flags RZ_DEBUG_E_ARG_NAME);
#else
            VKTexture::CreateImage(m_Desc.width, m_Desc.height, m_Desc.depth, m_TotalMipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VKUtilities::TextureTypeToVK(m_Desc.type), VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_VMAAllocation, m_Desc.layers, flags RZ_DEBUG_E_ARG_NAME);
#endif

            //  There are two transitions we need to handle:
            //      1. Undefined -> transfer destination: transfer writes that don't need to wait on anything
            //          1.1 Copy image from transfer staging buffer to the Image buffer on DEVICE
            //      2. Transfer destination -> shader reading: shader reads should wait on transfer writes, specifically the shader reads in the fragment shader, because that's where we're going to use the texture

            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_TotalMipLevels, m_Desc.layers);

            // Copy the data to GPU, if there is some
            if (desc.data)
                VKUtilities::CopyDataToGPUTextureResource(m_Desc.data, m_Image, m_Desc.width, m_Desc.height, m_Desc.size, m_CurrentMipRenderingLevel, m_Desc.layers, m_BaseArrayLayer);

            delete desc.data;

            if (m_Desc.enableMips)
                VKTexture::GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_Desc.width, m_Desc.height, m_TotalMipLevels, m_Desc.layers);

            // Create the Image view for the Vulkan image (uses color bit)
            if (desc.format == TextureFormat::DEPTH32F || desc.format == TextureFormat::DEPTH16_UNORM || m_Desc.format == TextureFormat::DEPTH_STENCIL) {
                m_Desc.filtering = {Filtering::Mode::kFilterModeNearest, Filtering::Mode::kFilterModeNearest},
                m_AspectBit      = VK_IMAGE_ASPECT_DEPTH_BIT;
            } else
                m_AspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            // We handle a special case here for RWTextureCube, we create both a SRV and UAV with different settings
            if (desc.type == TextureType::kRWCubeMap) {
                if ((m_ResourceViewHint & kSRV) == kSRV)
                    m_ResourceViews[RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER][RAZIX_TEXTURE_DEFAULT_MIP_IDX].srv = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(TextureType::kCubeMap), m_AspectBit, desc.layers, 0, 0 RZ_DEBUG_E_ARG_NAME);
                if ((m_ResourceViewHint & kUAV) == kUAV)
                    m_ResourceViews[RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER][RAZIX_TEXTURE_DEFAULT_MIP_IDX].uav = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(TextureType::kRW2DArray), m_AspectBit, desc.layers, 0, 0 RZ_DEBUG_E_ARG_NAME);

            } else {
                // Create the Image view at the 0 layer and 0 mip as a Shader Resource View default
                if ((m_ResourceViewHint & kSRV) == kSRV)
                    m_ResourceViews[RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER][RAZIX_TEXTURE_DEFAULT_MIP_IDX].srv = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(desc.type), m_AspectBit, desc.layers, 0, 0 RZ_DEBUG_E_ARG_NAME);
                if ((m_ResourceViewHint & kUAV) == kUAV)
                    m_ResourceViews[RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER][RAZIX_TEXTURE_DEFAULT_MIP_IDX].uav = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(desc.type), m_AspectBit, desc.layers, 0, 0 RZ_DEBUG_E_ARG_NAME);
            }

            if (m_Desc.enableMips) {
                for (u32 l = 1; l < m_Desc.layers; l++) {
                    for (u32 m = 1; m < m_TotalMipLevels; m++) {
                        auto imageView = VKTexture::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(desc.type), m_AspectBit, desc.layers, l, m);
                        if ((m_ResourceViewHint & kSRV) == kSRV)
                            m_ResourceViews[l][m].srv = imageView;
                        if ((m_ResourceViewHint & kUAV) == kUAV)
                            m_ResourceViews[l][m].uav = imageView;
                    }
                }
            }

            ////////////////////////////////////////
            // DEPRECATED, soon to be removed!
            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(m_TotalMipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);
            ////////////////////////////////////////

            VkImageLayout finalLayout = (m_ResourceViewHint & kUAV) == kUAV ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, finalLayout, m_TotalMipLevels, desc.layers);
            m_ImageLayout = finalLayout;
        }

        void VKTexture::loadImageDataFromFile()
        {
            if (m_Desc.filePath.empty())
                return;

            if (m_Desc.dataSize == sizeof(float))
                m_Desc.data = (u8*) Razix::Utilities::LoadImageDataFloat(m_Desc.filePath, &m_Desc.width, &m_Desc.height, &m_BitsPerPixel);
            else
                m_Desc.data = Razix::Utilities::LoadImageData(m_Desc.filePath, &m_Desc.width, &m_Desc.height, &m_BitsPerPixel, m_Desc.flipY);
            // Here the format for the texture is extracted based on bits per pixel
            m_Desc.format = Razix::Gfx::RZTexture::BitsToTextureFormat(RAZIX_TEXTURE_BITS_PER_PIXEL);    // everything is a 4-byte by default
            m_Desc.size   = static_cast<u64>(m_Desc.width * m_Desc.height * RAZIX_TEXTURE_BITS_PER_PIXEL * m_Desc.dataSize);
        }

        //-----------------------------------------------------------------------------------
    }    // namespace Gfx
}    // namespace Razix
