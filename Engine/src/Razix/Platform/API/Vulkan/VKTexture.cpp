#include "rzxpch.h"
#include "VKTexture.h"

#include "Razix/Utilities/LoadImage.h"

namespace Razix { 
    namespace Graphics {
        
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

        }

        VKTexture2D::VKTexture2D(VkImage image, VkImageView imageView)
        {

        }

        VKTexture2D::~VKTexture2D()
        {

        }

        void VKTexture2D::updateDescriptor()
        {

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

            return true;

        }

    }
}
