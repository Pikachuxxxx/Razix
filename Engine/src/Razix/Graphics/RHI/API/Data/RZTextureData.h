#pragma once

namespace Razix {
    namespace Graphics {

        /* The type of the texture */
        enum class TextureType
        {
            Texture_1D = 0,
            Texture_2D,
            Texture_2DArray,
            Texture_3D,
            Texture_Depth,
            Texture_CubeMap,
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
            RGB8,
            RGBA8,
            RGB16,
            RGBA16,
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
            "Texture_Depth",
            "Texture_CubeMap",
            "Texture_CubeMapArray",
            "Texture_SwapchainImage"};

        RAZIX_ENUM_NAMES_ASSERT(TextureTypeNames, TextureType);

        static const char* TextureFormatNames[] = {
            "R8",
            "R32_INT",
            "R32_UINT",
            "R32F",
            "RG8",
            "RGB8",
            "RGBA8",
            "RGB16",
            "RGBA16",
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

    }    // namespace Graphics
}    // namespace Razix