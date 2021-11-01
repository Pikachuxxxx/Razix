#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

#include "Razix/Graphics/API/Texture.h"

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
            VKTexture2D(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode);
            /**
             * Creates a 2D Vulkan texture
             * 
             * @param filePath The path to the texture file
             * @param name The name of the texture object
             * @param wrapMode The wrapping mode of the texture
             * @param filterMode The filtering to use for the texture
             */
            VKTexture2D(const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode);
            ~VKTexture2D() {}

            /* Binds the texture object to the given slot */
            void Bind(uint32_t slot) override {}
            /* Unbinds the texture object to the given slot */
            void Unbind(uint32_t slot) override {}

            /* Releases the vulkan texture resources */
            void Release() override;

            /* Gets the handle to the Vulkan texture i.e. Vulkan Image Descriptor */
            void* GetHandle() const override { return (void*) &m_Descriptor; }
            /* Sets the texture with the given pixel data */
            void SetData(const void* pixels) override {}

            /* Updates the descriptor about Vulkan image, it's sampler, View and layout */
            void updateDescriptor();

            /* Gets the layout of the image ex. depth or optimal etc. */
            VkImageLayout getLayout() { return m_ImageLayout; }
            /* Sets the vulkan image layout */
            void setImageLayout(VkImageLayout layout) { m_ImageLayout = layout; updateDescriptor(); }
            /* Gets the descriptor info about the Vulkan Texture object */
            VkDescriptorImageInfo& getDescriptor() { return m_Descriptor; }
            /* Gets the vulkan image object */
            VkImage getImage() const { return m_Image;};
            /* Gets the Vulkan image memory handle */
            VkDeviceMemory getMemory() const { return m_ImageMemory; }
            /* Gets the image view of the Vulkan image */
            VkImageView getImageView() const { return m_ImageView; }
            /* Gets the sampler for the Vulkan image */
            VkSampler getSampler() const { return m_ImageSampler; }

        private:
            VkImage                 m_Image;                                        /* Vulkan image handle for the Texture object                               */
            VkDeviceMemory          m_ImageMemory;                                  /* Memory for the Vulkan image                                              */
            VkImageView             m_ImageView;                                    /* Image view for the image, all images need a view to look into the image  */
            VkSampler               m_ImageSampler;                                 /* Sampler information used by shaders to sample the texture                */
            VkDescriptorImageInfo   m_Descriptor;                                   /* Descriptor info encapsulation the image, view and the sampler            */
            VkImageLayout           m_ImageLayout   = VK_IMAGE_LAYOUT_UNDEFINED;    /* Layout aka usage description of the image                                */
            uint8_t*                m_data          = nullptr;                      /* Texture data with which the image is create with                         */

        private:
            /* Creates the 2D Texture--> Image, view, sampler and performs layout transition and staged buffer copy operations */
            bool load();
        };
    }
}


#endif