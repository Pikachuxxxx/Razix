// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLTexture.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"
#include "Razix/Utilities/LoadImage.h"

namespace Razix {
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Texture2D
        //-----------------------------------------------------------------------------------

        OpenGLTexture::OpenGLTexture(const RZTextureDesc& desc)
        {
            //? Find out why these variables cannot be allocated in the initializers list
            m_Desc        = desc;
            m_VirtualPath = "";

            m_Handle = load(desc.data);
        }

        OpenGLTexture::OpenGLTexture(const RZTextureDesc& desc, const std::string& filePath)
        {
            m_Desc = desc;

            m_VirtualPath = filePath;

            m_Handle = load(nullptr);
        }

        void OpenGLTexture::Bind(u32 slot)
        {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, m_Handle));
        }

        void OpenGLTexture::Unbind(u32 slot)
        {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        }

        void OpenGLTexture::SetData(const void* pixels)
        {
            GL_CALL(glBindTexture(GL_TEXTURE_2D, m_Handle));
            GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Desc.width, m_Desc.height, Graphics::OpenGLUtilities::TextureFormatToGL(m_Desc.format, true), GL_UNSIGNED_BYTE, pixels));
            GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        }

        int32_t OpenGLTexture::ReadPixels(u32 x, u32 y)
        {
            return 0;
        }

        u32 OpenGLTexture::load(void* data)
        {
            u8* pixels = nullptr;

            if (data != nullptr)
                pixels = reinterpret_cast<u8*>(data);
            else {
                if (m_VirtualPath != "" && m_VirtualPath != "NULL") {
                    u32 bpp;
                    // Width and Height are extracted here
                    pixels = Razix::Utilities::LoadImageData(m_VirtualPath, &m_Desc.width, &m_Desc.height, &bpp);
                    // Here the format for the texture is extracted based on bits per pixel
                    m_Desc.format = Razix::Graphics::RZTexture::bitsToTextureFormat(bpp);
                    // Fill the size of the texture image
                    m_Size = m_Desc.width * m_Desc.height * bpp;
                }
            }

            RAZIX_CORE_ASSERT((pixels != nullptr), "[OpenGL] Failed to load Texture data! Name : {0}", m_Desc.name);

            u32 handle;
            GL_CALL(glGenTextures(1, &handle));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, handle));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Desc.filtering.minFilter == Filtering::Mode::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Desc.filtering.magFilter == Filtering::Mode::LINEAR ? GL_LINEAR : GL_NEAREST));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Graphics::OpenGLUtilities::TextureWrapToGL(m_Desc.wrapping)));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Graphics::OpenGLUtilities::TextureWrapToGL(m_Desc.wrapping)));

            u32 format = Graphics::OpenGLUtilities::TextureFormatToGL(m_Desc.format, true);
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, format, m_Desc.width, m_Desc.height, 0, Graphics::OpenGLUtilities::TextureFormatToInternalFormat(format), GL_UNSIGNED_BYTE, pixels));
            GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

            return handle;
        }
    }    // namespace Graphics
}    // namespace Razix