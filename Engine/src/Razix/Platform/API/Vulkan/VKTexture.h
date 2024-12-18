#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Gfx/RHI/API/RZTexture.h"

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {

        // TODO: Just as we generate one image view per mip do the same per each layer

        struct ImageResourceView
        {
            VkImageView uav = VK_NULL_HANDLE;
            VkImageView srv = VK_NULL_HANDLE;
            VkImageView rtv = VK_NULL_HANDLE;    // for swapchain
            VkImageView dsv = VK_NULL_HANDLE;

            void destroy() const
            {
                if (uav != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), uav, nullptr);

                if (srv != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), srv, nullptr);

                if (rtv != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), rtv, nullptr);

                if (dsv != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), dsv, nullptr);
            }
        };

        /* Vulkan implementation of the RZTexture class */
        class VKTexture final : public RZTexture
        {
            // Static Helpers
        public:
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
            static void CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG);

    #if RAZIX_USE_VMA
            /**
             * Creates a Vulkan Image handle using VMA
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
             * @param vmaAllocation VMA memory allocation handle
             */
            static void CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VmaAllocation& vmaAllocation, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG);
    #endif

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
            static VkImageView CreateImageView(VkImage image, VkFormat format, u32 mipLevels, VkImageViewType viewType, VkImageAspectFlags aspectMask, u32 layerCount, u32 baseArrayLayer = 0, u32 baseMipLevel = 0 RZ_DEBUG_NAME_TAG_E_ARG RZ_DEBUG_NAME_TAG_STR_S_ARG(= "someImageView! NAME IT !!! LAZY ASS MF#$"));

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
            static VkSampler CreateImageSampler(VkFilter magFilter = VK_FILTER_LINEAR, VkFilter minFilter = VK_FILTER_LINEAR, f32 minLod = 0.0f, f32 maxLod = 1.0f, bool anisotropyEnable = false, f32 maxAnisotropy = 1.0f, VkSamplerAddressMode modeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE RZ_DEBUG_NAME_TAG_E_ARG RZ_DEBUG_NAME_TAG_STR_S_ARG(= "someImageSampler! NAME IT !!! LAZY ASS MF#$"));

            /**
             * Generates mips for the give VkImage
             * 
             * @param image the Vulkan image to generate mips for
             * @param imageFormat the format of the vkImage
             * @param texWidth the width of the image
             * @param texHeight the height of the image
             * @param mipLevels the number of mips to generate
             * @param levels the Array length/Depth/Height of the image
             */
            static void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, u32 mipLevels, u32 layers = 1);

        public:
            VKTexture(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            /* Resize the texture mostly useful for RTs and DRTs */
            virtual void Resize(u32 width, u32 height) override;
            /* Gets the handle to the Vulkan texture i.e. vkImage handle */
            virtual void* GetAPIHandlePtr() const override { return (void*) &m_Image; }
            // TODO: Add support for reading z/array layer
            /* Reads the pixels from the Image (supports only 2D as of now!) in a particular mip */
            virtual int32_t ReadPixels(u32 x, u32 y) override;
            /* Generated mips maps per face */
            virtual void GenerateMips() override;
            /* Updates into the global bindless pool */
            virtual void UploadToBindlessSet() override;

            void transitonImageLayoutToSRV();
            void transitonImageLayoutToUAV();

            RAZIX_INLINE VkImageLayout getImageLayout() const { return m_ImageLayout; }
            RAZIX_INLINE void          setImageLayout(VkImageLayout layout) { m_ImageLayout = layout; }
            RAZIX_INLINE VkImage       getImage() const { return m_Image; };
            RAZIX_INLINE VkImageView   getSRVImageView() const { return m_ResourceViews[m_BaseArrayLayer][m_CurrentMipRenderingLevel].srv; }
            RAZIX_INLINE VkImageView   getUAVImageView() const { return m_ResourceViews[m_BaseArrayLayer][m_CurrentMipRenderingLevel].uav; }
            RAZIX_INLINE VkImageView   getDSVImageView() const { return m_ResourceViews[m_BaseArrayLayer][m_CurrentMipRenderingLevel].dsv; }
            RAZIX_INLINE VkImageView   getRTVImageView() const { return m_ResourceViews[m_BaseArrayLayer][m_CurrentMipRenderingLevel].rtv; }
            RAZIX_INLINE VkImageView   getSRVImageView(u32 layer, u32 mip) const { return m_ResourceViews[layer][mip].srv; }
            RAZIX_INLINE VkImageView   getUAVImageView(u32 layer, u32 mip) const { return m_ResourceViews[layer][mip].uav; }
            RAZIX_INLINE VkImageView   getDSVImageView(u32 layer, u32 mip) const { return m_ResourceViews[layer][mip].dsv; }
            RAZIX_INLINE VkImageView   getRTVImageView(u32 layer, u32 mip) const { return m_ResourceViews[layer][mip].rtv; }
            RAZIX_DEPRECATED("VkSampler is depricated in VKTexture class, it will be separated soon!")
            RAZIX_INLINE VkSampler      getSampler() const { return m_ImageSampler; }
            RAZIX_INLINE VkDeviceMemory getDeviceMemory() const { return m_ImageMemory; }
    #if RAZIX_USE_VMA
            RAZIX_INLINE VmaAllocation getVMAAllocation() const { return m_VMAAllocation; }
    #endif

        private:
            VkImage m_Image = VK_NULL_HANDLE;
            RAZIX_DEPRECATED("vkSampler is depricated in VKTexture class, it will be separated soon!")
            VkSampler             m_ImageSampler                                                    = VK_NULL_HANDLE;
            ImageResourceView     m_ResourceViews[RAZIX_MAX_TEXTURE_LAYERS][RAZIX_MAX_TEXTURE_MIPS] = {};
            VkImageLayout         m_ImageLayout                                                     = VK_IMAGE_LAYOUT_UNDEFINED;
            bool                  m_DeleteImageData                                                 = false;
            VkImageAspectFlagBits m_AspectBit                                                       = VK_IMAGE_ASPECT_NONE;
            VkDeviceMemory        m_ImageMemory                                                     = VK_NULL_HANDLE;
    #if RAZIX_USE_VMA
            VmaAllocation m_VMAAllocation = {};
    #endif

        private:
            // specialized constructor for swapchain class only
            friend class VKSwapchain;
            VKTexture(VkImage image, VkImageView imageView);

            // split into more does one thing functions
            void initializeBackendHandles(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            void loadImageDataFromFile();
        };
    }    // namespace Gfx
}    // namespace Razix

#endif
