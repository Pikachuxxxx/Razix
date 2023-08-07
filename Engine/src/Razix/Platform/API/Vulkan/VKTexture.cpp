// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKTexture.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include "Razix/Utilities/LoadImage.h"

#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <vendor/stb/stb_image_write.h>

namespace Razix {
    namespace Graphics {

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
            samplerInfo.compareEnable           = VK_FALSE;
            samplerInfo.borderColor             = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            samplerInfo.mipLodBias              = 0.0f;
            samplerInfo.compareOp               = VK_COMPARE_OP_NEVER;
            samplerInfo.minLod                  = minLod;
            samplerInfo.maxLod                  = maxLod;

            VK_CHECK_RESULT(vkCreateSampler(VKDevice::Get().getDevice(), &samplerInfo, nullptr, &sampler));

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_SAMPLER, (uint64_t) sampler);

            return sampler;
        }

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
#if 1
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
#endif

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

#if 1
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
#endif

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

            // Build a render target texture here if the data is nullptr
            if (desc.data == nullptr) {
                m_TotalMipLevels = 1;
                if (desc.enableMips)
                    m_TotalMipLevels = static_cast<u32>(std::floor(std::log2(std::max(desc.width, desc.height)))) + 1;

                VkImageUsageFlagBits usageBit{};
                if (desc.format == RZTextureProperties::Format::DEPTH32F || desc.format == RZTextureProperties::Format::DEPTH16_UNORM || desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                    usageBit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                else
                    usageBit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

                // Create the Vulkan Image and it's memory and Bind them together
                // We use a simple optimal tiling options
                VKTexture::CreateImage(desc.width, desc.height, desc.depth, m_TotalMipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VKUtilities::TextureTypeToVK(desc.type), VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, desc.layers, 0 RZ_DEBUG_E_ARG_NAME);

                VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_TotalMipLevels, desc.layers);

                if (m_Desc.enableMips)
                    VKTexture::GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_Desc.width, m_Desc.height, m_TotalMipLevels);

                // Create the Image view for the Vulkan image (uses color bit)
                if (desc.format == RZTextureProperties::Format::DEPTH32F || desc.format == RZTextureProperties::Format::DEPTH16_UNORM || desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                    m_AspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
                else
                    m_AspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

                // Create the Image view for the Vulkan image (uses color bit)
                m_ImageViews.push_back(CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(desc.type), m_AspectBit, desc.layers, 0, 0 RZ_DEBUG_E_ARG_NAME));

                if (m_Desc.enableMips) {
                    for (u32 i = 1; i < m_TotalMipLevels; i++)
                        m_ImageViews.push_back(VKTexture::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(desc.type), m_AspectBit, desc.layers, 0, i));
                }

                // Create a sampler view for the image
                auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
                m_ImageSampler           = CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(m_TotalMipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);

                VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_TotalMipLevels, desc.layers);

                m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                updateDescriptor();
            } else {
                bool loadResult = load(desc RZ_DEBUG_E_ARG_NAME);
                RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0}", desc.name);
            }

            updateDescriptor();
        }

        VKTexture::VKTexture(const RZTextureDesc& desc, const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Desc        = desc;
            m_VirtualPath = filePath;

            // Build a render target texture here if the data is nullptr
            bool loadResult = load(desc RZ_DEBUG_E_ARG_NAME);
            RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0}", desc.name);
            updateDescriptor();
        }

        VKTexture::VKTexture(VkImage image, VkImageView imageView)
            : m_Image(image), m_ImageSampler(VK_NULL_HANDLE), m_ImageMemory(VK_NULL_HANDLE)
        {
            m_ImageViews.push_back(imageView);

            updateDescriptor();
        }

        RAZIX_INLINE void VKTexture::updateDescriptor()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Descriptors.resize(1);

            m_Descriptors[0].sampler     = m_ImageSampler;
            m_Descriptors[0].imageView   = m_ImageViews[0];
            m_Descriptors[0].imageLayout = m_ImageLayout;

            if (!m_GenerateMips)
                return;

            for (u32 i = 1; i < m_TotalMipLevels; i++) {
                VkDescriptorImageInfo descriptor;
                descriptor.sampler     = m_ImageSampler;
                descriptor.imageView   = m_ImageViews[i];
                descriptor.imageLayout = m_ImageLayout;
                m_Descriptors.push_back(descriptor);
            }
        }

        bool VKTexture::load(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            void* pixels = nullptr;

            if (desc.data != nullptr) {
                pixels = desc.data;
                m_Size = VkDeviceSize(m_Desc.width * m_Desc.height * 4 * desc.dataSize);
            } else {
                if (m_VirtualPath != "" && m_VirtualPath != "NULL") {
                    u32 bpp;
                    // Width and Height are extracted here
                    // TODO: Support loading floating point Image data
                    pixels = Razix::Utilities::LoadImageData(m_VirtualPath, &m_Desc.width, &m_Desc.height, &bpp);
                    // Here the format for the texture is extracted based on bits per pixel
                    m_Desc.format = Razix::Graphics::RZTexture::bitsToTextureFormat(4);
                    // Size of the texture
                    m_Size = static_cast<u64>(m_Desc.width * m_Desc.height * 4);
                }
            }

            if (pixels == nullptr)
                return false;

            VkDeviceSize imageSize = VkDeviceSize(m_Desc.width * m_Desc.height * 4 * desc.dataSize);

            // Create a Staging buffer (Transfer from source) to transfer texture data from HOST memory to DEVICE memory
            VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, static_cast<u32>(imageSize), pixels RZ_DEBUG_NAME_TAG_STR_E_ARG("Staging Buffer VKTexture"));
            if (m_DeleteImageData)
                delete[] pixels;

            m_TotalMipLevels = static_cast<u32>(std::floor(std::log2(std::max(m_Desc.width, m_Desc.height)))) + 1;

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            VKTexture::CreateImage(m_Desc.width, m_Desc.height, m_Desc.depth, m_TotalMipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VKUtilities::TextureTypeToVK(m_Desc.type), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, m_Desc.layers, 0 RZ_DEBUG_E_ARG_NAME);

            //  There are two transitions we need to handle:
            //      1. Undefined -> transfer destination: transfer writes that don't need to wait on anything
            //          1.1 Copy image from transfer staging buffer to the Image buffer on DEVICE
            //      2. Transfer destination -> shader reading: shader reads should wait on transfer writes, specifically the shader reads in the fragment shader, because that's where we're going to use the texture

            // 1. Transfer layout to copy data from transfer buffer
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_TotalMipLevels);

            {
                // 1.1 Copy from staging buffer to Image
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                VkBufferImageCopy region               = {};
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = 0;
                region.bufferImageHeight               = 0;
                region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel       = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = {m_Desc.width, m_Desc.height, 1};

                vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }

            // 2. Transition from transfer to shader layout (This causing some error, some kind of unnecessary layout transition for the mip maps)
            //VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_TotalMipLevels);

            // This barrier transitions the last mip level from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            if (m_Desc.enableMips)
                GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_Desc.width, m_Desc.height, m_TotalMipLevels, m_Desc.layers);

            // TODO: Support loading multiple images into multiple layers/depths
            // Create the Image view for the Vulkan image (uses color bit)
            m_ImageViews.push_back(CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(desc.type), m_AspectBit, desc.layers, 0, 0 RZ_DEBUG_E_ARG_NAME));

            // Now since we have copied it properly we know the image is accessible from the DEVICE
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(m_TotalMipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);

            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_TotalMipLevels, desc.layers);

            // Update the Image descriptor with the created view and sampler
            updateDescriptor();

            // Delete and clean up any temp stuff
            stagingBuffer->destroy();
            delete stagingBuffer;

            return true;
        }

        void VKTexture::Destroy()
        {
            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            for (u32 i = 0; i < m_ImageViews.size(); i++)
                if (m_ImageViews[i] != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), m_ImageViews[i], nullptr);

            if (m_Image != VK_NULL_HANDLE)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);
        }

        int32_t VKTexture::ReadPixels(u32 x, u32 y)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Works only on Texture2D type for now!

            VKBuffer m_TransferBuffer;
            m_TransferBuffer.setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            m_TransferBuffer.setSize(m_Desc.width * m_Desc.height * 4);
            m_TransferBuffer.init(NULL RZ_DEBUG_NAME_TAG_STR_E_ARG("Transfer RT Buffer"));

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

            void* data = new char[m_Desc.width * m_Desc.height * 4];
            m_TransferBuffer.map();
            data = m_TransferBuffer.getMappedRegion();
            // Read and return the pixel value
            int32_t pixel_value = ((int32_t*) data)[(x) + (m_Desc.width * y)];
            m_TransferBuffer.unMap();

            return pixel_value;
        }

        //-----------------------------------------------------------------------------------

#if 0
 //-----------------------------------------------------------------------------------
        // Texture2D
        //-----------------------------------------------------------------------------------

        VKTexture2D::VKTexture2D(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            m_Desc        = desc;
            m_VirtualPath = "";

            m_Desc.type = RZTextureProperties::Type::Texture_2D;

            // Assert the Texture Type here
            RAZIX_ASSERT(m_Desc.type == RZTextureProperties::Type::Texture_2D, "[VULKAN] Incorrect texture type,  should be RZTextureProperties::Type::Texture_2D")

            // Build a render target texture here if the data is nullptr
            if (desc.data == nullptr) {
                u32 mipLevels = 1;
                if (desc.enableMips)
                    mipLevels = static_cast<u32>(std::floor(std::log2(std::max(desc.width, desc.height)))) + 1;

                VkImageUsageFlagBits usageBit{};
                if (desc.format == RZTextureProperties::Format::DEPTH32F || desc.format == RZTextureProperties::Format::DEPTH16_UNORM || desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                    usageBit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                else
                    usageBit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

                // Create the Vulkan Image and it's memory and Bind them together
                // We use a simple optimal tiling options
                VKTexture2D::CreateImage(desc.width, desc.height, 1, mipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, desc.layers, 0 RZ_DEBUG_E_ARG_NAME);

                // Create the Image view for the Vulkan image (uses color bit)
                VkImageAspectFlagBits aspectBit{};
                if (desc.format == RZTextureProperties::Format::DEPTH32F || desc.format == RZTextureProperties::Format::DEPTH16_UNORM || desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                    aspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
                else
                    aspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

                // Create the Image view for the Vulkan image (uses color bit)
                m_ImageView = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 0 RZ_DEBUG_E_ARG_NAME);

                // Create a sampler view for the image
                auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
                m_ImageSampler           = CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);
            } else {
                bool loadResult = load(desc RZ_DEBUG_E_ARG_NAME);
                RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0}", desc.name);
            }

            updateDescriptor();
        }

        VKTexture2D::VKTexture2D(RZ_DEBUG_NAME_TAG_F_ARG const std::string& filePath, const RZTextureDesc& desc)
        {
            m_Desc        = desc;
            m_VirtualPath = filePath;

            //RAZIX_ASSERT(desc.type == RZTextureProperties::Type::Texture_2D, "[VULKAN] Incorrect texture type,  should be RZTextureProperties::Type::Texture_2D")
            m_Desc.type = RZTextureProperties::Type::Texture_2D;

            // Build a render target texture here if the data is nullptr
            bool loadResult = load(desc RZ_DEBUG_E_ARG_NAME);
            RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0}", desc.name);
            updateDescriptor();
        }

        VKTexture2D::VKTexture2D(VkImage image, VkImageView imageView)
            : m_Image(image), m_ImageView(imageView), m_ImageSampler(VK_NULL_HANDLE), m_ImageMemory(VK_NULL_HANDLE)
        {
            m_Desc.type = RZTextureProperties::Type::Texture_2D;

            updateDescriptor();
        }

    #if 0
       VKTexture2D::VKTexture2D(const std::string& name, u32 width, u32 height, u32 numLayers, RZTextureProperties::Format format, RZTextureProperties::Wrapping wrapMode, RZTextureProperties::Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_VirtualPath = "";

            u32 mipLevels = 1;    // static_cast<u32>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;

            VkImageUsageFlagBits usageBit{};
            if (format == RZTextureProperties::Format::DEPTH32F || format == RZTextureProperties::Format::DEPTH16_UNORM || format == RZTextureProperties::Format::DEPTH_STENCIL)
                usageBit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usageBit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            VKTexture2D::CreateImage(m_Width, m_Height, 1, mipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, numLayers, 0 RZ_DEBUG_E_ARG_NAME);

            // Create the Image view for the Vulkan image (uses color bit)
            VkImageAspectFlagBits aspectBit{};
            if (format == RZTextureProperties::Format::DEPTH32F || format == RZTextureProperties::Format::DEPTH16_UNORM || format == RZTextureProperties::Format::DEPTH_STENCIL)
                aspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                aspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            // Create the Image view for the Vulkan image (uses color bit)
            m_ImageView = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 0 RZ_DEBUG_E_ARG_NAME);

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);

            updateDescriptor();
        }
    #endif

        void VKTexture2D::Release(bool deleteImage)
        {
            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            if (m_ImageView != VK_NULL_HANDLE)
                vkDestroyImageView(VKDevice::Get().getDevice(), m_ImageView, nullptr);

            if (deleteImage)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);

            //delete this;
        }

        void VKTexture2D::updateDescriptor()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Descriptor.imageView   = m_ImageView;
            m_Descriptor.sampler     = m_ImageSampler;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        void convert(unsigned char* dst, const unsigned char* src, sz num)
        {
            sz i;
            for (i = 0; i < num / 3; i++) {
                dst[4 * i]     = src[3 * i];
                dst[4 * i + 1] = src[3 * i + 1];
                dst[4 * i + 2] = src[3 * i + 2];
                dst[4 * i + 3] = 1;
            }
        }

        bool VKTexture2D::load(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            void* pixels = nullptr;

            if (desc.data != nullptr) {
                pixels = desc.data;
                m_Size = VkDeviceSize(m_Desc.width * m_Desc.height * 4 * desc.dataSize);    // TODO: Get the Bits per pixel from the format
            } else {
                if (m_VirtualPath != "" && m_VirtualPath != "NULL") {
                    u32 bpp;
                    // Width and Height are extracted here
                    pixels = Razix::Utilities::LoadImageData(m_VirtualPath, &m_Desc.width, &m_Desc.height, &bpp);
                    // Here the format for the texture is extracted based on bits per pixel
                    m_Desc.format = Razix::Graphics::RZTexture::bitsToTextureFormat(4);
                    // Size of the texture
                    m_Size = static_cast<u64>(m_Desc.width * m_Desc.height * 4);
                }
            }

            if (pixels == nullptr)
                return false;

            VkDeviceSize imageSize = VkDeviceSize(m_Desc.width * m_Desc.height * 4 * desc.dataSize);

            // Create a Staging buffer (Transfer from source) to transfer texture data from HOST memory to DEVICE memory
            VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, static_cast<u32>(imageSize), pixels RZ_DEBUG_NAME_TAG_STR_E_ARG("Staging Buffer VKTexture"));
            if (m_DeleteImageData)
                delete[] pixels;

            u32 mipLevels = static_cast<u32>(std::floor(std::log2(std::max(m_Desc.width, m_Desc.height)))) + 1;

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            CreateImage(m_Desc.width, m_Desc.height, 1, mipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0 RZ_DEBUG_E_ARG_NAME);

            //  There are two transitions we need to handle:
            //      1. Undefined -> transfer destination: transfer writes that don't need to wait on anything
            //          1.1 Copy image from transfer staging buffer to the Image buffer on DEVICE
            //      2. Transfer destination -> shader reading: shader reads should wait on transfer writes, specifically the shader reads in the fragment shader, because that's where we're going to use the texture

            // 1. Transfer layout to copy data from transfer buffer
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

            {
                // 1.1 Copy from staging buffer to Image
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                VkBufferImageCopy region               = {};
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = 0;
                region.bufferImageHeight               = 0;
                region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel       = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = {m_Desc.width, m_Desc.height, 1};

                vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }

            // 2. Transition from transfer to shader layout (This causing some error, some kind of unnecessary layout transition for the mip maps)
            //VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mipLevels);

            // This barrier transitions the last mip level from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            //if (m_Desc.enableMips)
            GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_Desc.width, m_Desc.height, mipLevels);

            // Create the Image view for the Vulkan image (uses color bit)
            m_ImageView = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 0 RZ_DEBUG_E_ARG_NAME);

            // Now since we have copied it properly we know the image is accessible from the DEVICE
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);

            // Update the Image descriptor with the created view and sampler
            updateDescriptor();

            // Delete and clean up any temp stuff
            stagingBuffer->destroy();
            delete stagingBuffer;

            return true;
        }

        //-----------------------------------------------------------------------------------

        VKTexture2DArray::VKTexture2DArray(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            m_Desc = desc;

            m_Desc.type = RZTextureProperties::Type::Texture_2DArray;

            // Asset the Texture Type
            RAZIX_ASSERT(m_Desc.type == RZTextureProperties::Type::Texture_2DArray, "[VULKAN] Incorrect texture type,  should be RZTextureProperties::Type::Texture_2DArray")

            m_VirtualPath = "";
    #if 1
            u32 mipLevels = 1;    // static_cast<u32>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;

            VkImageUsageFlagBits usageBit{};
            if (desc.format == RZTextureProperties::Format::DEPTH32F || desc.format == RZTextureProperties::Format::DEPTH16_UNORM || desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                usageBit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usageBit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            VKTexture2D::CreateImage(desc.width, desc.height, 1, mipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, desc.layers, 0 RZ_DEBUG_E_ARG_NAME);

            // Create the Image view for the Vulkan image (uses color bit)
            VkImageAspectFlagBits aspectBit{};
            if (desc.format == RZTextureProperties::Format::DEPTH32F || desc.format == RZTextureProperties::Format::DEPTH16_UNORM || desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                aspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                aspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            // Create the Image view for the Vulkan image (uses color bit)
            m_ImageView = VKTexture2D::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 0 RZ_DEBUG_E_ARG_NAME);

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = VKTexture2D::CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);

            updateDescriptor();
    #endif
        }

        void VKTexture2DArray::Release(bool deleteImage)
        {
            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            if (m_ImageView != VK_NULL_HANDLE)
                vkDestroyImageView(VKDevice::Get().getDevice(), m_ImageView, nullptr);

            if (deleteImage)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);
        }

        void VKTexture2DArray::updateDescriptor()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Descriptor.imageView   = m_ImageView;
            m_Descriptor.sampler     = m_ImageSampler;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        //-----------------------------------------------------------------------------------
        // Texture3D
        //-----------------------------------------------------------------------------------

        VKTexture3D::VKTexture3D(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            m_Desc        = desc;
            m_VirtualPath = "";

            m_Desc.type = RZTextureProperties::Type::Texture_3D;

            // Asset the Texture Type
            RAZIX_ASSERT(m_Desc.type == RZTextureProperties::Type::Texture_3D, "[VULKAN] Incorrect texture type,  should be RZTextureProperties::Type::Texture_3D")

            u32 mipLevels = 1;    //static_cast<u32>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;    //

            VkImageUsageFlagBits usageBit{};
            if (desc.format == RZTextureProperties::Format::DEPTH32F || desc.format == RZTextureProperties::Format::DEPTH16_UNORM || desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                usageBit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usageBit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            VKTexture2D::CreateImage(desc.width, desc.height, desc.layers, mipLevels, VKUtilities::TextureFormatToVK(desc.format), VK_IMAGE_TYPE_3D, VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0 RZ_DEBUG_E_ARG_NAME);

            //VKTexture2D::GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Format), m_Width, m_Height, mipLevels);

            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(desc.format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

            // Create the Image view for the Vulkan image (uses color bit)
            VkImageAspectFlagBits aspectBit{};
            if (desc.format == RZTextureProperties::Format::DEPTH32F || desc.format == RZTextureProperties::Format::DEPTH16_UNORM || desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                aspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                aspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            m_ImageView = VKTexture2D::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(desc.format), mipLevels, VK_IMAGE_VIEW_TYPE_3D, aspectBit, 1, 0, 0 RZ_DEBUG_E_ARG_NAME);

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = VKTexture2D::CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);

            updateDescriptor();
        }

        void VKTexture3D::Release(bool deleteImage /*= true*/)
        {
            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            if (m_ImageView != VK_NULL_HANDLE)
                vkDestroyImageView(VKDevice::Get().getDevice(), m_ImageView, nullptr);

            if (deleteImage)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);
        }

        void VKTexture3D::Bind(u32 slot)
        {
        }

        void VKTexture3D::Unbind(u32 slot)
        {
        }

        void* VKTexture3D::GetAPIHandlePtr() const
        {
            return (void*) &m_Descriptor;
        }

        void VKTexture3D::updateDescriptor()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Descriptor.imageView   = m_ImageView;
            m_Descriptor.sampler     = m_ImageSampler;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        //-----------------------------------------------------------------------------------
        // CubeMap Texture
        //-----------------------------------------------------------------------------------

        VKCubeMap::VKCubeMap(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
        {
            m_Desc         = desc;
            m_Desc.type    = RZTextureProperties::Type::Texture_CubeMap;
            m_GenerateMips = desc.enableMips;

            // Asset the Texture Type
            RAZIX_ASSERT(m_Desc.type == RZTextureProperties::Type::Texture_CubeMap, "[VULKAN] Incorrect texture type,  should be RZTextureProperties::Type::Texture_CubeMap")

            //m_Format    = RZTextureProperties::Format::RGBA32F;
            auto format = VKUtilities::TextureFormatToVK(m_Desc.format);

            if (m_Desc.enableMips)
                m_TotalMipLevels = static_cast<u32>(std::floor(std::log2(std::max(m_Desc.width, m_Desc.height)))) + 1;
            else
                m_TotalMipLevels = 1;

            VKTexture2D::CreateImage(m_Desc.width, m_Desc.height, 1, m_TotalMipLevels, format, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));

            VKUtilities::TransitionImageLayout(m_Image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_TotalMipLevels, 6);

            if (m_Desc.enableMips)
                VKTexture2D::GenerateMipmaps(m_Image, format, m_Desc.width, m_Desc.height, m_TotalMipLevels);

            m_ImageViews.push_back(VKTexture2D::CreateImageView(m_Image, format, m_TotalMipLevels, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 6, 0, 0));

            if (m_Desc.enableMips) {
                for (u32 i = 1; i < m_TotalMipLevels; i++)
                    m_ImageViews.push_back(VKTexture2D::CreateImageView(m_Image, format, m_TotalMipLevels, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 6, 0, i));
            }

            m_ImageSampler = VKTexture2D::CreateImageSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 0.0f, static_cast<f32>(m_TotalMipLevels), true, VKDevice::Get().getPhysicalDevice()->getProperties().limits.maxSamplerAnisotropy, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

            VKUtilities::TransitionImageLayout(m_Image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_TotalMipLevels, 6);

            updateDescriptor();
        }

        void VKCubeMap::Release(bool deleteImage /*= true*/)
        {
            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            for (u32 i = 0; i < m_ImageViews.size(); i++)
                if (m_ImageViews[i] != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), m_ImageViews[i], nullptr);

            if (deleteImage)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);
        }

        void VKCubeMap::Bind(u32 slot)
        {
        }

        void VKCubeMap::Unbind(u32 slot)
        {
        }

        void* VKCubeMap::GetAPIHandlePtr() const
        {
            return (void*) &m_Descriptors[m_CurrentMipRenderingLevel];
        }

        void VKCubeMap::convertEquirectangularToCubemap()
        {
        }

        void VKCubeMap::updateDescriptor()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Descriptors.resize(1);

            m_Descriptors[0].sampler     = m_ImageSampler;
            m_Descriptors[0].imageView   = m_ImageViews[0];
            m_Descriptors[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            if (!m_GenerateMips)
                return;

            for (u32 i = 1; i < m_TotalMipLevels; i++) {
                VkDescriptorImageInfo descriptor;
                descriptor.sampler     = m_ImageSampler;
                descriptor.imageView   = m_ImageViews[i];
                descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                m_Descriptors.push_back(descriptor);
            }
        }

        //-----------------------------------------------------------------------------------
        // Depth Texture
        //-----------------------------------------------------------------------------------

        VKDepthTexture::VKDepthTexture(const RZTextureDesc& desc)
            : m_ImageView(VK_NULL_HANDLE), m_ImageSampler(VK_NULL_HANDLE)
        {
            m_Desc = desc;

            m_Desc.type = RZTextureProperties::Type::Texture_Depth;

            // Asset the Texture Type
            RAZIX_ASSERT(desc.type == RZTextureProperties::Type::Texture_Depth, "[VULKAN] Incorrect texture type,  should be RZTextureProperties::Type::Texture_3D")

            init();
        }

        VKDepthTexture::~VKDepthTexture()
        {
        }

        void VKDepthTexture::Resize(u32 width, u32 height RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Desc.width  = width;
            m_Desc.height = height;

            Release(true);

            init();
        }

        void VKDepthTexture::Release(bool deleteImage /*= true*/)
        {
            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            if (m_ImageView != VK_NULL_HANDLE)
                vkDestroyImageView(VKDevice::Get().getDevice(), m_ImageView, nullptr);

            if (deleteImage)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);
        }

        void VKDepthTexture::Bind(u32 slot)
        {
        }

        void VKDepthTexture::Unbind(u32 slot)
        {
        }

        void* VKDepthTexture::GetAPIHandlePtr() const
        {
            return (void*) &m_Descriptor;
        }

        void VKDepthTexture::init()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            VkFormat depthFormat = VKUtilities::FindDepthFormat();

            VKTexture2D::CreateImage(m_Desc.width, m_Desc.height, 1, 1, depthFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0 RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));

            m_ImageView = VKTexture2D::CreateImageView(m_Image, depthFormat, 1, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

            m_ImageSampler = VKTexture2D::CreateImageSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 0.0f, 1.0f, true, VKDevice::Get().getPhysicalDevice()->getProperties().limits.maxSamplerAnisotropy, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

            VKUtilities::TransitionImageLayout(m_Image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            // Update the Image descriptor with the created view and sampler
            updateDescriptor();
        }

        void VKDepthTexture::updateDescriptor()
        {
            m_Descriptor.sampler     = m_ImageSampler;
            m_Descriptor.imageView   = m_ImageView;
            m_Descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;    //VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;    //
        }

        //-----------------------------------------------------------------------------------
        // Render Texture
        //-----------------------------------------------------------------------------------

        VKRenderTexture::VKRenderTexture(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc)
            : m_TransferBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT, desc.width * desc.height * 4, NULL RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name + "::Transfer RT Buffer"))
        {
            m_Desc        = desc;
            m_VirtualPath = "";
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            m_Desc.type = RZTextureProperties::Type::Texture_2D;

            // Asset the Texture Type
            RAZIX_ASSERT(desc.type == RZTextureProperties::Type::Texture_2D, "[VULKAN] Incorrect texture type,  should be RZTextureProperties::Type::Texture_2D")

            init(RZ_DEBUG_S_ARG_NAME);
        }

        VKRenderTexture::VKRenderTexture(VkImage image, VkImageView imageView)
            : m_Image(image), m_ImageView(imageView), m_ImageSampler(VK_NULL_HANDLE), m_ImageMemory(VK_NULL_HANDLE)
        {
            m_Desc.name   = "Render Target";
            m_VirtualPath = "";
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            m_Desc.type = RZTextureProperties::Type::Texture_2D;

            updateDescriptor();
        }

        void VKRenderTexture::Resize(u32 width, u32 height RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Desc.width  = width;
            m_Desc.height = height;

            Release(true);
            m_TransferBuffer.setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            m_TransferBuffer.setSize(width * height * 4);
            m_TransferBuffer.init(NULL RZ_DEBUG_NAME_TAG_STR_E_ARG("Transfer RT Buffer"));

            init(RZ_DEBUG_NAME_TAG_STR_S_ARG(m_Desc.name));
        }

        void VKRenderTexture::Release(bool deleteImage /*= true*/)
        {
            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            if (m_ImageView != VK_NULL_HANDLE)
                vkDestroyImageView(VKDevice::Get().getDevice(), m_ImageView, nullptr);

            if (deleteImage)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);

            m_TransferBuffer.destroy();
        }

        void VKRenderTexture::Bind(u32 slot)
        {
        }

        void VKRenderTexture::Unbind(u32 slot)
        {
        }

        void* VKRenderTexture::GetAPIHandlePtr() const
        {
            return (void*) &m_Descriptor;
        }

        int32_t VKRenderTexture::ReadPixels(u32 x, u32 y)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Change the image layout from shader read only optimal to transfer source
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            {
                // 1.1 Copy from staging buffer to Image
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                VkBufferImageCopy region               = {};
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = m_Desc.width;
                region.bufferImageHeight               = m_Desc.width;
                region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel       = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = {m_Desc.width, m_Desc.height, 1};

                vkCmdCopyImageToBuffer(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_TransferBuffer.getBuffer(), 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            void* data = new char[m_Desc.width * m_Desc.height * 4];
            m_TransferBuffer.map();
            data = m_TransferBuffer.getMappedRegion();
            // Read and return the pixel value
            int32_t pixel_value = ((int32_t*) data)[(x) + (m_Desc.width * y)];
            m_TransferBuffer.unMap();

            return pixel_value;
        }

        void VKRenderTexture::init(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            u32 mipLevels = 1;    // static_cast<u32>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;//1;//

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            VKTexture2D::CreateImage(m_Desc.width, m_Desc.height, 1, mipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0 RZ_DEBUG_E_ARG_NAME);

            // Create the Image view for the Vulkan image (uses color bit)
            VkImageAspectFlagBits aspectBit{};
            if (m_Desc.format == RZTextureProperties::Format::DEPTH32F || m_Desc.format == RZTextureProperties::Format::DEPTH16_UNORM || m_Desc.format == RZTextureProperties::Format::DEPTH_STENCIL)
                aspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                aspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            // Create the Image view for the Vulkan image (uses color bit)
            m_ImageView = VKTexture2D::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), mipLevels, VK_IMAGE_VIEW_TYPE_2D, aspectBit, 1, 0, 0 RZ_DEBUG_E_ARG_NAME);

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = VKTexture2D::CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);

            updateDescriptor();
        }

        void VKRenderTexture::updateDescriptor()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Descriptor.imageView   = m_ImageView;
            m_Descriptor.sampler     = m_ImageSampler;
            m_Descriptor.imageLayout = m_ImageLayout;
        }
#endif
    }    // namespace Graphics
}    // namespace Razix