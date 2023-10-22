// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAPIDesc.h"

#include "Razix/Graphics/RHI/API/RZShader.h"

namespace Razix {
    namespace Graphics {

        // TODO: Make VS tool for Enum->String and String->Enum code generator

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

        static std::unordered_map<std::string, Graphics::TextureFormat> StringTextureFormatMap = {
            {"RGBA32F", Graphics::TextureFormat::RGBA32F},
            {"R8", Graphics::TextureFormat::R8},
            {"R32_INT", Graphics::TextureFormat::R32_INT},
            {"R32_UINT", Graphics::TextureFormat::R32_UINT},
            {"R32F", Graphics::TextureFormat::R32F},
            {"RG8", Graphics::TextureFormat::RG8},
            {"RGB8", Graphics::TextureFormat::RGB8},
            {"RGBA8", Graphics::TextureFormat::RGBA8},
            {"RGB16", Graphics::TextureFormat::RGB16},
            {"RGBA16", Graphics::TextureFormat::RGBA16},
            {"RGB32", Graphics::TextureFormat::RGB32},
            {"RGBA32", Graphics::TextureFormat::RGBA32},
            {"RGB", Graphics::TextureFormat::RGB},
            {"RGBA", Graphics::TextureFormat::RGBA},
            {"DEPTH16_UNORM", Graphics::TextureFormat::DEPTH16_UNORM},
            {"DEPTH32F", Graphics::TextureFormat::DEPTH32F},
            {"STENCIL", Graphics::TextureFormat::STENCIL},
            {"DEPTH_STENCIL", Graphics::TextureFormat::DEPTH_STENCIL},
            {"SCREEN", Graphics::TextureFormat::SCREEN},
            {"BGRA8_UNORM", Graphics::TextureFormat::BGRA8_UNORM},
            {"R11G11B10A2_UINT", Graphics::TextureFormat::R11G11B10A2_UINT},
            {"R11G11B10A2_SFLOAT", Graphics::TextureFormat::R11G11B10A2_SFLOAT},
            {"NONE", Graphics::TextureFormat::NONE}};

        Razix::Graphics::TextureFormat RZTextureDesc::StringToFormat(const std::string& str)
        {
            RAZIX_ASSERT((StringTextureFormatMap.size() == (u32) TextureFormat::COUNT), "TextureFormat string enum map has missing values");
            return StringTextureFormatMap[str];
        }

        static std::unordered_map<std::string, Graphics::TextureType> StringTextureTypeMap = {

            {"Texture_1D", Graphics::TextureType::Texture_1D},
            {"Texture_2D", Graphics::TextureType::Texture_2D},
            {"Texture_2DArray", Graphics::TextureType::Texture_2DArray},
            {"Texture_3D", Graphics::TextureType::Texture_3D},
            {"Texture_Depth", Graphics::TextureType::Texture_Depth},
            {"Texture_CubeMap", Graphics::TextureType::Texture_CubeMap},
            {"Texture_CubeMapArray", Graphics::TextureType::Texture_CubeMapArray},
            {"Texture_SwapchainImage", Graphics::TextureType::Texture_SwapchainImage}};

        Razix::Graphics::TextureType RZTextureDesc::StringToType(const std::string& str)
        {
            return StringTextureTypeMap[str];
        }

        static std::unordered_map<std::string, Razix::Graphics::CompareOp>
            CompareOpStringMap = {
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

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::BlendOp> BlendOpStringMap = {
            {"Add", Razix::Graphics::BlendOp::Add},
            {"Subtract", Razix::Graphics::BlendOp::Subtract},
            {"ReverseSubtract", Razix::Graphics::BlendOp::ReverseSubtract},
            {"Min", Razix::Graphics::BlendOp::Min},
            {"Max", Razix::Graphics::BlendOp::Max}};

        Razix::Graphics::BlendOp StringToBlendOp(const std::string& str)
        {
            return BlendOpStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::DrawType> DrawTypeStringMap = {
            {"Triangle", Razix::Graphics::DrawType::Triangle},
            {"Line", Razix::Graphics::DrawType::Line},
            {"Point", Razix::Graphics::DrawType::Point}};

        Razix::Graphics::DrawType StringToDrawType(const std::string& str)
        {
            return DrawTypeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::PolygonMode> PolygonModeStringMap = {
            {"Fill", Razix::Graphics::PolygonMode::Fill},
            {"Line", Razix::Graphics::PolygonMode::Line},
            {"Point", Razix::Graphics::PolygonMode::Point}};

        Razix::Graphics::PolygonMode StringToPolygonMode(const std::string& str)
        {
            return PolygonModeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::CullMode> CullModeStringMap = {
            {"Back", Razix::Graphics::CullMode::Back},
            {"Front", Razix::Graphics::CullMode::Front},
            {"FrontBack", Razix::Graphics::CullMode::FrontBack},
            {"None", Razix::Graphics::CullMode::None},
        };

        Razix::Graphics::CullMode StringToCullMode(const std::string& str)
        {
            return CullModeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::TextureFormat> TextureFormatStringMap = {
            {"R8", Razix::Graphics::TextureFormat::R8},
            {"R32_INT", Razix::Graphics::TextureFormat::R32_INT},
            {"R32_UINT", Razix::Graphics::TextureFormat::R32_UINT},
            {"R32F", Razix::Graphics::TextureFormat::R32F},
            {"RG8", Razix::Graphics::TextureFormat::RG8},
            {"RGB8", Razix::Graphics::TextureFormat::RGB8},
            {"RGBA8", Razix::Graphics::TextureFormat::RGBA8},
            {"RGB16", Razix::Graphics::TextureFormat::RGB16},
            {"RGBA16", Razix::Graphics::TextureFormat::RGBA16},
            {"RGB32", Razix::Graphics::TextureFormat::RGB32},
            {"RGBA32", Razix::Graphics::TextureFormat::RGBA32},
            {"RGBA32F", Razix::Graphics::TextureFormat::RGBA32F},
            {"RGB", Razix::Graphics::TextureFormat::RGB},
            {"RGBA", Razix::Graphics::TextureFormat::RGBA},
            {"DEPTH16_UNORM", Razix::Graphics::TextureFormat::DEPTH16_UNORM},
            {"DEPTH32F", Razix::Graphics::TextureFormat::DEPTH32F},
            {"STENCIL", Razix::Graphics::TextureFormat::STENCIL},
            {"DEPTH_STENCIL", Razix::Graphics::TextureFormat::DEPTH_STENCIL},
            {"SCREEN", Razix::Graphics::TextureFormat::SCREEN},
            {"BGRA8_UNORM", Razix::Graphics::TextureFormat::BGRA8_UNORM},
            {"R11G11B10A2_UINT", Razix::Graphics::TextureFormat::R11G11B10A2_UINT},
            {"R11G11B10A2_SFLOAT", Razix::Graphics::TextureFormat::R11G11B10A2_SFLOAT}};

        TextureFormat StringToTextureFormat(const std::string& str)
        {
            return TextureFormatStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::Wrapping> TextureWrapModeStringMap = {
            {"REPEAT", Razix::Graphics::Wrapping::REPEAT},
            {"MIRRORED_REPEAT", Razix::Graphics::Wrapping::MIRRORED_REPEAT},
            {"CLAMP_TO_EDGE", Razix::Graphics::Wrapping::CLAMP_TO_EDGE},
            {"CLAMP_TO_BORDER", Razix::Graphics::Wrapping::CLAMP_TO_BORDER}};

        Razix::Graphics::Wrapping StringToWrapping(const std::string& str)
        {
            return TextureWrapModeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::Filtering::Mode> TextureFiletingModeStringMap = {
            {"LINEAR", Razix::Graphics::Filtering::Mode::LINEAR},
            {"NEAREST", Razix::Graphics::Filtering::Mode::NEAREST}};

        Razix::Graphics::Filtering::Mode StringToFilteringMode(const std::string& str)
        {
            return TextureFiletingModeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::BlendFactor>
            BlendFactorStringMap = {
                {"Zero", Razix::Graphics::BlendFactor::Zero},
                {"One", Razix::Graphics::BlendFactor::One},
                {"SrcColor", Razix::Graphics::BlendFactor::SrcColor},
                {"OneMinusSrcColor", Razix::Graphics::BlendFactor::OneMinusSrcColor},
                {"DstColor", Razix::Graphics::BlendFactor::DstColor},
                {"OneMinusDstColor", Razix::Graphics::BlendFactor::OneMinusDstColor},
                {"SrcAlpha", Razix::Graphics::BlendFactor::SrcAlpha},
                {"OneMinusSrcAlpha", Razix::Graphics::BlendFactor::OneMinusSrcAlpha},
                {"DstAlpha", Razix::Graphics::BlendFactor::DstAlpha},
                {"OneMinusDstAlpha", Razix::Graphics::BlendFactor::OneMinusDstAlpha},
                {"ConstantColor", Razix::Graphics::BlendFactor::ConstantColor},
                {"OneMinusConstantColor", Razix::Graphics::BlendFactor::OneMinusConstantColor},
                {"ConstantAlpha", Razix::Graphics::BlendFactor::ConstantAlpha},
                {"OneMinusConstantAlpha", Razix::Graphics::BlendFactor::OneMinusConstantAlpha},
                {"SrcAlphaSaturate", Razix::Graphics::BlendFactor::SrcAlphaSaturate}};

        Razix::Graphics::BlendFactor StringToBlendFactor(const std::string& str)
        {
            return BlendFactorStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::BufferUsage> BufferUsageStringMap = {
            {"Static", Razix::Graphics::BufferUsage::Static},
            {"Dynamic", Razix::Graphics::BufferUsage::Dynamic},
            {"Stream", Razix::Graphics::BufferUsage::Stream}};

        Razix::Graphics::BufferUsage StringToBufferUsage(const std::string& str)
        {
            return BufferUsageStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Graphics::ShaderStage> ShaderStageStringMap = {
            {"NONE", Razix::Graphics::ShaderStage::NONE},
            {"VERTEX", Razix::Graphics::ShaderStage::Vertex},
            {"PIXEL", Razix::Graphics::ShaderStage::Pixel},
            {"COMPUTE", Razix::Graphics::ShaderStage::Compute},
            {"GEOMETRY", Razix::Graphics::ShaderStage::Geometry},
            {"TCS", Razix::Graphics::ShaderStage::TCS},
            {"TES", Razix::Graphics::ShaderStage::TES}};

        Razix::Graphics::ShaderStage StringToShaderStage(const std::string& str)
        {
            return ShaderStageStringMap[str];
        }
    }    // namespace Graphics
}    // namespace Razix