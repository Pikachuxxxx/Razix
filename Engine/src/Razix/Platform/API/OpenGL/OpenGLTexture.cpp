#include "rzxpch.h"
#include "OpenGLTexture.h"

#include "Razix/Utilities/LoadImage.h"
#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

namespace Razix {
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Texture2D
        //-----------------------------------------------------------------------------------

        OpenGLTexture2D::OpenGLTexture2D(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode)
        {
            //? Find out why these variables cannot be allocated in the initializers list
            m_Name              = name;
            m_Width             = width;
            m_Height            = height;
            m_Format            = format;
            m_FilterMode        = filterMode;
            m_WrapMode          = wrapMode;
            m_VirtualPath       = "";

            m_Handle = load(data);
        }

        OpenGLTexture2D::OpenGLTexture2D(const std::string& filePath, const std::string& name,  Wrapping wrapMode, Filtering filterMode)
        {
            m_Name = name;
            m_FilterMode = filterMode;
            m_WrapMode = wrapMode;
            m_VirtualPath = "";

            m_Handle = load(nullptr);
        }


        void OpenGLTexture2D::Bind(uint32_t slot)
        {
            GLCall(glActiveTexture(GL_TEXTURE0 + slot));
            GLCall(glBindTexture(GL_TEXTURE_2D, m_Handle));
        }

        void OpenGLTexture2D::Unbind(uint32_t slot)
        {
            GLCall(glActiveTexture(GL_TEXTURE0 + slot));
            GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        }

        void OpenGLTexture2D::SetData(const void* pixels)
        {
            GLCall(glBindTexture(GL_TEXTURE_2D, m_Handle));
            GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, Graphics::Utilities::OpenGL::TextureFormatToGL(m_Format, true), GL_UNSIGNED_BYTE, pixels));
            GLCall(glGenerateMipmap(GL_TEXTURE_2D));
        }

        uint32_t OpenGLTexture2D::load(void* data)
        {
            uint8_t* pixels = nullptr;

            if (data != nullptr) {
                pixels = reinterpret_cast<uint8_t*>(data);
            }
            else {
                if (m_VirtualPath != "" && m_VirtualPath != "NULL") {
                    uint32_t bpp;
                    // Width and Height are extracted here
                    pixels = Razix::Utilities::LoadImageData(m_VirtualPath, &m_Width, &m_Height, &bpp);
                    // Here the format for the texture is extracted based on bits per pixel
                    m_Format = Razix::Graphics::RZTexture::bitsToTextureFormat(bpp);
                }
            }

            uint32_t handle;
            GLCall(glGenTextures(1, &handle));
            GLCall(glBindTexture(GL_TEXTURE_2D, handle));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_FilterMode.minFilter == Filtering::FilterMode::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_FilterMode.magFilter == Filtering::FilterMode::LINEAR ? GL_LINEAR : GL_NEAREST));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Graphics::Utilities::OpenGL::TextureWrapToGL(m_WrapMode)));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Graphics::Utilities::OpenGL::TextureWrapToGL(m_WrapMode)));

            uint32_t format = Graphics::Utilities::OpenGL::TextureFormatToGL(m_Format, true);
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, Graphics::Utilities::OpenGL::TextureFormatToInternalFormat(format), GL_UNSIGNED_BYTE, data ? data : NULL));
            GLCall(glGenerateMipmap(GL_TEXTURE_2D));

            return handle;
        }
    }
}