#pragma once

#include "Razix/Gfx/RHI/API/RZBufferLayout.h"

#include "Razix/Gfx/RHI/API/Data/RZBufferData.h"
#include "Razix/Gfx/RHI/API/Data/RZPipelineData.h"
#include "Razix/Gfx/RHI/API/Data/RZTextureData.h"

#include "Razix/Gfx/RHI/API/RZAPIHandles.h"

namespace Razix {
    namespace Gfx {

        // Forward Declarations
        class RZShader;
        enum ShaderStage : u32;
        enum class ShaderBuiltin : u32;

        // Graphics API

        // All API desc structs must have a name member variable

        // TODO: Add checks for data members based on type, ex. Texture_CubeMap must have layers > 1 etc.
        struct RZTextureDesc
        {
            std::string   name                  = "$UNNAMED_TEXTURE_RESOURCE";
            u32           width                 = 0;
            u32           height                = 0;
            u32           depth                 = 1;
            u32           layers                = 1;
            void*         data                  = nullptr;
            u32           size                  = 0;
            TextureType   type                  = TextureType::k2D;
            TextureFormat format                = TextureFormat::RGBA16F;
            bool          enableMips            = false;
            bool          flipX                 = false;
            bool          flipY                 = true;
            u8            initResourceViewHints = 1;
            u32           dataSize              = sizeof(unsigned char);
            std::string   filePath              = "";

            static RAZIX_API std::string FormatToString(const Gfx::TextureFormat format);
            static RAZIX_API std::string TypeToString(TextureType type);

            static RAZIX_API TextureFormat StringToFormat(const std::string& str);
            static RAZIX_API TextureType   StringToType(const std::string& str);
        };

        struct RZSamplerDesc
        {
            std::string name             = "$UNNAMED_SAMPLER";
            Wrapping    wrapping         = Wrapping::kRepeat;
            Filtering   filtering        = Filtering{};
            f32         minLOD           = 0.0f;
            f32         maxLOD           = 1.0f;
            f32         maxAnisotropy    = 1.0f;
            bool        enableAnisotropy = true;

            RZSamplerDesc(const std::string& name             = "$UNNAMED_SAMPLER",
                Wrapping                     wrapping         = Wrapping::kRepeat,
                Filtering                    filtering        = Filtering{},
                f32                          minLOD           = 0.0f,
                f32                          maxLOD           = 1.0f,
                f32                          maxAnisotropy    = 1.0f,
                bool                         enableAnisotropy = false)
                : name(name),
                  wrapping(wrapping),
                  filtering(filtering),
                  minLOD(minLOD),
                  maxLOD(maxLOD),
                  maxAnisotropy(maxAnisotropy),
                  enableAnisotropy(enableAnisotropy)
            {
            }
        };

        /* Used for creating Vertex (VB_), Index(IB_) or Constant buffers(CB_) */
        struct RZBufferDesc
        {
            std::string name = "$UNNAMED_BUFFER";
            union
            {
                u32 size = 0;
                u32 count;
            };
            void*          data                  = nullptr;
            BufferUsage    usage                 = BufferUsage::Static;
            RZBufferLayout layout                = {};
            u8             initResourceViewHints = 0;
        };

        struct RZShaderDesc
        {
            std::string   name = "$UNNAMED_SHADER";
            std::string   filePath;
            ShaderBuiltin libraryID;
        };

        // TODO: Add presets to select blending like Additive, Subtractive etc as in PhotoShop + util methods
        enum class BlendPresets
        {
            Additive,
            AlphaBlend,
            Subtractive,
            Multiply,
            Darken
        };

        /* Information necessary to create the pipeline */
        struct RZPipelineDesc
        {
            std::string                name                   = "$UNNAMED_PIPELINE_RESOURCE";
            PipelineType               pipelineType           = PipelineType::kGraphics;
            RZShaderHandle             shader                 = {};
            std::vector<TextureFormat> colorAttachmentFormats = {};
            TextureFormat              depthFormat            = TextureFormat::DEPTH32F;
            CullMode                   cullMode               = CullMode::Front;
            PolygonMode                polygonMode            = PolygonMode::Fill;
            DrawType                   drawType               = DrawType::Triangle;
            bool                       transparencyEnabled    = true;
            bool                       depthBiasEnabled       = false;
            bool                       depthTestEnabled       = true;
            bool                       depthWriteEnabled      = true;
            BlendFactor                colorSrc               = BlendFactor::SrcAlpha;
            BlendFactor                colorDst               = BlendFactor::OneMinusSrcAlpha;
            BlendOp                    colorOp                = BlendOp::Add;
            BlendFactor                alphaSrc               = BlendFactor::One;
            BlendFactor                alphaDst               = BlendFactor::One;
            BlendOp                    alphaOp                = BlendOp::Add;
            CompareOp                  depthOp                = CompareOp::Less;
        };

        //-----------------------------------------------------------------------------------
        /* utility functions for frame graph parsing to convert string to enums */
        CompareOp       StringToCompareOp(const std::string& str);
        DrawType        StringToDrawType(const std::string& str);
        PolygonMode     StringToPolygonMode(const std::string& str);
        CullMode        StringToCullMode(const std::string& str);
        BlendOp         StringToBlendOp(const std::string& str);
        TextureFormat   StringToTextureFormat(const std::string& str);
        TextureType     StringToTextureType(const std::string& str);
        Wrapping        StringToWrapping(const std::string& str);
        Filtering::Mode StringToFilteringMode(const std::string& str);
        BlendFactor     StringToBlendFactor(const std::string& str);
        BufferUsage     StringToBufferUsage(const std::string& str);
        ShaderStage     StringToShaderStage(const std::string& str);
        //-----------------------------------------------------------------------------------
        // [Source] : https://twitter.com/SebAaltonen/status/1597135035811106816

        //-----------------------------------------------------------------------------------
    }    // namespace Gfx
}    // namespace Razix
