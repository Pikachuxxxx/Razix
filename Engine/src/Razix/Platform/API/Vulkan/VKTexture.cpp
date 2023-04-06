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
        // Texture Utility Functions
        //-----------------------------------------------------------------------------------

        void VKTexture2D::CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG)
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

        void VKTexture2D::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, u32 mipLevels)
        {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(VKDevice::Get().getGPU(), imageFormat, &formatProperties);

            if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
                RAZIX_CORE_ERROR("Texture image format does not support linear blitting!");
            }

            VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

            VkImageMemoryBarrier barrier{};
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image                           = image;
            barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
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
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount     = 1;
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

        VkImageView VKTexture2D::CreateImageView(VkImage image, VkFormat format, u32 mipLevels, VkImageViewType viewType, VkImageAspectFlags aspectMask, u32 layerCount, u32 baseArrayLayer RZ_DEBUG_NAME_TAG_E_ARG)
        {
            VkImageViewCreateInfo viewInfo           = {};
            viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image                           = image;
            viewInfo.viewType                        = viewType;
            viewInfo.format                          = format;
            viewInfo.components                      = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
            viewInfo.subresourceRange.aspectMask     = aspectMask;
            viewInfo.subresourceRange.baseMipLevel   = 0;
            viewInfo.subresourceRange.levelCount     = mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
            viewInfo.subresourceRange.layerCount     = layerCount;

            VkImageView imageView;
            VK_CHECK_RESULT(vkCreateImageView(VKDevice::Get().getDevice(), &viewInfo, nullptr, &imageView));

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t) imageView);

            return imageView;
        }

        VkSampler VKTexture2D::CreateImageSampler(VkFilter magFilter /*= VK_FILTER_LINEAR*/, VkFilter minFilter /*= VK_FILTER_LINEAR*/, f32 minLod /*= 0.0f*/, f32 maxLod /*= 1.0f*/, bool anisotropyEnable /*= false*/, f32 maxAnisotropy /*= 1.0f*/, VkSamplerAddressMode modeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode modeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode modeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/ RZ_DEBUG_NAME_TAG_E_ARG)
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

        //-----------------------------------------------------------------------------------
        // Texture2D
        //-----------------------------------------------------------------------------------

        VKTexture2D::VKTexture2D(const std::string& name, u32 width, u32 height, void* data, Format format, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Name        = name;
            m_Width       = width;
            m_Height      = height;
            m_Format      = format;
            m_FilterMode  = filterMode;
            m_WrapMode    = wrapMode;
            m_VirtualPath = "";
            m_data        = static_cast<u8*>(data);

            m_TextureType = RZTexture::Type::COLOR_2D;

            // Build a render target texture here if the data is nullptr

            bool loadResult = load(RZ_DEBUG_S_ARG_NAME);
            RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0}", name);
            updateDescriptor();
        }

        VKTexture2D::VKTexture2D(const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_VirtualPath = filePath;
            m_Name        = name;
            m_FilterMode  = filterMode;
            m_WrapMode    = wrapMode;

            m_TextureType = RZTexture::Type::COLOR_2D;

            bool loadResult = load(RZ_DEBUG_S_ARG_NAME);
            RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0} at location : {1}", name, filePath);
            updateDescriptor();
        }

        VKTexture2D::VKTexture2D(VkImage image, VkImageView imageView)
            : m_Image(image), m_ImageView(imageView), m_ImageSampler(VK_NULL_HANDLE), m_ImageMemory(VK_NULL_HANDLE)
        {
            m_TextureType = RZTexture::Type::COLOR_2D;

            updateDescriptor();
        }

        VKTexture2D::VKTexture2D(const std::string& name, u32 width, u32 height, u32 numLayers, Format format, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_TextureType = RZTexture::Type::COLOR_2D;    // It is also an Render Target
            m_Name        = name;
            m_Width       = width;
            m_Height      = height;
            m_Format      = format;
            m_FilterMode  = filterMode;
            m_WrapMode    = wrapMode;
            m_VirtualPath = "";

            u32 mipLevels = 1;    // static_cast<u32>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;//1;//

            VkImageUsageFlagBits usageBit{};
            if (format == RZTexture::Format::DEPTH32F || format == RZTexture::Format::DEPTH16_UNORM || format == RZTexture::Format::DEPTH_STENCIL)
                usageBit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usageBit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            VKTexture2D::CreateImage(m_Width, m_Height, 1, mipLevels, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, numLayers, 0 RZ_DEBUG_E_ARG_NAME);

            // Create the Image view for the Vulkan image (uses color bit)
            VkImageAspectFlagBits aspectBit{};
            if (format == RZTexture::Format::DEPTH32F || format == RZTexture::Format::DEPTH16_UNORM || format == RZTexture::Format::DEPTH_STENCIL)
                aspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                aspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            m_ImageView = VKTexture2D::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Format), mipLevels, VK_IMAGE_VIEW_TYPE_2D_ARRAY, aspectBit, numLayers, 0 RZ_DEBUG_E_ARG_NAME);

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = VKTexture2D::CreateImageSampler(VKUtilities::TextureFilterToVK(m_FilterMode.magFilter), VKUtilities::TextureFilterToVK(m_FilterMode.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode) RZ_DEBUG_E_ARG_NAME);

            updateDescriptor();
        }

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

        bool VKTexture2D::load(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            unsigned char* pixels = nullptr;

            if (m_data != nullptr) {
                pixels = reinterpret_cast<u8*>(m_data);
                m_Size = VkDeviceSize(m_Width * m_Height * 4);    // TODO: Get the Bits per pixel from the format
            } else {
                if (m_VirtualPath != "" && m_VirtualPath != "NULL") {
                    u32 bpp;
                    // Width and Height are extracted here
                    pixels = Razix::Utilities::LoadImageData(m_VirtualPath, &m_Width, &m_Height, &bpp);
                    // Here the format for the texture is extracted based on bits per pixel
                    m_Format = Razix::Graphics::RZTexture::bitsToTextureFormat(4);
                    // Size of the texture
                    m_Size = m_Width * m_Height * bpp;    // Divided by 8 cause char* is 8 bits and size is in bytes
                }
            }

            if (pixels == nullptr)
                return false;

            VkDeviceSize imageSize = VkDeviceSize(m_Width * m_Height * 4);

            std::vector<unsigned char> pixelData(imageSize);
            //if (m_Size != imageSize)
            //    convert(pixelData.data(), pixels, m_Size);
            //else
            memcpy(pixelData.data(), pixels, m_Size);

            // Create a Staging buffer (Transfer from source) to transfer texture data from HOST memory to DEVICE memory
            VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, static_cast<u32>(imageSize), pixelData.data() RZ_DEBUG_NAME_TAG_STR_E_ARG("Staging Buffer VKTexture"));
            //stagingBuffer->setData(imageSize, pixels);

            u32 mipLevels = static_cast<u32>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;    //1;//

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            CreateImage(m_Width, m_Height, 1, mipLevels, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0 RZ_DEBUG_E_ARG_NAME);

            //  There are two transitions we need to handle:
            //      1. Undefined -> transfer destination: transfer writes that don't need to wait on anything
            //          1.1 Copy image from transfer staging buffer to the Image buffer on DEVICE
            //      2. Transfer destination -> shader reading: shader reads should wait on transfer writes, specifically the shader reads in the fragment shader, because that's where we're going to use the texture

            // 1. Transfer layout to copy data from transfer buffer
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

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
                region.imageExtent                     = {m_Width, m_Height, 1};

                vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }

            // 2. Transition from transfer to shader layout (This causing some error, some kind of unnecessary layout transition for the mip maps)
            //VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

            // This barrier transitions the last mip level from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Format), m_Width, m_Height, mipLevels);

            // Create the Image view for the Vulkan image (uses color bit)
            m_ImageView = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Format), mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0 RZ_DEBUG_E_ARG_NAME);

            // Now since we have copied it properly we know the image is accessible from the DEVICE
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = CreateImageSampler(VKUtilities::TextureFilterToVK(m_FilterMode.magFilter), VKUtilities::TextureFilterToVK(m_FilterMode.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode) RZ_DEBUG_E_ARG_NAME);

            // Update the Image descriptor with the created view and sampler
            updateDescriptor();

            // Delete and clean up any temp stuff
            stagingBuffer->destroy();
            delete stagingBuffer;

            return true;
        }

        //-----------------------------------------------------------------------------------
        // Texture3D
        //-----------------------------------------------------------------------------------

        VKTexture3D::VKTexture3D(const std::string& name, u32 width, u32 height, u32 depth, Format format, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_TextureType = RZTexture::Type::COLOR_3D;
            m_Name        = name;
            m_Width       = width;
            m_Height      = height;
            m_Format      = format;
            m_FilterMode  = filterMode;
            m_WrapMode    = wrapMode;
            m_VirtualPath = "";

            u32 mipLevels = 1;    //static_cast<u32>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;    //

            VkImageUsageFlagBits usageBit{};
            if (format == RZTexture::Format::DEPTH32F || format == RZTexture::Format::DEPTH16_UNORM || format == RZTexture::Format::DEPTH_STENCIL)
                usageBit = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usageBit = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            VKTexture2D::CreateImage(m_Width, m_Height, depth, mipLevels, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_TYPE_3D, VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0 RZ_DEBUG_E_ARG_NAME);

            //VKTexture2D::GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Format), m_Width, m_Height, mipLevels);

            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

            // Create the Image view for the Vulkan image (uses color bit)
            VkImageAspectFlagBits aspectBit{};
            if (format == RZTexture::Format::DEPTH32F || format == RZTexture::Format::DEPTH16_UNORM || format == RZTexture::Format::DEPTH_STENCIL)
                aspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                aspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            m_ImageView = VKTexture2D::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Format), mipLevels, VK_IMAGE_VIEW_TYPE_3D, aspectBit, 1, 0 RZ_DEBUG_E_ARG_NAME);

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = VKTexture2D::CreateImageSampler(VKUtilities::TextureFilterToVK(m_FilterMode.magFilter), VKUtilities::TextureFilterToVK(m_FilterMode.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode) RZ_DEBUG_E_ARG_NAME);

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

        void* VKTexture3D::GetHandle() const
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
        // Depth Texture
        //-----------------------------------------------------------------------------------

        VKDepthTexture::VKDepthTexture(u32 width, u32 height)
            : m_ImageView(VK_NULL_HANDLE), m_ImageSampler(VK_NULL_HANDLE)
        {
            m_Name   = "Depth Texture";
            m_Width  = width;
            m_Height = height;

            m_TextureType = RZTexture::Type::DEPTH;

            init();
        }

        VKDepthTexture::~VKDepthTexture()
        {
        }

        void VKDepthTexture::Resize(u32 width, u32 height RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Width  = width;
            m_Height = height;

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

        void* VKDepthTexture::GetHandle() const
        {
            return (void*) &m_Descriptor;
        }

        void VKDepthTexture::init()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Format = RZTexture::Format::DEPTH32F;

            VkFormat depthFormat = VKUtilities::FindDepthFormat();

            VKTexture2D::CreateImage(m_Width, m_Height, 1, 1, depthFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0 RZ_DEBUG_NAME_TAG_STR_E_ARG("DepthTexture"));

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

        VKRenderTexture::VKRenderTexture(u32 width, u32 height, Format format, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG)
            : m_TransferBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT, width * height * 4, NULL RZ_DEBUG_NAME_TAG_STR_E_ARG("Transfer RT Buffer"))
        {
            m_Name        = "Render Target";
            m_Width       = width;
            m_Height      = height;
            m_Format      = format;
            m_FilterMode  = filterMode;
            m_WrapMode    = wrapMode;
            m_VirtualPath = "";
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            m_TextureType = RZTexture::Type::COLOR_RT;

            init(RZ_DEBUG_S_ARG_NAME);
        }

        VKRenderTexture::VKRenderTexture(VkImage image, VkImageView imageView)
            : m_Image(image), m_ImageView(imageView), m_ImageSampler(VK_NULL_HANDLE), m_ImageMemory(VK_NULL_HANDLE)
        {
            m_Name        = "Render Target";
            m_VirtualPath = "";
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            m_TextureType = RZTexture::Type::COLOR_RT;

            updateDescriptor();
        }

        void VKRenderTexture::Resize(u32 width, u32 height RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Width  = width;
            m_Height = height;

            m_TextureType = RZTexture::Type::COLOR_RT;

            Release(true);
            m_TransferBuffer.setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            m_TransferBuffer.setSize(width * height * 4);
            m_TransferBuffer.init(NULL RZ_DEBUG_NAME_TAG_STR_E_ARG("Transfer RT Buffer"));

            init(RZ_DEBUG_NAME_TAG_STR_S_ARG(m_Name));
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

        void* VKRenderTexture::GetHandle() const
        {
            return (void*) &m_Descriptor;
        }

        int32_t VKRenderTexture::ReadPixels(u32 x, u32 y)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Change the image layout from shader read only optimal to transfer source
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            {
                // 1.1 Copy from staging buffer to Image
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                VkBufferImageCopy region               = {};
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = m_Width;     // Try with m_Width
                region.bufferImageHeight               = m_Height;    // Try with m_Height
                region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel       = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = {m_Width, m_Height, 1};

                vkCmdCopyImageToBuffer(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_TransferBuffer.getBuffer(), 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            void* data = new char[m_Width * m_Height * 4];
            m_TransferBuffer.map();
            data = m_TransferBuffer.getMappedRegion();
            // Read and return the pixel value
            int32_t pixel_value = ((int32_t*) data)[(x) + (m_Width * y)];
            m_TransferBuffer.unMap();

            return pixel_value;
        }

        void VKRenderTexture::init(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            m_TextureType = RZTexture::Type::COLOR_RT;

            u32 mipLevels = 1;    // static_cast<u32>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;//1;//

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            VKTexture2D::CreateImage(m_Width, m_Height, 1, mipLevels, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0 RZ_DEBUG_E_ARG_NAME);

            // Create the Image view for the Vulkan image (uses color bit)
            VkImageAspectFlagBits aspectBit{};
            if (m_Format == RZTexture::Format::DEPTH32F || m_Format == RZTexture::Format::DEPTH16_UNORM || m_Format == RZTexture::Format::DEPTH_STENCIL)
                aspectBit = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                aspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            // Create the Image view for the Vulkan image (uses color bit)
            m_ImageView = VKTexture2D::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Format), mipLevels, VK_IMAGE_VIEW_TYPE_2D, aspectBit, 1, 0 RZ_DEBUG_E_ARG_NAME);

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = VKTexture2D::CreateImageSampler(VKUtilities::TextureFilterToVK(m_FilterMode.magFilter), VKUtilities::TextureFilterToVK(m_FilterMode.minFilter), 0.0f, static_cast<f32>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode) RZ_DEBUG_E_ARG_NAME);

            updateDescriptor();
        }

        void VKRenderTexture::updateDescriptor()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Descriptor.imageView   = m_ImageView;
            m_Descriptor.sampler     = m_ImageSampler;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        //-----------------------------------------------------------------------------------
        // CubeMap Texture
        //-----------------------------------------------------------------------------------

        VKCubeMap::VKCubeMap(const std::string& hdrFilePath, const std::string& name, Wrapping wrapMode, Filtering filterMode)
        {
            m_Name        = name;
            m_FilterMode  = filterMode;
            m_WrapMode    = wrapMode;
            m_TextureType = RZTexture::Type::CUBEMAP;
            m_VirtualPath = hdrFilePath;

            // FIXME: hard coded shit!
            m_Width  = 1024;
            m_Height = 1024;

            updateDescriptor();
        }

        VKCubeMap::VKCubeMap(const std::string& name, Wrapping wrapMode, Filtering filterMode)
        {
            m_Name        = name;
            m_FilterMode  = filterMode;
            m_WrapMode    = wrapMode;
            m_TextureType = RZTexture::Type::CUBEMAP;

            // FIXME: hard coded shit!
            m_Width  = 1024;
            m_Height = 1024;

            auto format = VK_FORMAT_R32G32B32A32_SFLOAT;
            VKTexture2D::CreateImage(1024, 1024, 1, 1, format, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Name));

            m_ImageView = VKTexture2D::CreateImageView(m_Image, format, 1, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 6);

            m_ImageSampler = VKTexture2D::CreateImageSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 0.0f, 1.0f, true, VKDevice::Get().getPhysicalDevice()->getProperties().limits.maxSamplerAnisotropy, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

            VKUtilities::TransitionImageLayout(m_Image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 6);

            updateDescriptor();
        }

        void VKCubeMap::Release(bool deleteImage /*= true*/)
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

        void VKCubeMap::Bind(u32 slot)
        {
        }

        void VKCubeMap::Unbind(u32 slot)
        {
        }

        void* VKCubeMap::GetHandle() const
        {
            return (void*) &m_Descriptor;
        }

        void VKCubeMap::convertEquirectangularToCubemap()
        {
        }

        void VKCubeMap::updateDescriptor()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Descriptor.sampler     = m_ImageSampler;
            m_Descriptor.imageView   = m_ImageView;
            m_Descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
    }    // namespace Graphics
}    // namespace Razix