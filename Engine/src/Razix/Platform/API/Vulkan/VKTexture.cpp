#include "rzxpch.h"
#include "VKTexture.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include "Razix/Utilities/LoadImage.h"

#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix { 
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Texture Utility Functions
        //-----------------------------------------------------------------------------------

        /**
         * Creates a Vulkan Image handle
         * 
         * @param width The width of the texture
         * @param height The height of the texture
         * @param mipLevels Mips to generate
         * @param format The Vulkan format
         * @param imageType The image type of Vulkan
         * @param tiling The tiling to be used for image
         * @param usage The vulkan image usage
         * @param properties The properties of the image memory
         * @param image The reference to the image to be created
         * @param imageMemory The reference to the image memory to created and will be bound to
         */
        static void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t arrayLayers, VkImageCreateFlags flags)
        {
            // We pass the image as reference because we need the memory for it as well
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = imageType;
            imageInfo.extent = { width, height, 1 };
            imageInfo.mipLevels = mipLevels;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.arrayLayers = arrayLayers;

            imageInfo.flags = flags;

            // Create the image 
            VK_CHECK_RESULT(vkCreateImage(VKDevice::Get().getDevice(), &imageInfo, nullptr, &image));

            // Get the memory requirements for the image and allocate memory for it
            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(VKDevice::Get().getDevice(), image, &memRequirements);

            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = VKDevice::Get().getPhysicalDevice().get()->getMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

            VK_CHECK_RESULT(vkAllocateMemory(VKDevice::Get().getDevice(), &allocInfo, nullptr, &imageMemory));
            // Bind the image memory with the image
            VK_CHECK_RESULT(vkBindImageMemory(VKDevice::Get().getDevice(), image, imageMemory, 0));
        }
        
        /**
         * Creates an ImageView for the Vulkan image
         * 
         * @param image The handle for vulkan image
         * @param format The format of the image
         * @param mipLevels The number of mip maps to generate
         * @param viewType How are we viewing the image in 1D or 2D etc
         * @param aspectMask Bit mask flags specifying which aspects of an image are included in a view for purposes such as identifying a subresource
         * @param layerCount The layers of image views usually 1 unless stereoscopic 3D is used
         * @param baseArrayLayer used if sterescopic3D is used to identify the layer of image to create the image view for
         */
        static VkImageView CreateImageView(VkImage image, VkFormat format, uint32_t mipLevels, VkImageViewType viewType, VkImageAspectFlags aspectMask, uint32_t layerCount, uint32_t baseArrayLayer = 0)
        {
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = viewType;
            viewInfo.format = format;
            viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
            viewInfo.subresourceRange.aspectMask = aspectMask;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
            viewInfo.subresourceRange.layerCount = layerCount;

            VkImageView imageView;
            VK_CHECK_RESULT(vkCreateImageView(VKDevice::Get().getDevice(), &viewInfo, nullptr, &imageView));

            return imageView;
        }

        /**
         * Creates a sampler to sampler the image in shader pipeline stage
         * 
         * @param magFilter The mag filter to use
         * @param minFilter The minification filter to use
         * @param minLod The min LOD with which the image will be sampled with
         * @param maxLod The max LOD with which the image will be sampled with
         * @param anisotropyEnable Whether or not to enable anisotropic filtering
         * @param maxAnisotropy Maximum anisotropy supported by the GPU
         * @param modeU Texel U coordinate wrap mode
         * @param modeV Texel V coordinate wrap mode
         * @param modeW Texel W coordinate wrap mode
         */
        static VkSampler CreateImageSampler(VkFilter magFilter = VK_FILTER_LINEAR, VkFilter minFilter = VK_FILTER_LINEAR, float minLod = 0.0f, float maxLod = 1.0f, bool anisotropyEnable = false, float maxAnisotropy = 1.0f, VkSamplerAddressMode modeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
        {
            VkSampler sampler;
            VkSamplerCreateInfo samplerInfo = {};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = magFilter;
            samplerInfo.minFilter = minFilter;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.addressModeU = modeU;
            samplerInfo.addressModeV = modeV;
            samplerInfo.addressModeW = modeW;
            samplerInfo.maxAnisotropy = maxAnisotropy;
            samplerInfo.anisotropyEnable = anisotropyEnable;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
            samplerInfo.minLod = minLod;
            samplerInfo.maxLod = maxLod;

            VK_CHECK_RESULT(vkCreateSampler(VKDevice::Get().getDevice(), &samplerInfo, nullptr, &sampler));
            
            return sampler;
        }

        //-----------------------------------------------------------------------------------
        // Texture2D
        //-----------------------------------------------------------------------------------

        VKTexture2D::VKTexture2D(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode)
        {
            m_Name = name;
            m_Width = width;
            m_Height = height;
            m_Format = format;
            m_FilterMode = filterMode;
            m_WrapMode = wrapMode;
            m_VirtualPath = "";
            m_data = static_cast<uint8_t*>(data);

            bool loadResult = load();
            RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0}", name);
        }

        VKTexture2D::VKTexture2D(const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode)
        {
            m_VirtualPath = filePath;
            m_Name = name;
            m_FilterMode = filterMode;
            m_WrapMode = wrapMode;

            bool loadResult = load();
            RAZIX_CORE_ASSERT(loadResult, "[Vulkan] Failed to load Texture data! Name : {0} at location : {1}", name, filePath);
        }

        VKTexture2D::~VKTexture2D()
        {
            if (m_ImageSampler)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            if (m_ImageView)
                vkDestroyImageView(VKDevice::Get().getDevice(), m_ImageView, nullptr);

            vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemory)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemory, nullptr);
        }

        void VKTexture2D::updateDescriptor()
        {
            m_Descriptor.imageView      = m_ImageView;
            m_Descriptor.sampler        = m_ImageSampler;
            m_Descriptor.imageLayout    = m_ImageLayout;
        }

        bool VKTexture2D::load()
        {
            uint8_t* pixels = nullptr;

            if (m_data != nullptr)
                pixels = reinterpret_cast<uint8_t*>(m_data);
            else {
                if (m_VirtualPath != "" && m_VirtualPath != "NULL") {
                    uint32_t bpp;
                    // Width and Height are extracted here
                    pixels = Razix::Utilities::LoadImageData(m_VirtualPath, &m_Width, &m_Height, &bpp);
                    // Here the format for the texture is extracted based on bits per pixel
                    m_Format = Razix::Graphics::RZTexture::bitsToTextureFormat(bpp);
                    // Size of the texture
                    m_Size = m_Width * m_Height * bpp;
                }
            }

            if (pixels == nullptr)
                return false;

            VkDeviceSize imageSize = VkDeviceSize(m_Size);

            // Create a Staging buffer (Transfer from source) to transfer texture data from HOST memory to DEVICE memory 
            VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, static_cast<uint32_t>(imageSize), pixels);

            uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1;

            // Create the Vulkan Image and it's memory and Bind them together
            // We use a simple optimal tiling options
            CreateImage(m_Width, m_Height, mipLevels, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, 1, 0);

            //  There are two transitions we need to handle:
            //      1. Undefined -> transfer destination: transfer writes that don't need to wait on anything
            //          1.1 Copy image from transfer staging buffer to the Image buffer on DEVICE
            //      2. Transfer destination -> shader reading: shader reads should wait on transfer writes, specifically the shader reads in the fragment shader, because that's where we're going to use the texture
            
            // 1. Transfer layout to copy data from transfer buffer
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

            {
                // 1.1 Copy from staging buffer to Image
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                VkBufferImageCopy region;
                region.bufferOffset = 0;
                region.bufferRowLength = 0;
                region.bufferImageHeight = 0;
                region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount = 1;
                region.imageOffset = { 0, 0, 0 };
                region.imageExtent = { m_Width, m_Height, 1 };

                vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }

            // Now since we have copied it properly we know the image is accessible from the DEVICE
            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            // 2. Transition from transfer to shader layout
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

            // Delete and clean up any temp stuff
            delete stagingBuffer;

            // TODO: Generate mip maps for the Vulkan Texture

            // Create the Image view for the Vulkan image (uses color bit)
            m_ImageView = CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Format), mipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);

            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler = CreateImageSampler(VKUtilities::TextureFilterToVK(m_FilterMode.magFilter), VKUtilities::TextureFilterToVK(m_FilterMode.minFilter), 0.0f, static_cast<float>(mipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode), VKUtilities::TextureWrapToVK(m_WrapMode));

            // Update the Image descriptor with the created view and sampler
            updateDescriptor();

            return true;
        }
    }
}
