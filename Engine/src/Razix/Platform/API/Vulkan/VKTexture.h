#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

#include "Razix/Graphics/API/Texture.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        class VKTexture2D : public RZTexture2D
        {
        public:
            VKTexture2D(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode);
            VKTexture2D(const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode);
            VKTexture2D(VkImage image, VkImageView imageView);
            ~VKTexture2D();

            void Bind(uint32_t slot) override {}
            void Unbind(uint32_t slot) override {}

            void* GetHandle() const override { return (void*) &m_Descriptor; }
            void SetData(const void* pixels) override {}

            void updateDescriptor();

            VkImageLayout getLayout() { return m_ImageLayout; }
            void setImageLayout(VkImageLayout layout) { m_ImageLayout = layout; updateDescriptor(); }
            VkDescriptorImageInfo& getDescriptor() { return m_Descriptor; }
            VkImage getImage() const { return m_TextureImage;};
            VkDeviceMemory getMemory() const { return m_TextureImageMemory; }
            VkImageView getImageView() const { return m_TextureImageView; }
            VkSampler getSampler() const { return m_TextureSampler; }
        private:
            VkImage                 m_TextureImage{};
            VkImageLayout           m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkDeviceMemory          m_TextureImageMemory{};
            VkImageView             m_TextureImageView;
            VkSampler               m_TextureSampler{};
            VkDescriptorImageInfo   m_Descriptor{};
            uint8_t*                m_data;

        private:
            bool load();
        };
    }
}


#endif