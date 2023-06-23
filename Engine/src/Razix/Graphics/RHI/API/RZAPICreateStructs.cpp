// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAPICreateStructs.h"

namespace Razix {
    namespace Graphics {
        std::string RZTextureDesc::FormatToString(const Graphics::RZTextureProperties::Format format)
        {
            switch (format) {
                case Razix::Graphics::RZTextureProperties::Format::R8:
                    return "R8";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::R32_INT:
                    return "R32_INT";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::R32_UINT:
                    return "R32_UINT";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::R32F:
                    return "R32F";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RG8:
                    return "RG8";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGB8:
                    return "RGB8";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGBA8:
                    return "RGBA8";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGB16:
                    return "RGB16";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGBA16:
                    return "RGBA16";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGB32:
                    return "RGB32";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGBA32:
                    return "RGBA32";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGBA32F:
                    return "RGBA32F";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGB:
                    return "RGB";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::RGBA:
                    return "RGBA";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::DEPTH16_UNORM:
                    return "DEPTH";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::STENCIL:
                    return "STENCIL";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::DEPTH_STENCIL:
                    return "DEPTH_STENCIL";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::SCREEN:
                    return "Presentation";
                    break;
                case Razix::Graphics::RZTextureProperties::Format::BGRA8_UNORM:
                    return "BGRA8_UNORM";
                    break;
                default:
                    return "BGRA8_UNORM";
                    break;
                    break;
            }
            return "BGRA8_UNORM";
        }

        std::string RZTextureDesc::TypeToString(RZTextureProperties::Type type)
        {
            switch (type) {
                case Razix::Graphics::RZTextureProperties::Type::Texture_2D:
                    return "2D";
                    break;
                case Razix::Graphics::RZTextureProperties::Type::Texture_3D:
                    return "3D";
                    break;
                case Razix::Graphics::RZTextureProperties::Type::Texture_CubeMap:
                    return "CubeMap";
                    break;
                case Razix::Graphics::RZTextureProperties::Type::Texture_DepthTarget:
                    return "Depth";
                    break;
                case Razix::Graphics::RZTextureProperties::Type::Texture_RenderTarget:
                    return "RT";
                    break;
                case Razix::Graphics::RZTextureProperties::Type::Texture_SwapchainImage:
                    return "Swapchain Image";
                    break;
                default:
                    return "NONE";
                    break;
            }
            return "NONE";
        }
    }    // namespace Graphics
}    // namespace Razix