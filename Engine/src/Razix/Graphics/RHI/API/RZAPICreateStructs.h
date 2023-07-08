#pragma once

#include "Razix/Graphics/RHI/API/Data/RZBufferData.h"
#include "Razix/Graphics/RHI/API/Data/RZPipelineData.h"
#include "Razix/Graphics/RHI/API/Data/RZTextureData.h"

namespace Razix {
    namespace Graphics {

        // Forward Declarations
        class RZShader;

        // Graphics API

        struct RZTextureDesc
        {
            std::string                    name       = "UN_NAMED_TEXTURE_RESOURCE";                    /* Name of the texture                                                                      */
            u32                            width      = 0;                                              /*  The Width of the texture                                                                */
            u32                            height     = 0;                                              /* The Height of the texture                                                                */
            u32                            layers     = 1;                                              /* The Height/Layers/Depth of the texture (depending on if it's a array or cubemap or 3D)   */
            void*                          data       = nullptr;                                        /* The Data uses to initialize the Texture with                                             */
            RZTextureProperties::Type      type       = RZTextureProperties::Type::Texture_2D;          /* The type of the Texture                                                                  */
            RZTextureProperties::Format    format     = RZTextureProperties::Format::RGBA32F;           /* The format of the texture                                                                */
            RZTextureProperties::Wrapping  wrapping   = RZTextureProperties::Wrapping::CLAMP_TO_BORDER; /* Wrap mode of the texture in memory                                                       */
            RZTextureProperties::Filtering filtering  = RZTextureProperties::Filtering{};               /* Filtering mode of the texture                                                            */
            bool                           enableMips = false;                                          /* Whether or not to generate mip maps or not for the texture                               */
            bool                           flipX      = false;                                          /* Flip the texture on X-axis during load                                                   */
            bool                           flipY      = false;                                          /* Flip the texture on Y-axis during load                                                   */

            /**
             * Returns the Format of the Texture in string
             * 
             * @param format The format of the Texture
             */
            static std::string FormatToString(const Graphics::RZTextureProperties::Format format);
            /**
             * Returns the Type of the Texture in string
             * 
             * @param type The type of the Texture
             */
            static std::string TypeToString(RZTextureProperties::Type type);
        };

        struct RZVertexBufferDesc
        {
            u32         size;  /* The size of the vertex buffer         */
            void*       data;  /* vertex data to fill the buffer with   */
            BufferUsage usage; /* Usage of the vertex buffer            */
        };

        struct RZIndexBufferDesc
        {
            u32         count; /* The size of the index buffer         */
            u32         data;  /* index data to fill the buffer with   */
            BufferUsage usage; /* Usage of the index buffer            */
        };

        /* Information necessary to create the pipeline */
        struct RZPipelineDesc
        {
            RZShader*                                shader                 = nullptr;                           /* Shader used by the Pipeline                                                 */
            std::vector<RZTextureProperties::Format> colorAttachmentFormats = {};                                /* color attachments used by this pipeline, that we write to                   */
            RZTextureProperties::Format              depthFormat            = RZTextureProperties::Format::NONE; /* depth attachment format for this pipeline                                   */
            CullMode                                 cullMode               = CullMode::BACK;                    /* geometry cull mode                                                          */
            PolygonMode                              polygonMode            = PolygonMode::FILL;                 /* polygons fill mode                                                          */
            DrawType                                 drawType               = DrawType::TRIANGLE;                /* draw primitive used to draw the geometry                                    */
            bool                                     transparencyEnabled    = true;                              /* whether or not to enable transparency while blending colors                 */
            bool                                     depthBiasEnabled       = false;                             /* whether or not to enable depth bias when writing to depth texture           */
            bool                                     depthTestEnabled       = true;                              /* whether or not to enable depth testing                                      */
            bool                                     depthWriteEnabled      = true;                              /* whether or not to enable wiring depth target                                */
            BlendFactor                              colorSrc               = BlendFactor::SrcAlpha;             /* color source blend factor                                                   */
            BlendFactor                              colorDst               = BlendFactor::OneMinusSrcAlpha;     /* color destination colour factor                                             */
            BlendOp                                  colorOp                = BlendOp::Add;                      /* blend operation uses when 2 colors are to mixed in the pixel shader stage   */
            BlendFactor                              alphaSrc               = BlendFactor::One;                  /* blend factor for alpha of the source color                                  */
            BlendFactor                              alphaDst               = BlendFactor::One;                  /* blend factor for alpha of the destination color                             */
            BlendOp                                  alphaOp                = BlendOp::Add;                      /* blend operation when mixing alpha of the 2 colors in pixel shader stage     */
            CompareOp                                depthOp                = CompareOp::Less;                   /* depth operation comparison function                                         */
        };
    }    // namespace Graphics
}    // namespace Razix