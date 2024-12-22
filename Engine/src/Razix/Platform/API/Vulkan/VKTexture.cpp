// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKTexture.h"

#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Utilities/RZColorUtilities.h"
#include "Razix/Utilities/RZLoadImage.h"

#include <vendor/stb/stb_image_write.h>

namespace Razix {
    namespace Gfx {

        //-----------------------------------------------------------------------------------
        // VKTexture
        //-----------------------------------------------------------------------------------

        VKTexture::VKTexture(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Desc = desc;

            // updates the IRZResource with initialization time hints
            setResourceViewHints(m_Desc.initResourceViewHints);

            evaluateMipsCount();

            initializeBackendHandles(m_Desc RZ_DEBUG_E_ARG_NAME);
        }

        // Special swapchain class helper constructor
        VKTexture::VKTexture(VkImage image, VkImageView imageView)
            : m_Image(image)
        {
            // This way of creating usually means one this, it's a SWAPCHAIN IMAGE
            m_Desc.name        = "$RazixSwapchainBackBuffer$";
            m_Desc.type        = TextureType::kSwapchainImage;
            m_Desc.format      = TextureFormat::SCREEN;
            m_FinalImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            m_FullResourceView.rtv = imageView;
        }

        //-------------------------------------------------------------------------------------------

        RAZIX_CLEANUP_RESOURCE_IMPL(VKTexture)
        {
            // swapchain images are destroyed using VkDestroySwapchainKHR
            if (m_Desc.type == TextureType::kSwapchainImage)
                return;

            m_FullResourceView.destroy();

            for (u32 l = 1; l < m_Desc.layers; l++) {
                for (u32 m = 1; m < m_TotalMipLevels; m++) {
                    m_LayerMipResourceViews[l][m].destroy();
                }
            }

#if !RAZIX_USE_VMA
            if (m_Image != VK_NULL_HANDLE)
                vkDestroyImage(VKDevice::Get().getDevice(), m_Image, nullptr);

            if (m_ImageMemoryWrapper.nativeAllocation != VK_NULL_HANDLE)
                vkFreeMemory(VKDevice::Get().getDevice(), m_ImageMemoryWrapper.nativeAllocation, nullptr);
#else
            vmaDestroyImage(VKDevice::Get().getVMA(), m_Image, m_ImageMemoryWrapper.vmaAllocation);
#endif
            m_CurrentMipRenderingLevel = 0;
            m_BaseArrayLayer           = 0;
        }

        //-------------------------------------------------------------------------------------------

        void VKTexture::Resize(u32 width, u32 height)
        {
            m_Desc.width  = width;
            m_Desc.height = height;

            DestroyResource();

            initializeBackendHandles(m_Desc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));
        }

        void VKTexture::GenerateMipsAndViews()
        {
            m_Desc.enableMips = true;
            m_TotalMipLevels  = static_cast<u32>(std::floor(std::log2(std::max(m_Desc.width, m_Desc.height)))) + 1;

            VKUtilities::GenerateMipmaps(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_Desc.width, m_Desc.height, m_TotalMipLevels, m_Desc.layers);

            createMipViewsPerFace();

            m_OldImageLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            m_FinalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        void VKTexture::UploadToBindlessSet()
        {
            // Now if Bindless is available Bind to it
            if (VKDevice::Get().isBindlessSupported()) {
                RZDescriptor descriptor = {};

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = m_FinalImageLayout;
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
                region.imageSubresource.aspectMask     = m_AspectFlags;
                region.imageSubresource.mipLevel       = m_CurrentMipRenderingLevel;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = {m_Desc.width, m_Desc.height, 1};

                vkCmdCopyImageToBuffer(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_TransferBuffer.getBuffer(), 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            char* data = new char[m_Desc.width * m_Desc.height * RAZIX_TEXTURE_BITS_PER_PIXEL];
            m_TransferBuffer.map();
            data = (char*) m_TransferBuffer.getMappedRegion();
            // Read and return the pixel value
            int32_t pixel_value = ((int32_t*) data)[(x) + (m_Desc.width * y)];
            m_TransferBuffer.unMap();
            delete[] data;

            return pixel_value;
        }

        //-------------------------------------------------------------------------------------------

        void VKTexture::transitonImageLayoutToSRV()
        {
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_FinalImageLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_TotalMipLevels, m_Desc.layers);
            setImageLayoutValue(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        void VKTexture::transitonImageLayoutToUAV()
        {
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_FinalImageLayout, VK_IMAGE_LAYOUT_GENERAL, m_TotalMipLevels, m_Desc.layers);
            setImageLayoutValue(VK_IMAGE_LAYOUT_GENERAL);
        }

        //-------------------------------------------------------------------------------------------

        void VKTexture::initializeBackendHandles(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            // swapchain images are destroyed using VkDestroySwapchainKHR
            if (m_Desc.type == TextureType::kSwapchainImage)
                return;

            if (bufferName == "$UNNAMED_TEXTURE_RESOURCE")
                RAZIX_DEBUG_BREAK();

            if (!desc.filePath.empty())
                loadImageDataInfoFromFile();

            RAZIX_ASSERT_MESSAGE(desc.width && desc.height, "[VULKAN] cannot create texture with null width/height.");

            if (isDepthFormat() && ((m_ResourceViewHint & kDSV) != kDSV))
                RAZIX_ASSERT_MESSAGE(false, "[Vulkan] image is depth depth format but doesn't have a kDSV resource hint! Cannot view the image until a proper resourve view is set");

            VkImageCreateFlags flags = resolveCreateFlags();

            VkImageUsageFlags usageFlags = resolveUsageFlags();

            m_AspectFlags = resolveAspectFlags();

            createVkImageHandleWithDesc(flags, usageFlags RZ_DEBUG_E_ARG_NAME);

            //  There are two transitions we need to handle:
            //      1. Undefined -> transfer destination: transfer writes that don't need to wait on anything
            //          1.1 Copy image from transfer staging buffer to the Image buffer on DEVICE
            //      2. Transfer destination -> shader reading: shader reads should wait on transfer writes, specifically the shader reads in the fragment shader, because that's where we're going to use the texture
            if (desc.data) {
                loadImageDataFromFile();
                delete (u8*) m_Desc.data;
            }

            // initial layout transition to a generic state
            m_OldImageLayout = VK_IMAGE_LAYOUT_GENERAL;
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, m_OldImageLayout, m_TotalMipLevels, m_Desc.layers);

            if (m_Desc.enableMips)
                GenerateMipsAndViews();

            if (desc.type == TextureType::kRWCubeMap) {
                createSpecializedRWCubemapViews();
            } else
                createFullResourceViews();

            if (m_OldImageLayout != m_FinalImageLayout) {
                VkImageLayout m_FinalImageLayout = (m_ResourceViewHint & kUAV) == kUAV ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), m_OldImageLayout, m_FinalImageLayout, m_TotalMipLevels, desc.layers);
            }
        }

        void VKTexture::loadImageDataInfoFromFile()
        {
            if (m_Desc.filePath.empty())
                return;

            if (m_Desc.dataSize == sizeof(float))
                m_Desc.data = (u8*) Razix::Utilities::LoadImageDataFloat(m_Desc.filePath, &m_Desc.width, &m_Desc.height, &m_BitsPerPixel);
            else
                m_Desc.data = Razix::Utilities::LoadImageData(m_Desc.filePath, &m_Desc.width, &m_Desc.height, &m_BitsPerPixel, m_Desc.flipY);
            // Here the format for the texture is extracted based on bits per pixel
            m_Desc.format = Razix::Gfx::RZTexture::BitsToTextureFormat(RAZIX_TEXTURE_BITS_PER_PIXEL);    // everything is a 4-byte by default
            m_Desc.size   = static_cast<u32>(m_Desc.width * m_Desc.height * RAZIX_TEXTURE_BITS_PER_PIXEL * m_Desc.dataSize);
        }

        void VKTexture::evaluateMipsCount()
        {
            if (m_Desc.enableMips)
                m_TotalMipLevels = RZTexture::CalculateMipMapCount(m_Desc.width, m_Desc.height);
        }

        VkImageUsageFlags VKTexture::resolveUsageFlags()
        {
            VkImageUsageFlags usageFlags = {};
            if (m_Desc.format == TextureFormat::DEPTH32F || m_Desc.format == TextureFormat::DEPTH16_UNORM || m_Desc.format == TextureFormat::DEPTH_STENCIL)
                usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            if ((usageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT && m_ResourceViewHint != kDSV) {
                RAZIX_CORE_ERROR("[Vulkan] Depth Texture is being created without a DSV hint, depth write image view will cause a crash! provide correct resource view hint during texture initialization");
                RAZIX_DEBUG_BREAK();
            }

            // if the texture is a RW resource we add an extra flag to enable writing to it from shaders, we also check if it can be a UAV, did the user specify the enable flag
            if ((m_ResourceViewHint & kUAV) == kUAV) usageFlags = VkImageUsageFlagBits(VK_IMAGE_USAGE_STORAGE_BIT | usageFlags);

            constexpr VkImageUsageFlagBits additionalDefaultFlagBits = VkImageUsageFlagBits(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

            return VkImageUsageFlags(usageFlags | additionalDefaultFlagBits);
        }

        VkImageCreateFlags VKTexture::resolveCreateFlags()
        {
            VkImageCreateFlags flags = 0;
            if (m_Desc.type == TextureType::kCubeMap || m_Desc.type == TextureType::kCubeMapArray || m_Desc.type == TextureType::kRWCubeMap)
                flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            // https://www.reddit.com/r/vulkan/comments/pc87in/whats_the_correct_way_to_create_a_2d_image_array/
            // TODO: Investigate if this is necessary and enable it later
            // VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
            return flags;
        }

        VkImageAspectFlags VKTexture::resolveAspectFlags()
        {
            VkImageAspectFlags aspectFlags = {};
            if (isDepthFormat())
                aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
            else
                aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

            return aspectFlags;
        }

        void VKTexture::createVkImageHandleWithDesc(VkImageCreateFlags flags, VkImageUsageFlags usageFlags RZ_DEBUG_NAME_TAG_E_ARG)
        {
            VKUtilities::VKCreateImageDesc imageDesc = {};
            imageDesc.width                          = m_Desc.width;
            imageDesc.height                         = m_Desc.height;
            imageDesc.depth                          = m_Desc.depth;
            imageDesc.mipLevels                      = m_TotalMipLevels;
            imageDesc.arrayLayers                    = m_Desc.layers;
            imageDesc.format                         = VKUtilities::TextureFormatToVK(m_Desc.format);
            imageDesc.imageType                      = VKUtilities::TextureTypeToVK(m_Desc.type);
            imageDesc.tiling                         = VK_IMAGE_TILING_OPTIMAL;
            imageDesc.usage                          = usageFlags;
            imageDesc.properties                     = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            imageDesc.flags                          = flags;

            auto handles = VKUtilities::CreateImageMemoryHandles(imageDesc RZ_DEBUG_E_ARG_NAME);

            m_Image              = handles.image;
            m_ImageMemoryWrapper = handles.memoryWrapper;
        }

        void VKTexture::loadImageDataFromFile()
        {
            m_OldImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            VKUtilities::TransitionImageLayout(m_Image, VKUtilities::TextureFormatToVK(m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, m_OldImageLayout, m_TotalMipLevels, m_Desc.layers);
            VKUtilities::CopyDataToGPUTextureResource(m_Desc.data, m_Image, m_Desc.width, m_Desc.height, m_Desc.size, m_CurrentMipRenderingLevel, m_Desc.layers, m_BaseArrayLayer);
        }

        void VKTexture::createSpecializedRWCubemapViews()
        {
            // We handle a special case here for RWTextureCube, we create both a SRV and UAV with different settings
            // Create the Image view for every layer at 0 mip as default Shader Resource View
            // We store the initial full resource view
            VKUtilities::VKCreateImageViewDesc imageViewDesc = {};
            imageViewDesc.image                              = m_Image;
            imageViewDesc.format                             = VKUtilities::TextureFormatToVK(m_Desc.format);
            imageViewDesc.aspectMask                         = m_AspectFlags;
            imageViewDesc.mipLevels                          = m_TotalMipLevels;
            imageViewDesc.baseMipLevel                       = RAZIX_TEXTURE_DEFAULT_MIP_IDX;
            imageViewDesc.layerCount                         = m_Desc.layers;
            imageViewDesc.baseArrayLayer                     = RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER;

            if ((m_ResourceViewHint & kSRV) == kSRV) {
                imageViewDesc.viewType = VKUtilities::TextureTypeToVKViewType(TextureType::kCubeMap);
                m_FullResourceView.srv = VKUtilities::CreateImageView(imageViewDesc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));
            }
            if ((m_ResourceViewHint & kUAV) == kUAV) {
                imageViewDesc.viewType = VKUtilities::TextureTypeToVKViewType(TextureType::kRW2DArray);
                m_FullResourceView.uav = VKUtilities::CreateImageView(imageViewDesc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));
            }
        }

        void VKTexture::createFullResourceViews()
        {
            // TODO: Use this methods for further customization, currently a single setup works for all resource view types
            // we might need to customize the aspect and create flags for more restrictive behavior

            VKUtilities::VKCreateImageViewDesc imageViewDesc = {};
            imageViewDesc.image                              = m_Image;
            imageViewDesc.format                             = VKUtilities::TextureFormatToVK(m_Desc.format);
            imageViewDesc.viewType                           = VKUtilities::TextureTypeToVKViewType(m_Desc.type);
            imageViewDesc.aspectMask                         = m_AspectFlags;
            imageViewDesc.mipLevels                          = m_TotalMipLevels;
            imageViewDesc.baseMipLevel                       = RAZIX_TEXTURE_DEFAULT_MIP_IDX;
            imageViewDesc.layerCount                         = m_Desc.layers;
            imageViewDesc.baseArrayLayer                     = RAZIX_TEXTURE_DEFAULT_ARRAY_LAYER;

            if ((m_ResourceViewHint & kSRV) == kSRV)
                m_FullResourceView.srv = VKUtilities::CreateImageView(imageViewDesc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));
            if ((m_ResourceViewHint & kUAV) == kUAV)
                m_FullResourceView.uav = VKUtilities::CreateImageView(imageViewDesc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));    // TODO: make sure it has storage flags bit set
            if ((m_ResourceViewHint & kDSV) == kDSV)
                m_FullResourceView.dsv = VKUtilities::CreateImageView(imageViewDesc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));    // TODO: make sure it has depth bit flags set
        }

        void VKTexture::createMipViewsPerFace()
        {
            // TODO: for each face per mip we have a 2D image view, if user wants' a more complicated view for
            // instance a 2DArray view from layer 2:4 and mips 2:11 they need to use the explicit API
            // The current setup is just to read each view as a 2D image

            VKUtilities::VKCreateImageViewDesc imageViewDesc = {};
            imageViewDesc.image                              = m_Image;
            imageViewDesc.format                             = VKUtilities::TextureFormatToVK(m_Desc.format);
            imageViewDesc.viewType                           = VKUtilities::TextureTypeToVKViewType(TextureType::k2D);    // all mips/layer views are just 2D images
            imageViewDesc.aspectMask                         = m_AspectFlags;
            imageViewDesc.layerCount                         = 1;    // since we just have a single 2D image per mip per face

            for (u32 l = 0; l < m_Desc.layers; l++) {
                for (u32 m = 0; m < m_TotalMipLevels; m++) {
                    imageViewDesc.baseMipLevel   = m;
                    imageViewDesc.baseArrayLayer = l;
                    imageViewDesc.mipLevels      = m == 0 ? m_TotalMipLevels : 1;    // only views at 0 mip stores all mips together
                    auto imageView               = VKUtilities::CreateImageView(imageViewDesc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));
                    if ((m_ResourceViewHint & kSRV) == kSRV)
                        m_LayerMipResourceViews[l][m].srv = imageView;
                    if ((m_ResourceViewHint & kUAV) == kUAV)
                        m_LayerMipResourceViews[l][m].uav = imageView;
                }
            }
        }

        bool VKTexture::isDepthFormat()
        {
            if (m_Desc.format == TextureFormat::DEPTH32F || m_Desc.format == TextureFormat::DEPTH16_UNORM || m_Desc.format == TextureFormat::DEPTH_STENCIL)
                return true;
            else
                return false;
        }

        bool VKTexture::isRWImage()
        {
            if (m_Desc.type == TextureType::kRW1D || m_Desc.type == TextureType::kRW2D || m_Desc.type == TextureType::kRW2DArray || m_Desc.type == TextureType::kRW3D || m_Desc.type == TextureType::kRWCubeMap)
                return true;
            else
                return false;
        }

        //-----------------------------------------------------------------------------------
    }    // namespace Gfx
}    // namespace Razix
