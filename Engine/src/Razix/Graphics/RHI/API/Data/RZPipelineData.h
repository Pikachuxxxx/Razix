#pragma once

namespace Razix {
    namespace Graphics {

        /* Culling mode describes which face of the polygon will be culled */
        enum class CullMode
        {
            BACK,    // default
            FRONT,
            FRONTANDBACK,
            NONE
        };

        /* Polygon mode describes how the geometry will be drawn, not the primitive used to draw */
        enum class PolygonMode
        {
            FILL, /* The geometry will be filled with pixels                                                  */
            LINE, /* Only the outline of the geometry primitives will be drawn based on the line width set    */
            POINT /* Only the vertices will be drawn with square shaped points based on point size            */
        };

        /* Draw type describes what primitive will be used to draw the vertex data */
        enum class DrawType
        {
            POINT,
            TRIANGLE,
            LINES
        };

        /* Render Targets color blending function */
        enum class BlendOp
        {
            Add,
            Subtract,
            ReverseSubtract,
            Min,
            Max,
        };

        /* Blend Function factor */
        enum class BlendFactor
        {
            Zero,
            One,
            SrcColor,
            OneMinusSrcColor,
            DstColor,
            OneMinusDstColor,
            SrcAlpha,
            OneMinusSrcAlpha,
            DstAlpha,
            OneMinusDstAlpha,
            ConstantColor,
            OneMinusConstantColor,
            ConstantAlpha,
            OneMinusConstantAlpha,
            SrcAlphaSaturate,
        };

        /* Compare Operation Function for Depth and Stencil tests */
        enum class CompareOp
        {
            Never,
            Less,
            Equal,
            LessOrEqual,
            Greater,
            NotEqual,
            GreaterOrEqual,
            Always,
        };
    }    // namespace Graphics
}    // namespace Razix