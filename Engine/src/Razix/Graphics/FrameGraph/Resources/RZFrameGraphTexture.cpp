// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            static std::string FormatToString(const Graphics::RZTexture::Format format)
            {
                switch (format) {
                    case Razix::Graphics::RZTexture::Format::R8:
                        return "R8";
                        break;
                    case Razix::Graphics::RZTexture::Format::R32_INT:
                        return "R32_INT";
                        break;
                    case Razix::Graphics::RZTexture::Format::R32_UINT:
                        return "R32_UINT";
                        break;
                    case Razix::Graphics::RZTexture::Format::R32F:
                        return "R32F";
                        break;
                    case Razix::Graphics::RZTexture::Format::RG8:
                        return "RG8";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGB8:
                        return "RGB8";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGBA8:
                        return "RGBA8";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGB16:
                        return "RGB16";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGBA16:
                        return "RGBA16";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGB32:
                        return "RGB32";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGBA32:
                        return "RGBA32";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGBA32F:
                        return "RGBA32F";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGB:
                        return "RGB";
                        break;
                    case Razix::Graphics::RZTexture::Format::RGBA:
                        return "RGBA";
                        break;
                    case Razix::Graphics::RZTexture::Format::DEPTH:
                        return "DEPTH";
                        break;
                    case Razix::Graphics::RZTexture::Format::STENCIL:
                        return "STENCIL";
                        break;
                    case Razix::Graphics::RZTexture::Format::DEPTH_STENCIL:
                        return "DEPTH_STENCIL";
                        break;
                    case Razix::Graphics::RZTexture::Format::SCREEN:
                        return "Presentation";
                        break;
                    case Razix::Graphics::RZTexture::Format::BGRA8_UNORM:
                        return "BGRA8_UNORM";
                        break;
                    default:
                        break;
                }
            }

            void RZFrameGraphTexture::create(const Desc& desc, void* allocator)
            {
                switch (desc.type) {
                    case TextureType::Texture_2D:
                        m_Texture = Graphics::RZTexture2D::Create(desc.name, desc.name, desc.extent.x, desc.extent.y, nullptr, desc.format);
                        break;
                    case TextureType::Texture_3D:
                        break;
                    case TextureType::Texture_CubeMap:
                        break;
                    case TextureType::Texture_Depth:
                        m_Texture = Graphics::RZDepthTexture::Create(desc.extent.x, desc.extent.y);
                        break;
                    case TextureType::Texture_RenderTarget:
                        m_Texture = Graphics::RZRenderTexture::Create(desc.name, desc.extent.x, desc.extent.y, desc.format);
                        break;
                    case TextureType::Texture_SwapchainImage:
                        //m_Texture = RZRenderContext::getSwapchain()->GetCurrentImage()
                        break;
                }
            }

            void RZFrameGraphTexture::destroy(const Desc& desc, void* allocator)
            {
                m_Texture->Release(true);
            }

            std::string RZFrameGraphTexture::toString(const Desc& desc)
            {
                // Size, Format
                return "(" + std::to_string(desc.extent.x) + ", " + std::to_string(desc.extent.y) + ") - " + FormatToString(desc.format);
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix