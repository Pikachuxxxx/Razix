#include "rzxpch.h"
#include "Texture.h"

#include "Razix/Graphics/API/GraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/API/OpenGL/OpenGLTexture.h"
#endif

namespace Razix {
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Texture
        //-----------------------------------------------------------------------------------

        uint32_t RZTexture::calculateMipMapCount(uint32_t width, uint32_t height)
        {
            //tex: The texture mip levels are $numLevels = 1 + floor(log2(max(w, h, d)))$
            uint32_t levels = 1;
            while ((width | height) >> levels)
                levels++;

            return levels;
        }

        RZTexture::Format RZTexture::bitsToTextureFormat(uint32_t bits)
        {
            switch (bits) {
                case 8:
                    return  RZTexture::Format::R8;
                case 16:
                    return  RZTexture::Format::RG8;
                case 24:
                    return  RZTexture::Format::RGB8;
                case 32:
                    return  RZTexture::Format::RGBA8;
                case 48:
                    return  RZTexture::Format::RGB16;
                case 64:
                    return  RZTexture::Format::RGBA16;
                default:
                    RAZIX_CORE_ASSERT(false, "[Texture] Unsupported image bit-depth! ({0})", bits);
                    return RZTexture::Format::RGB8;
            }
        }

        //-----------------------------------------------------------------------------------
        // Texture 2D
        //-----------------------------------------------------------------------------------

        RZTexture2D* RZTexture2D::Create(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:  new OpenGLTexture2D(name, width, height, data, format, wrapMode, filterMode); break;
                case Razix::Graphics::RenderAPI::VULKAN:
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
            return nullptr;
        }

        RZTexture2D* RZTexture2D::CreateFromFile(const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:
                case Razix::Graphics::RenderAPI::VULKAN:
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
            return nullptr;
        }

    }
}