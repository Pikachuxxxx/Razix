// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAPIDesc.h"

namespace Razix {
    namespace Graphics {
        std::string RZTextureDesc::FormatToString(const Graphics::TextureFormat format)
        {
            switch (format) {
                case Razix::Graphics::TextureFormat::R8:
                    return "R8";
                    break;
                case Razix::Graphics::TextureFormat::R32_INT:
                    return "R32_INT";
                    break;
                case Razix::Graphics::TextureFormat::R32_UINT:
                    return "R32_UINT";
                    break;
                case Razix::Graphics::TextureFormat::R32F:
                    return "R32F";
                    break;
                case Razix::Graphics::TextureFormat::RG8:
                    return "RG8";
                    break;
                case Razix::Graphics::TextureFormat::RGB8:
                    return "RGB8";
                    break;
                case Razix::Graphics::TextureFormat::RGBA8:
                    return "RGBA8";
                    break;
                case Razix::Graphics::TextureFormat::RGB16:
                    return "RGB16";
                    break;
                case Razix::Graphics::TextureFormat::RGBA16:
                    return "RGBA16";
                    break;
                case Razix::Graphics::TextureFormat::RGB32:
                    return "RGB32";
                    break;
                case Razix::Graphics::TextureFormat::RGBA32:
                    return "RGBA32";
                    break;
                case Razix::Graphics::TextureFormat::RGBA32F:
                    return "RGBA32F";
                    break;
                case Razix::Graphics::TextureFormat::RGB:
                    return "RGB";
                    break;
                case Razix::Graphics::TextureFormat::RGBA:
                    return "RGBA";
                    break;
                case Razix::Graphics::TextureFormat::DEPTH16_UNORM:
                    return "DEPTH";
                    break;
                case Razix::Graphics::TextureFormat::STENCIL:
                    return "STENCIL";
                    break;
                case Razix::Graphics::TextureFormat::DEPTH_STENCIL:
                    return "DEPTH_STENCIL";
                    break;
                case Razix::Graphics::TextureFormat::SCREEN:
                    return "Presentation";
                    break;
                case Razix::Graphics::TextureFormat::BGRA8_UNORM:
                    return "BGRA8_UNORM";
                    break;
                default:
                    return "BGRA8_UNORM";
                    break;
                    break;
            }
            return "BGRA8_UNORM";
        }

        std::string RZTextureDesc::TypeToString(TextureType type)
        {
            switch (type) {
                case Razix::Graphics::TextureType::Texture_2D:
                    return "2D";
                    break;
                case Razix::Graphics::TextureType::Texture_3D:
                    return "3D";
                    break;
                case Razix::Graphics::TextureType::Texture_CubeMap:
                    return "CubeMap";
                    break;
                case Razix::Graphics::TextureType::Texture_Depth:
                    return "Depth";
                    break;
                case Razix::Graphics::TextureType::Texture_SwapchainImage:
                    return "Swapchain Image";
                    break;
                case TextureType::Texture_1D:
                    return "1D";
                    break;
                case TextureType::Texture_2DArray:
                    return "2D Array";
                    break;
                case TextureType::Texture_CubeMapArray:
                    return "CubeMap Array";
                    break;
                default:
                    return "NONE";
                    break;
            }
            return "NONE";
        }

        //-----------------------------------------------------------------------------------

        static std::map<std::string, Razix::Graphics::CompareOp> CompareOpStringMap = {
            {"Never", Razix::Graphics::CompareOp::Never},
            {"Less", Razix::Graphics::CompareOp::Less},
            {"Equal", Razix::Graphics::CompareOp::Equal},
            {"LessOrEqual", Razix::Graphics::CompareOp::LessOrEqual},
            {"Greater", Razix::Graphics::CompareOp::Greater},
            {"NotEqual", Razix::Graphics::CompareOp::NotEqual},
            {"GreaterOrEqual", Razix::Graphics::CompareOp::GreaterOrEqual},
            {"Always", Razix::Graphics::CompareOp::Always}};

        // FIXME: compile time checks for map<=>enum size check
        //static_assert(sizeof(CompareOpStringMap) == (int) CompareOp::COUNT - 1, "CompareOpStringMap size is not correct, missing values from enum");

        Razix::Graphics::CompareOp StringToCompareOp(const std::string& str)
        {
            return CompareOpStringMap[str];
        }
    }    // namespace Graphics
}    // namespace Razix