#include "rzxpch.h"
#include "Texture.h"

#include "Razix/Graphics/API/GraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/API/OpenGL/OpenGLTexture.h"
#endif

namespace Razix {
    namespace Graphics {

        Texture* Texture::Create2D(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode)
        {
            switch (Graphics::GraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:  new OpenGLTexture(); break;
                case Razix::Graphics::RenderAPI::VULKAN:
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }

        Texture* Texture::Create2DFromFile(const std::string& filePath, const std::string& name, Format format, Wrapping wrapMode, Filtering filterMode)
        {
            switch (Graphics::GraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:
                case Razix::Graphics::RenderAPI::VULKAN:
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }

        uint32_t Texture::calculateMipMapCount(uint32_t width, uint32_t height)
        {
            //tex: The texture mip levels are $numLevels = 1 + floor(log2(max(w, h, d)))$
            uint32_t levels = 1;
            while ((width | height) >> levels)
                levels++;

            return levels;
        }
    }
}