// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"

#include "Razix/Graphics/FrameGraph/Resources/RZTransientResources.h"

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
                        return "BGRA8_UNORM";
                        break;
                        break;
                }
                return "BGRA8_UNORM";
            }

            static std::string TypeToString(TextureType type)
            {
                switch (type) {
                    case Razix::Graphics::FrameGraph::TextureType::Texture_2D:
                        return "2D";
                        break;
                    case Razix::Graphics::FrameGraph::TextureType::Texture_3D:
                        return "3D";
                        break;
                    case Razix::Graphics::FrameGraph::TextureType::Texture_CubeMap:
                        return "CubeMap";
                        break;
                    case Razix::Graphics::FrameGraph::TextureType::Texture_Depth:
                        return "Depth";
                        break;
                    case Razix::Graphics::FrameGraph::TextureType::Texture_RenderTarget:
                        return "RT";
                        break;
                    case Razix::Graphics::FrameGraph::TextureType::Texture_SwapchainImage:
                        return "Swapchain Image";
                        break;
                    default:
                        return "NONE";
                        break;
                }
                return "NONE";
            }

            void RZFrameGraphTexture::create(const Desc& desc, void* allocator)
            {
                m_Texture = static_cast<FrameGraph::RZTransientResources*>(allocator)->acquireTexture(desc);
            }

            void RZFrameGraphTexture::destroy(const Desc& desc, void* allocator)
            {
                static_cast<FrameGraph::RZTransientResources*>(allocator)->releaseTexture(desc, m_Texture);
            }

            std::string RZFrameGraphTexture::toString(const Desc& desc)
            {
                // Size, Format
                return "(" + std::to_string(int(desc.extent.x)) + ", " + std::to_string(int(desc.extent.y)) + ") - " + FormatToString(desc.format) + " [" + TypeToString(desc.type) + "]";
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix