// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAPIDesc.h"

#include "Razix/Gfx/RHI/API/RZShader.h"

namespace Razix {
    namespace Gfx {

        // TODO: Make VS tool for Enum->String and String->Enum code generator

        std::string RZTextureDesc::FormatToString(const Gfx::TextureFormat format)
        {
            switch (format) {
                case Razix::Gfx::TextureFormat::R8:
                    return "R8";
                    break;
                case Razix::Gfx::TextureFormat::R32_INT:
                    return "R32_INT";
                    break;
                case Razix::Gfx::TextureFormat::R32_UINT:
                    return "R32_UINT";
                    break;
                case Razix::Gfx::TextureFormat::R32F:
                    return "R32F";
                    break;
                case Razix::Gfx::TextureFormat::RG8:
                    return "RG8";
                    break;
                case Razix::Gfx::TextureFormat::RG16F:
                    return "RG16F";
                    break;
                case Razix::Gfx::TextureFormat::RGB8:
                    return "RGB8";
                    break;
                case Razix::Gfx::TextureFormat::RGBA8:
                    return "RGBA8";
                    break;
                case Razix::Gfx::TextureFormat::RGB16:
                    return "RGB16";
                    break;
                case Razix::Gfx::TextureFormat::RGBA16:
                    return "RGBA16";
                    break;
                case Razix::Gfx::TextureFormat::RGBA16F:
                    return "RGBA16F";
                    break;
                case Razix::Gfx::TextureFormat::RGB32:
                    return "RGB32";
                    break;
                case Razix::Gfx::TextureFormat::RGBA32:
                    return "RGBA32";
                    break;
                case Razix::Gfx::TextureFormat::RGBA32F:
                    return "RGBA32F";
                    break;
                case Razix::Gfx::TextureFormat::RGB:
                    return "RGB";
                    break;
                case Razix::Gfx::TextureFormat::RGBA:
                    return "RGBA";
                    break;
                case Razix::Gfx::TextureFormat::DEPTH16_UNORM:
                    return "DEPTH";
                    break;
                case Razix::Gfx::TextureFormat::STENCIL:
                    return "STENCIL";
                    break;
                case Razix::Gfx::TextureFormat::DEPTH_STENCIL:
                    return "DEPTH_STENCIL";
                    break;
                case Razix::Gfx::TextureFormat::SCREEN:
                    return "Presentation";
                    break;
                case Razix::Gfx::TextureFormat::BGRA8_UNORM:
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
                case Razix::Gfx::TextureType::Texture_2D:
                    return "2D";
                    break;
                case Razix::Gfx::TextureType::Texture_3D:
                    return "3D";
                    break;
                case Razix::Gfx::TextureType::Texture_CubeMap:
                    return "CubeMap";
                    break;
                case Razix::Gfx::TextureType::Texture_Depth:
                    return "Depth";
                    break;
                case Razix::Gfx::TextureType::Texture_SwapchainImage:
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

        static std::unordered_map<std::string, Gfx::TextureFormat> StringTextureFormatMap = {
            {"RGBA32F", Gfx::TextureFormat::RGBA32F},
            {"R8", Gfx::TextureFormat::R8},
            {"R32_INT", Gfx::TextureFormat::R32_INT},
            {"R32_UINT", Gfx::TextureFormat::R32_UINT},
            {"R32F", Gfx::TextureFormat::R32F},
            {"RG8", Gfx::TextureFormat::RG8},
            {"RG16F", Gfx::TextureFormat::RG16F},
            {"RGB8", Gfx::TextureFormat::RGB8},
            {"RGBA8", Gfx::TextureFormat::RGBA8},
            {"RGB16", Gfx::TextureFormat::RGB16},
            {"RGBA16", Gfx::TextureFormat::RGBA16},
            {"RGBA16F", Gfx::TextureFormat::RGBA16F},
            {"RGB32", Gfx::TextureFormat::RGB32},
            {"RGBA32", Gfx::TextureFormat::RGBA32},
            {"RGB", Gfx::TextureFormat::RGB},
            {"RGBA", Gfx::TextureFormat::RGBA},
            {"DEPTH16_UNORM", Gfx::TextureFormat::DEPTH16_UNORM},
            {"DEPTH32F", Gfx::TextureFormat::DEPTH32F},
            {"STENCIL", Gfx::TextureFormat::STENCIL},
            {"DEPTH_STENCIL", Gfx::TextureFormat::DEPTH_STENCIL},
            {"SCREEN", Gfx::TextureFormat::SCREEN},
            {"BGRA8_UNORM", Gfx::TextureFormat::BGRA8_UNORM},
            {"R11G11B10A2_UINT", Gfx::TextureFormat::R11G11B10A2_UINT},
            {"R11G11B10A2_SFLOAT", Gfx::TextureFormat::R11G11B10A2_SFLOAT},
            {"NONE", Gfx::TextureFormat::NONE}};

        Razix::Gfx::TextureFormat RZTextureDesc::StringToFormat(const std::string& str)
        {
            RAZIX_ASSERT((StringTextureFormatMap.size() == (u32) TextureFormat::COUNT), "TextureFormat string enum map has missing values");
            return StringTextureFormatMap[str];
        }

        static std::unordered_map<std::string, Gfx::TextureType> StringTextureTypeMap = {

            {"Texture_1D", Gfx::TextureType::Texture_1D},
            {"Texture_2D", Gfx::TextureType::Texture_2D},
            {"Texture_2DArray", Gfx::TextureType::Texture_2DArray},
            {"Texture_3D", Gfx::TextureType::Texture_3D},
            {"Texture_Depth", Gfx::TextureType::Texture_Depth},
            {"Texture_CubeMap", Gfx::TextureType::Texture_CubeMap},
            {"Texture_CubeMapArray", Gfx::TextureType::Texture_CubeMapArray},
            {"Texture_SwapchainImage", Gfx::TextureType::Texture_SwapchainImage}};

        Razix::Gfx::TextureType RZTextureDesc::StringToType(const std::string& str)
        {
            return StringTextureTypeMap[str];
        }

        static std::unordered_map<std::string, Razix::Gfx::CompareOp>
            CompareOpStringMap = {
                {"Never", Razix::Gfx::CompareOp::Never},
                {"Less", Razix::Gfx::CompareOp::Less},
                {"Equal", Razix::Gfx::CompareOp::Equal},
                {"LessOrEqual", Razix::Gfx::CompareOp::LessOrEqual},
                {"Greater", Razix::Gfx::CompareOp::Greater},
                {"NotEqual", Razix::Gfx::CompareOp::NotEqual},
                {"GreaterOrEqual", Razix::Gfx::CompareOp::GreaterOrEqual},
                {"Always", Razix::Gfx::CompareOp::Always}};

        // FIXME: compile time checks for map<=>enum size check
        //static_assert(sizeof(CompareOpStringMap) == (int) CompareOp::COUNT - 1, "CompareOpStringMap size is not correct, missing values from enum");

        Razix::Gfx::CompareOp StringToCompareOp(const std::string& str)
        {
            return CompareOpStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::BlendOp> BlendOpStringMap = {
            {"Add", Razix::Gfx::BlendOp::Add},
            {"Subtract", Razix::Gfx::BlendOp::Subtract},
            {"ReverseSubtract", Razix::Gfx::BlendOp::ReverseSubtract},
            {"Min", Razix::Gfx::BlendOp::Min},
            {"Max", Razix::Gfx::BlendOp::Max}};

        Razix::Gfx::BlendOp StringToBlendOp(const std::string& str)
        {
            return BlendOpStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::DrawType> DrawTypeStringMap = {
            {"Triangle", Razix::Gfx::DrawType::Triangle},
            {"Line", Razix::Gfx::DrawType::Line},
            {"Point", Razix::Gfx::DrawType::Point}};

        Razix::Gfx::DrawType StringToDrawType(const std::string& str)
        {
            return DrawTypeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::PolygonMode> PolygonModeStringMap = {
            {"Fill", Razix::Gfx::PolygonMode::Fill},
            {"Line", Razix::Gfx::PolygonMode::Line},
            {"Point", Razix::Gfx::PolygonMode::Point}};

        Razix::Gfx::PolygonMode StringToPolygonMode(const std::string& str)
        {
            return PolygonModeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::CullMode> CullModeStringMap = {
            {"Back", Razix::Gfx::CullMode::Back},
            {"Front", Razix::Gfx::CullMode::Front},
            {"FrontBack", Razix::Gfx::CullMode::FrontBack},
            {"None", Razix::Gfx::CullMode::None},
        };

        Razix::Gfx::CullMode StringToCullMode(const std::string& str)
        {
            return CullModeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::TextureFormat> TextureFormatStringMap = {
            {"R8", Razix::Gfx::TextureFormat::R8},
            {"R32_INT", Razix::Gfx::TextureFormat::R32_INT},
            {"R32_UINT", Razix::Gfx::TextureFormat::R32_UINT},
            {"R32F", Razix::Gfx::TextureFormat::R32F},
            {"RG8", Razix::Gfx::TextureFormat::RG8},
            {"RGB8", Razix::Gfx::TextureFormat::RGB8},
            {"RGBA8", Razix::Gfx::TextureFormat::RGBA8},
            {"RGB16", Razix::Gfx::TextureFormat::RGB16},
            {"RGBA16", Razix::Gfx::TextureFormat::RGBA16},
            {"RGB32", Razix::Gfx::TextureFormat::RGB32},
            {"RGBA32", Razix::Gfx::TextureFormat::RGBA32},
            {"RGBA32F", Razix::Gfx::TextureFormat::RGBA32F},
            {"RGB", Razix::Gfx::TextureFormat::RGB},
            {"RGBA", Razix::Gfx::TextureFormat::RGBA},
            {"DEPTH16_UNORM", Razix::Gfx::TextureFormat::DEPTH16_UNORM},
            {"DEPTH32F", Razix::Gfx::TextureFormat::DEPTH32F},
            {"STENCIL", Razix::Gfx::TextureFormat::STENCIL},
            {"DEPTH_STENCIL", Razix::Gfx::TextureFormat::DEPTH_STENCIL},
            {"SCREEN", Razix::Gfx::TextureFormat::SCREEN},
            {"BGRA8_UNORM", Razix::Gfx::TextureFormat::BGRA8_UNORM},
            {"R11G11B10A2_UINT", Razix::Gfx::TextureFormat::R11G11B10A2_UINT},
            {"R11G11B10A2_SFLOAT", Razix::Gfx::TextureFormat::R11G11B10A2_SFLOAT}};

        TextureFormat StringToTextureFormat(const std::string& str)
        {
            return TextureFormatStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::Wrapping> TextureWrapModeStringMap = {
            {"REPEAT", Razix::Gfx::Wrapping::REPEAT},
            {"MIRRORED_REPEAT", Razix::Gfx::Wrapping::MIRRORED_REPEAT},
            {"CLAMP_TO_EDGE", Razix::Gfx::Wrapping::CLAMP_TO_EDGE},
            {"CLAMP_TO_BORDER", Razix::Gfx::Wrapping::CLAMP_TO_BORDER}};

        Razix::Gfx::Wrapping StringToWrapping(const std::string& str)
        {
            return TextureWrapModeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::Filtering::Mode> TextureFiletingModeStringMap = {
            {"LINEAR", Razix::Gfx::Filtering::Mode::LINEAR},
            {"NEAREST", Razix::Gfx::Filtering::Mode::NEAREST}};

        Razix::Gfx::Filtering::Mode StringToFilteringMode(const std::string& str)
        {
            return TextureFiletingModeStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::BlendFactor>
            BlendFactorStringMap = {
                {"Zero", Razix::Gfx::BlendFactor::Zero},
                {"One", Razix::Gfx::BlendFactor::One},
                {"SrcColor", Razix::Gfx::BlendFactor::SrcColor},
                {"OneMinusSrcColor", Razix::Gfx::BlendFactor::OneMinusSrcColor},
                {"DstColor", Razix::Gfx::BlendFactor::DstColor},
                {"OneMinusDstColor", Razix::Gfx::BlendFactor::OneMinusDstColor},
                {"SrcAlpha", Razix::Gfx::BlendFactor::SrcAlpha},
                {"OneMinusSrcAlpha", Razix::Gfx::BlendFactor::OneMinusSrcAlpha},
                {"DstAlpha", Razix::Gfx::BlendFactor::DstAlpha},
                {"OneMinusDstAlpha", Razix::Gfx::BlendFactor::OneMinusDstAlpha},
                {"ConstantColor", Razix::Gfx::BlendFactor::ConstantColor},
                {"OneMinusConstantColor", Razix::Gfx::BlendFactor::OneMinusConstantColor},
                {"ConstantAlpha", Razix::Gfx::BlendFactor::ConstantAlpha},
                {"OneMinusConstantAlpha", Razix::Gfx::BlendFactor::OneMinusConstantAlpha},
                {"SrcAlphaSaturate", Razix::Gfx::BlendFactor::SrcAlphaSaturate}};

        Razix::Gfx::BlendFactor StringToBlendFactor(const std::string& str)
        {
            return BlendFactorStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::BufferUsage> BufferUsageStringMap = {
            {"Static", Razix::Gfx::BufferUsage::Static},
            {"PersistentStream", Razix::Gfx::BufferUsage::PersistentStream},
            {"Staging", Razix::Gfx::BufferUsage::Staging},
            {"ReadBack", Razix::Gfx::BufferUsage::ReadBack},
            {"IndirectDrawArgs", Razix::Gfx::BufferUsage::IndirectDrawArgs}};

        Razix::Gfx::BufferUsage StringToBufferUsage(const std::string& str)
        {
            return BufferUsageStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        static std::unordered_map<std::string, Razix::Gfx::ShaderStage> ShaderStageStringMap = {
            {"NONE", Razix::Gfx::ShaderStage::kNone},
            {"VERTEX", Razix::Gfx::ShaderStage::kVertex},
            {"PIXEL", Razix::Gfx::ShaderStage::kPixel},
            {"COMPUTE", Razix::Gfx::ShaderStage::kCompute},
            {"GEOMETRY", Razix::Gfx::ShaderStage::kGeometry},
            {"TCS", Razix::Gfx::ShaderStage::kTesselationControl},
            {"TES", Razix::Gfx::ShaderStage::kTesselationEvaluation}};

        Razix::Gfx::ShaderStage StringToShaderStage(const std::string& str)
        {
            return ShaderStageStringMap[str];
        }
    }    // namespace Gfx
}    // namespace Razix