#pragma once

namespace Razix {
    namespace Gfx {
    
        enum class PipelineType
        {
            kGraphics,
            kCompute
        };

        /* Culling mode describes which face of the polygon will be culled */
        enum class CullMode
        {
            Back,    // default
            Front,
            FrontBack,
            None,
            COUNT
        };

        /* Polygon mode describes how the geometry will be drawn, not the primitive used to draw */
        enum class PolygonMode
        {
            Fill,  /* The geometry will be filled with pixels                                                  */
            Line,  /* Only the outline of the geometry primitives will be drawn based on the line width set    */
            Point, /* Only the vertices will be drawn with square shaped points based on point size            */
            COUNT
        };

        /* Draw type describes what primitive will be used to draw the vertex data */
        enum class DrawType
        {
            Point,
            Triangle,
            Line,
            COUNT
        };

        /* Render Targets color blending function */
        enum class BlendOp
        {
            Add,
            Subtract,
            ReverseSubtract,
            Min,
            Max,
            COUNT
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
            COUNT
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
            COUNT
        };

        //-----------------------------------------------------------------------------------
        // [Source] : https://twitter.com/SebAaltonen/status/1597135035811106816

        static const char* CullModeNames[] = {
            "Back",
            "Front",
            "FrontBack",
            "None"};

        RAZIX_ENUM_NAMES_ASSERT(CullModeNames, CullMode);

        static const char* PolygonModeNames[] = {
            "Fill",
            "Line",
            "Point"};

        RAZIX_ENUM_NAMES_ASSERT(PolygonModeNames, PolygonMode);

        static const char* DrawTypeNames[] = {
            "Point",
            "Triangle",
            "Line"};

        RAZIX_ENUM_NAMES_ASSERT(DrawTypeNames, DrawType);

        static const char* BlendOpNames[] = {
            "Add",
            "Subtract",
            "ReverseSubtract",
            "Min",
            "Max"};

        RAZIX_ENUM_NAMES_ASSERT(BlendOpNames, BlendOp);

        static const char* BlendFactorNames[] = {
            "Zero",
            "One",
            "SrcColor",
            "OneMinusSrcColor",
            "DstColor",
            "OneMinusDstColor",
            "SrcAlpha",
            "OneMinusSrcAlpha",
            "DstAlpha",
            "OneMinusDstAlpha",
            "ConstantColor",
            "OneMinusConstantColor",
            "ConstantAlpha",
            "OneMinusConstantAlpha",
            "SrcAlphaSaturate"};

        RAZIX_ENUM_NAMES_ASSERT(BlendFactorNames, BlendFactor);

        static const char* CompareOpNames[] = {
            "Never",
            "Less",
            "Equal",
            "LessOrEqual",
            "Greater",
            "NotEqual",
            "GreaterOrEqual",
            "Always"};

        RAZIX_ENUM_NAMES_ASSERT(CompareOpNames, CompareOp);

        //-----------------------------------------------------------------------------------

    }    // namespace Gfx
}    // namespace Razix
