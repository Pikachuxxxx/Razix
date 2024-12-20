// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKTexture.h"

#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include "Razix/Utilities/RZLoadImage.h"

#include "Razix/Utilities/RZColorUtilities.h"

#include <vendor/stb/stb_image_write.h>

namespace Razix {
    namespace Gfx {

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

            m_FullResourceView.rtv = imageView;
        }

        //-------------------------------------------------------------------------------------------

        RAZIX_CLEANUP_RESOURCE_IMPL(VKTexture)
        {
            if (m_Desc.type == TextureType::kSwapchainImage)
                return;

            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);

            m_FullResourceView.destroy();

            for (u32 l = 1; l < m_Desc.layers; l++) {
                for (u32 m = 1; m < m_TotalMipLevels; m++) {
                    m_LayerMipResourceViews[l][m].destroy();
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
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer("Read Pixels", glm::vec4(0.0f));

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

            VKUtilities::GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_Desc.width, m_Desc.height, m_TotalMipLevels, m_Desc.layers);

            initMipViewsPerFace();

            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        void VKTexture::UploadToBindlessSet()
        {
            // Now if Bindless is available Bind to it
            if (VKDevice::Get().isBindlessSupported()) {
                RZDescriptor descriptor = {};

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = m_ImageLayout;
                imageInfo.imageView   = getFullSRVImageView();
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
            VKTexture::CreateImage(m_Desc.width, m_Desc.height, m_Desc.depth, m_TotalMipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VKUtilities::TextureTypeToVK(m_Desc.type), VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory, m_Desc.layers, flags RZ_DEBUG_E_ARG_NAME);
#else
            VKUtilities::CreateImage(m_Desc.width, m_Desc.height, m_Desc.depth, m_TotalMipLevels, VKUtilities::TextureFormatToVK(m_Desc.format), VKUtilities::TextureTypeToVK(m_Desc.type), VK_IMAGE_TILING_OPTIMAL, usageBit | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_VMAAllocation, m_Desc.layers, flags RZ_DEBUG_E_ARG_NAME);
#endif

            //  There are two transitions we need to handle:
            //      1. Undefined -> transfer destination: transfer writes that don't need to wait on anything
            //          1.1 Copy image from transfer staging buffer to the Image buffer on DEVICE
            //      2. Transfer destination -> shader reading: shader reads should wait on transfer writes, specifically the shader reads in the fragment shader, because that's where we're going to use the texture
            VkImageLayout firstTransitionLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            // Copy the data to GPU, if there is some
            if (desc.data) {
                firstTransitionLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, firstTransitionLayout, m_TotalMipLevels, m_Desc.layers);
                VKUtilities::CopyDataToGPUTextureResource(m_Desc.data, m_Image, m_Desc.width, m_Desc.height, m_Desc.size, m_CurrentMipRenderingLevel, m_Desc.layers, m_BaseArrayLayer);
            } else {
                firstTransitionLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;    // used to generate mips just in case
                VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, firstTransitionLayout, m_TotalMipLevels, m_Desc.layers);

                // DEBUG: Testing if mips and all layers can successfully transition to another layout
                //VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), firstTransitionLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_TotalMipLevels, m_Desc.layers);
            }

            delete desc.data;

            // DEBUG: Generate on demand at least while I'm debugging
            //if (m_Desc.enableMips)
            //    VKTexture::GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_Desc.width, m_Desc.height, m_TotalMipLevels, m_Desc.layers);

            // Create the Image view for the Vulkan image (uses color bit)
            if (desc.format == TextureFormat::DEPTH32F || desc.format == TextureFormat::DEPTH16_UNORM || m_Desc.format == TextureFormat::DEPTH_STENCIL) {
                m_Desc.filtering = {Filtering::Mode::kFilterModeNearest, Filtering::Mode::kFilterModeNearest},
                m_AspectBit      = VK_IMAGE_ASPECT_DEPTH_BIT;
            } else
                m_AspectBit = VK_IMAGE_ASPECT_COLOR_BIT;

            // We handle a special case here for RWTextureCube, we create both a SRV and UAV with different settings
            // Create the Image view for every layer at 0 mip as default Shader Resource View
            // We store the initial full resource view
            if (desc.type == TextureType::kRWCubeMap) {
                if ((m_ResourceViewHint & kSRV) == kSRV)
                    m_FullResourceView.srv = VKUtilities::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(TextureType::kCubeMap), m_AspectBit, desc.layers, RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER, RAZIX_TEXTURE_DEFAULT_MIP_IDX RZ_DEBUG_E_ARG_NAME);
                if ((m_ResourceViewHint & kUAV) == kUAV)
                    m_FullResourceView.uav = VKUtilities::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(TextureType::kRW2DArray), m_AspectBit, desc.layers, RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER, RAZIX_TEXTURE_DEFAULT_MIP_IDX RZ_DEBUG_E_ARG_NAME);

            } else {
                if ((m_ResourceViewHint & kSRV) == kSRV)
                    m_FullResourceView.srv = VKUtilities::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(desc.type), m_AspectBit, desc.layers, RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER, RAZIX_TEXTURE_DEFAULT_MIP_IDX RZ_DEBUG_E_ARG_NAME);
                if ((m_ResourceViewHint & kUAV) == kUAV)
                    m_FullResourceView.uav = m_FullResourceView.srv;
            }

            if (m_Desc.enableMips)
                initMipViewsPerFace();

            ////////////////////////////////////////
            // DEPRECATED, soon to be removed!
            // Create a sampler view for the image
            auto physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            m_ImageSampler           = VKUtilities::CreateImageSampler(VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter), VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter), 0.0f, static_cast<f32>(m_TotalMipLevels), true, physicalDeviceProps.limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping), VKUtilities::TextureWrapToVK(m_Desc.wrapping) RZ_DEBUG_E_ARG_NAME);
            ////////////////////////////////////////

            VkImageLayout finalLayout = (m_ResourceViewHint & kUAV) == kUAV ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), firstTransitionLayout, finalLayout, m_TotalMipLevels, desc.layers);
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

        void VKTexture::initMipViewsPerFace()
        {
            // TODO: for each face per mip we have a 2D image view, if user wants' a more complicated view for
            // instance a 2DArray view from layer 2:4 and mips 2:11 they need to use the explicit API
            // The current setup is just to read each view as a 2D image

            for (u32 l = 0; l < m_Desc.layers; l++) {
                for (u32 m = 1; m < m_TotalMipLevels; m++) {
                    auto imageView = VKUtilities::CreateImageView(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_TotalMipLevels, VKUtilities::TextureTypeToVKViewType(TextureType::k2D), m_AspectBit, 1, l, m);
                    if ((m_ResourceViewHint & kSRV) == kSRV)
                        m_LayerMipResourceViews[l][m - 1].srv = imageView;
                    if ((m_ResourceViewHint & kUAV) == kUAV)
                        m_LayerMipResourceViews[l][m - 1].uav = imageView;
                }
            }
        }

        //-----------------------------------------------------------------------------------
    }    // namespace Gfx
}    // namespace Razix
