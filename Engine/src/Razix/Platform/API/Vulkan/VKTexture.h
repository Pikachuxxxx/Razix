#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Graphics/RHI/API/RZTexture.h"

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        // TODO: Just as we generate one image view per mip do the same per each layer

        /* Vulkan implementation of the RZTexture2D class */
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
    #ifndef RAZIX_USE_VMA
            static void CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG);
    #else
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
             * @param vmaAllocation VMA memory allocation handle
             */
            static void   CreateImage(u32 width, u32 height, u32 depth, u32 mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VmaAllocation& vmaAllocation, u32 arrayLayers, VkImageCreateFlags flags RZ_DEBUG_NAME_TAG_E_ARG);
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
            VKTexture(const RZTextureDesc& desc, const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG);
            VKTexture(VkImage image, VkImageView imageView);
            ~VKTexture() {}

            /* Binds the texture object to the given slot */
            void Bind(u32 slot) override {}
            /* Unbinds the texture object to the given slot */
            void Unbind(u32 slot) override {}

            /* Releases the vulkan texture resources */
            void DestroyResource() override;

            /* Resize the texture mostly useful for RTs and DRTs */
            void Resize(u32 width, u32 height) override;

            /* Gets the handle to the Vulkan texture i.e. Vulkan Image Descriptor */
            void* GetAPIHandlePtr() const override { return (void*) &m_Descriptors[m_CurrentMipRenderingLevel]; }

            // TODO: Add support for reading z/array layer
            /* Reads the pixels from the Image (supports only 2D as of now!) in a particular mip */
            int32_t ReadPixels(u32 x, u32 y) override;

            virtual void GenerateMips() override;

            /* Updates into the global bindless pool */
            void UploadToBindlessSet() override;
            /* Updates the descriptor about Vulkan image, it's sampler, View and layout */
            RAZIX_INLINE void updateDescriptor();
            /* Gets the layout of the image ex. depth or optimal etc. */
            RAZIX_INLINE VkImageLayout getLayout() { return m_ImageLayout; }
            /* Sets the vulkan image layout */
            void setImageLayout(VkImageLayout layout)
            {
                m_ImageLayout = layout;
                updateDescriptor();
                // TODO: Update the Bindless descriptor set
            }
            /* Gets the descriptor info about the Vulkan Texture object in a particular mip level */
            RAZIX_INLINE VkDescriptorImageInfo getDescriptor() { return m_Descriptors[m_CurrentMipRenderingLevel]; }
            /* Gets the vulkan image object */
            RAZIX_INLINE VkImage getImage() const { return m_Image; };
            /* Gets the image view of the Vulkan image in a particular mip level */
            RAZIX_INLINE VkImageView getImageView() const { return m_ImageViews[m_CurrentMipRenderingLevel]; }
            /* Gets the sampler for the Vulkan image */
            RAZIX_INLINE VkSampler getSampler() const { return m_ImageSampler; }
    #ifndef RAZIX_USE_VMA
            /* Gets the Vulkan image memory handle */
            RAZIX_INLINE VkDeviceMemory getMemory() const { return m_ImageMemory; }
    #endif
    #ifdef RAZIX_USE_VMA
            /* Gets the VMA allocation for the buffer */
            RAZIX_INLINE VmaAllocation getVMAAllocation() const { return m_VMAAllocation; }
    #endif

        private:
            VkImage                            m_Image           = VK_NULL_HANDLE;            /* Vulkan image handle for the Texture object                                      */
            VkSampler                          m_ImageSampler    = VK_NULL_HANDLE;            /* Sampler information used by shaders to sample the texture                       */
            std::vector<VkImageView>           m_ImageViews      = {};                        /* Image views for the image, all faces & mips need a view to look into the image  */
            std::vector<VkDescriptorImageInfo> m_Descriptors     = {};                        /* Descriptors info encapsulation the image, it's views and the sampler            */
            VkImageLayout                      m_ImageLayout     = VK_IMAGE_LAYOUT_UNDEFINED; /* Layout aka usage description of the image                                       */
            bool                               m_DeleteImageData = false;                     /* Whether or not to delete image intermediate data                                */
            VkImageAspectFlagBits              m_AspectBit       = VK_IMAGE_ASPECT_NONE;      /* Aspect bit of the image                                                         */
    #ifndef RAZIX_USE_VMA
            VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE; /* Handle to the image memory               */
    #else
            VmaAllocation m_VMAAllocation = {}; /* Holds the VMA allocation state info       */
    #endif

        private:
            /* Creates the 2D Texture--> Image, view, sampler and performs layout transition and staged buffer copy operations */
            bool load(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            void init(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
        };
    }    // namespace Graphics
}    // namespace Razix

#endif