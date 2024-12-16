#pragma once

namespace Razix {
    namespace Gfx {

        /* The type of the texture */
        enum class TextureType
        {
            Texture_1D = 0,
            Texture_2D,
            Texture_2DArray,
            Texture_3D,
            Texture_RW1D,
            Texture_RW2D,
            Texture_RW2DArray,
            Texture_RW3D,
            Texture_Depth,
            Texture_CubeMap,
            Texture_RWCubeMap,    // SRV is a CubeMap and UAV will be a RW@DArray view, special case
            Texture_CubeMapArray,
            Texture_SwapchainImage,
            COUNT
        };

        /* The format of the Texture resource */
        enum class TextureFormat
        {
            R8,
            R32_INT,
            R32_UINT,
            R32F,
            RG8,
            RG16F,
            RGB8,
            RGBA8,
            RGB16,
            RGBA16,
            RGBA16F,
            RGB32,
            RGBA32,
            RGBA32F,
            RGB,
            RGBA,
            DEPTH16_UNORM,
            DEPTH32F,
            STENCIL,
            DEPTH_STENCIL,
            SCREEN,
            BGRA8_UNORM,
            R11G11B10A2_UINT,
            R11G11B10A2_SFLOAT,
            NONE,
            COUNT
        };

        /* Wrap mode for the texture texels */
        enum class Wrapping
        {
            REPEAT,
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER,
            COUNT
        };

        /* Filtering for the Texture */
        struct Filtering
        {
            enum class Mode
            {
                LINEAR,
                NEAREST,
                COUNT
            };
            Mode minFilter = Mode::NEAREST;
            Mode magFilter = Mode::NEAREST;

            Filtering() {}
            Filtering(Mode min, Mode max)
                : minFilter(min), magFilter(max) {}
        };

        //-----------------------------------------------------------------------------------
        // [Source] : https://twitter.com/SebAaltonen/status/1597135035811106816

        static const char* TextureTypeNames[] = {
            "Texture_1D",
            "Texture_2D",
            "Texture_2DArray",
            "Texture_3D",
            "Texture_RW1D",
            "Texture_RW2D",
            "Texture_RW2DArray",
            "Texture_RW3D",
            "Texture_Depth",
            "Texture_CubeMap",
            "Texture_RWCubeMap",
            "Texture_CubeMapArray",
            "Texture_SwapchainImage"};

        RAZIX_ENUM_NAMES_ASSERT(TextureTypeNames, TextureType);

        static const char* TextureFormatNames[] = {
            "R8",
            "R32_INT",
            "R32_UINT",
            "R32F",
            "RG8",
            "RG16F",
            "RGB8",
            "RGBA8",
            "RGB16",
            "RGBA16",
            "RGBA16F",
            "RGB32",
            "RGBA32",
            "RGBA32F",
            "RGB",
            "RGBA",
            "DEPTH16_UNORM",
            "DEPTH32F",
            "STENCIL",
            "DEPTH_STENCIL",
            "SCREEN",
            "BGRA8_UNORM",
            "R11G11B10A2_UINT",
            "R11G11B10A2_SFLOAT",
            "NONE"};

        RAZIX_ENUM_NAMES_ASSERT(TextureFormatNames, TextureFormat);

        static const char* WrappingNames[] = {
            "REPEAT",
            "MIRRORED_REPEAT",
            "CLAMP_TO_EDGE",
            "CLAMP_TO_BORDER"};

        RAZIX_ENUM_NAMES_ASSERT(WrappingNames, Wrapping);

        static const char* FitleringModeNames[] =
            {
                "LINEAR",
                "NEAREST"};

        RAZIX_ENUM_NAMES_ASSERT(FitleringModeNames, Filtering::Mode);

        //-----------------------------------------------------------------------------------

    }    // namespace Gfx
}    // namespace Razix