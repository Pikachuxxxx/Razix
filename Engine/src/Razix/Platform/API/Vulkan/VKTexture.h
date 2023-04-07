#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Graphics/RHI/API/RZTexture.h"

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* Vulkan implementation of the RZTexture2D class */
        // TODO: Add fail logging for VK_CHECK_RESULT calls
        // TODO: Add successful creation logs
        class VKTexture2D : public RZTexture2D
        {
        public:
            /**
             * Creates a 2D Vulkan texture with given raw pixel data
             * 
             * @param name The name of the texture object
             * @param width The width of the texture
             * @param height The height of the texture
             * @param data The data with which the texture will be filled with
             * @param format The format of the texture
             * @param wrapMode The wrapping mode of the texture
             * @param filterMode The filtering to use for the texture
             */
            VKTexture2D(const std::string& name, u32 width, u32 height, void* data, Format format, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG);

            /**
             * Creates an Empty 2D array texture
             */
            VKTexture2D(const std::string& name, u32 width, u32 height, u32 numLayers, Format format, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG);

            /**
             * Creates a 2D Vulkan texture
             * 
             * @param filePath The path to the texture file
             * @param name The name of the texture object
             * @param wrapMode The wrapping mode of the texture
             * @param filterMode The filtering to use for the texture
             */
            VKTexture2D(const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG);
            /**
             * Creates a TRXTexture2D from the given image and it's view
             * @brief Used by swapchain and render passes to create their own texture resources
             * What will be the image layout of this?
             * @param image Vulkan image handle
             * @param imageView Vulkan image view handle for the specified image
             */
            VKTexture2D(VkImage image, VkImageView imageView);
            ~VKTexture2D() {}

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

            static void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, u32 mipLevels, u32 layers = 1);

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

            /* Binds the texture object to the given slot */
            void Bind(u32 slot) override {}
            /* Unbinds the texture object to the given slot */
            void Unbind(u32 slot) override {}

            /* Releases the vulkan texture resources */
            void Release(bool deleteImage) override;

            /* Gets the handle to the Vulkan texture i.e. Vulkan Image Descriptor */
            void* GetHandle() const override { return (void*) &m_Descriptor; }
            /* Sets the texture with the given pixel data */
            void SetData(const void* pixels) override {}

            /* Updates the descriptor about Vulkan image, it's sampler, View and layout */
            void updateDescriptor();

            /* Gets the layout of the image ex. depth or optimal etc. */
            VkImageLayout getLayout() { return m_ImageLayout; }
            /* Sets the vulkan image layout */
            void setImageLayout(VkImageLayout layout)
            {
                m_ImageLayout = layout;
                updateDescriptor();
            }
            /* Gets the descriptor info about the Vulkan Texture object */
            VkDescriptorImageInfo getDescriptor() { return m_Descriptor; }
            /* Gets the vulkan image object */
            VkImage getImage() const { return m_Image; };
            /* Gets the Vulkan image memory handle */
            VkDeviceMemory getMemory() const { return m_ImageMemory; }
            /* Gets the image view of the Vulkan image */
            VkImageView getImageView() const { return m_ImageView; }
            /* Gets the sampler for the Vulkan image */
            VkSampler getSampler() const { return m_ImageSampler; }

        private:
            VkImage               m_Image;                                   /* Vulkan image handle for the Texture object                               */
            VkDeviceMemory        m_ImageMemory;                             /* Memory for the Vulkan image                                              */
            VkImageView           m_ImageView;                               /* Image view for the image, all images need a view to look into the image  */
            VkSampler             m_ImageSampler;                            /* Sampler information used by shaders to sample the texture                */
            VkDescriptorImageInfo m_Descriptor;                              /* Descriptor info encapsulation the image, view and the sampler            */
            VkImageLayout         m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED; /* Layout aka usage description of the image                                */
            u8*                   m_data        = nullptr;                   /* Texture data with which the image is create with                         */

        private:
            /* Creates the 2D Texture--> Image, view, sampler and performs layout transition and staged buffer copy operations */
            bool load(RZ_DEBUG_NAME_TAG_S_ARG);
        };

        //-----------------------------------------------------------------------------------
        // Texture3D
        //-----------------------------------------------------------------------------------

        class VKTexture3D : public RZTexture3D
        {
        public:
            /* This is used to generate cube map from equirectangular maps or multiple texture files */
            VKTexture3D(const std::string& name, u32 width, u32 height, u32 depth, Format format, Wrapping wrapMode, Filtering filterMode RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKTexture3D() {}

            void  Release(bool deleteImage = true) override;
            void  Bind(u32 slot) override;
            void  Unbind(u32 slot) override;
            void* GetHandle() const override;

            /* Gets the vulkan image object */
            VkImage getImage() const { return m_Image; };

        private:
            VkImage               m_Image;        /* Vulkan image handle for the Texture object                               */
            VkDeviceMemory        m_ImageMemory;  /* Memory for the Vulkan image                                              */
            VkImageView           m_ImageView;    /* Image view for the image, all images need a view to look into the image  */
            VkSampler             m_ImageSampler; /* Sampler information used by shaders to sample the texture                */
            VkImageLayout         m_ImageLayout;  /* Layout aka usage description of the image                                */
            VkDescriptorImageInfo m_Descriptor;   /* Descriptor info encapsulation the image, view and the sampler            */

        private:
            /* Updates the descriptor about Vulkan image, it's sampler, View and layout */
            void updateDescriptor();
        };

        //-----------------------------------------------------------------------------------
        // CubeMap Texture
        //-----------------------------------------------------------------------------------

        class VKCubeMap : public RZCubeMap
        {
        public:
            /* This is used to generate cube map from equirectangular maps or multiple texture files */
            VKCubeMap(const std::string& hdrFilePath, const std::string& name, Wrapping wrapMode, Filtering filterMode);
            /* This is used to create a empty cube map that can be used to write data into */
            VKCubeMap(const std::string& name, u32 width, u32 height, bool enableMipsGeneration, Wrapping wrapMode, Filtering filterMode);
            ~VKCubeMap() {}

            void  Release(bool deleteImage = true) override;
            void  Bind(u32 slot) override;
            void  Unbind(u32 slot) override;
            void* GetHandle() const override;

            /* Gets the vulkan image object */
            VkImage getImage() const { return m_Image; };

        private:
            VkImage                            m_Image;        /* Vulkan image handle for the Texture object                                      */
            VkDeviceMemory                     m_ImageMemory;  /* Memory for the Vulkan image                                                     */
            std::vector<VkImageView>           m_ImageViews;   /* Image views for the image, all faces & mips need a view to look into the image  */
            VkSampler                          m_ImageSampler; /* Sampler information used by shaders to sample the texture                       */
            VkImageLayout                      m_ImageLayout;  /* Layout aka usage description of the image                                       */
            std::vector<VkDescriptorImageInfo> m_Descriptors;  /* Descriptors info encapsulation the image, it's views and the sampler            */

        private:
            void convertEquirectangularToCubemap();
            /* Updates the descriptor about Vulkan image, it's sampler, View and layout */
            void updateDescriptor();
        };

        //-----------------------------------------------------------------------------------
        // Depth Texture
        //-----------------------------------------------------------------------------------

        class VKDepthTexture : public RZDepthTexture
        {
        public:
            VKDepthTexture(u32 width, u32 height);
            ~VKDepthTexture();

            void  Resize(u32 width, u32 height RZ_DEBUG_NAME_TAG_E_ARG) override;
            void  Release(bool deleteImage = true) override;
            void  Bind(u32 slot) override;
            void  Unbind(u32 slot) override;
            void* GetHandle() const override;

            /* Gets the vulkan image object */
            VkImage getImage() const { return m_Image; };

        private:
            VkImage               m_Image;        /* Vulkan image handle for the Texture object                               */
            VkDeviceMemory        m_ImageMemory;  /* Memory for the Vulkan image                                              */
            VkImageView           m_ImageView;    /* Image view for the image, all images need a view to look into the image  */
            VkSampler             m_ImageSampler; /* Sampler information used by shaders to sample the texture                */
            VkDescriptorImageInfo m_Descriptor;   /* Descriptor info encapsulation the image, view and the sampler            */

        private:
            void init();
            /* Updates the descriptor about Vulkan image, it's sampler, View and layout */
            void updateDescriptor();
        };

        //-----------------------------------------------------------------------------------
        // Render Texture
        //-----------------------------------------------------------------------------------

        class VKRenderTexture : public RZRenderTexture
        {
        public:
            VKRenderTexture(
                u32 width, u32 height, Format format = RZTexture::Format::SCREEN, Wrapping wrapMode = RZTexture::Wrapping::REPEAT, Filtering filterMode = Filtering {} RZ_DEBUG_NAME_TAG_E_ARG RZ_DEBUG_NAME_TAG_STR_S_ARG(= "some RenderTExture! NAME IT !!! LAZY ASS MF#$"));
            VKRenderTexture(VkImage image, VkImageView imageView);
            ~VKRenderTexture() {}

            void  Resize(u32 width, u32 height RZ_DEBUG_NAME_TAG_E_ARG) override;
            void  Release(bool deleteImage = true) override;
            void  Bind(u32 slot) override;
            void  Unbind(u32 slot) override;
            void* GetHandle() const override;

            int32_t ReadPixels(u32 x, u32 y) override;

            /* Gets the vulkan image object */
            VkImage getImage() const { return m_Image; };

        private:
            VkImage               m_Image;                                   /* Vulkan image handle for the Texture object                               */
            VkDeviceMemory        m_ImageMemory;                             /* Memory for the Vulkan image                                              */
            VkImageView           m_ImageView;                               /* Image view for the image, all images need a view to look into the image  */
            VkImageLayout         m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED; /* Layout aka usage description of the image                                */
            VkSampler             m_ImageSampler;                            /* Sampler information used by shaders to sample the texture                */
            VkDescriptorImageInfo m_Descriptor;                              /* Descriptor info encapsulation the image, view and the sampler            */
            VKBuffer              m_TransferBuffer;

        private:
            /* recreates the render texture */
            void init(RZ_DEBUG_NAME_TAG_S_ARG);
            /* Updates the descriptor about Vulkan image, it's sampler, View and layout */
            void updateDescriptor();
        };
    }    // namespace Graphics
}    // namespace Razix

#endif