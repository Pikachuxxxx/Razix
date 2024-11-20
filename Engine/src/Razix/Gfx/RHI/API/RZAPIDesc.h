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
        enum class ShaderStage;
        enum class ShaderBuiltin : u32;

        // Graphics API

        // All API desc structs must have a name member variable

        // TODO: Add checks for data members based on type, ex. Texture_CubeMap must have layers > 1 etc.
        struct RZTextureDesc
        {
            std::string   name       = "$UNNAMED_TEXTURE_RESOURCE"; /* Name of the texture                                           */
            u32           width      = 0;                           /*  The Width of the texture                                     */
            u32           height     = 0;                           /* The Height of the texture                                     */
            u32           depth      = 1;                           /* The depth of the texture used only for 3D textures            */
            u32           layers     = 1;                           /* The array Layers of the texture used for Arrays and CubeMaps  */
            void*         data       = nullptr;                     /* The Data uses to initialize the Texture with                  */
            u32           size       = 0;                           /* Total size of pixel data used for initialization              */
            TextureType   type       = TextureType::Texture_2D;     /* The type of the Texture                                       */
            TextureFormat format     = TextureFormat::RGBA16F;      /* The format of the texture                                     */
            Wrapping      wrapping   = Wrapping::REPEAT;            /* Wrap mode of the texture in memory                            */
            Filtering     filtering  = Filtering{};                 /* Filtering mode of the texture                                 */
            bool          enableMips = false;                       /* Whether or not to generate mip maps or not for the texture    */
            bool          flipX      = false;                       /* Flip the texture on X-axis during load                        */
            bool          flipY      = true;                        /* Flip the texture on Y-axis during load                        */
            u32           dataSize   = sizeof(unsigned char);       /* data size of each pixel, HDR data vs normal pixel data        */
            std::string   filePath   = "";                          /* Filepath to load texture from, empty if not                   */

            // TODO: move them with below functions or to some util class

            static RAZIX_API std::string FormatToString(const Gfx::TextureFormat format);
            static RAZIX_API std::string TypeToString(TextureType type);

            static RAZIX_API TextureFormat StringToFormat(const std::string& str);
            static RAZIX_API TextureType   StringToType(const std::string& str);
        };

        /* Used for creating Vertex (VB_), Index(IB_) or Constant buffers(CB_) */
        struct RZBufferDesc
        {
            std::string name = "$UNNAMED_BUFFER"; /* Name of the buffer */
            union
            {
                u32 size = 0; /* The size of the vertex buffer         */
                u32 count;    /* The count of the index buffer         */
            };
            void*          data   = nullptr;             /* vertex data to fill the buffer with   */
            BufferUsage    usage  = BufferUsage::Static; /* Usage of the vertex buffer            */
            RZBufferLayout layout = {};                  /* Layout of buffer on how elements have been stored in the buffer */
        };

        struct RZShaderDesc
        {
            std::string   name = "$UNNAMED_SHADER"; /* Name of the shader */
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
            std::string                name                   = "$UNNAMED_PIPELINE_RESOURCE";  /* Name of the texture                                                         */
            RZShaderHandle             shader                 = {};                            /* Shader used by the Pipeline                                                 */
            std::vector<TextureFormat> colorAttachmentFormats = {};                            /* color attachments used by this pipeline, that we write to                   */
            TextureFormat              depthFormat            = TextureFormat::DEPTH32F;       /* depth attachment format for this pipeline                                   */
            CullMode                   cullMode               = CullMode::Front;               /* geometry cull mode                                                          */
            PolygonMode                polygonMode            = PolygonMode::Fill;             /* polygons fill mode                                                          */
            DrawType                   drawType               = DrawType::Triangle;            /* draw primitive used to draw the geometry                                    */
            bool                       transparencyEnabled    = true;                          /* whether or not to enable transparency while blending colors                 */
            bool                       depthBiasEnabled       = false;                         /* whether or not to enable depth bias when writing to depth texture           */
            bool                       depthTestEnabled       = true;                          /* whether or not to enable depth testing                                      */
            bool                       depthWriteEnabled      = true;                          /* whether or not to enable wiring depth target                                */
            BlendFactor                colorSrc               = BlendFactor::SrcAlpha;         /* color source blend factor                                                   */
            BlendFactor                colorDst               = BlendFactor::OneMinusSrcAlpha; /* color destination colour factor                                             */
            BlendOp                    colorOp                = BlendOp::Add;                  /* blend operation uses when 2 colors are to mixed in the pixel shader stage   */
            BlendFactor                alphaSrc               = BlendFactor::One;              /* blend factor for alpha of the source color                                  */
            BlendFactor                alphaDst               = BlendFactor::One;              /* blend factor for alpha of the destination color                             */
            BlendOp                    alphaOp                = BlendOp::Add;                  /* blend operation when mixing alpha of the 2 colors in pixel shader stage     */
            CompareOp                  depthOp                = CompareOp::Less;               /* depth operation comparison function                                         */
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
    }    // namespace Graphics
}    // namespace Razix