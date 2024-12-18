#pragma once

namespace Razix {
    namespace Gfx {

        /* The type of the texture */
        enum class TextureType
        {
            k1D = 0,
            k2D,
            k2DArray,
            k3D,
            kRW1D,
            kRW2D,
            kRW2DArray,
            kRW3D,
            kDepth,
            kCubeMap,
            kRWCubeMap,    // SRV is a CubeMap and UAV will be a RW2DArray view, special case handling of image views in backend
            kCubeMapArray,
            kSwapchainImage,
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
            kRepeat,
            kMirroredRepeat,
            kClampToEdge,
            kClampToBorder,
            COUNT
        };

        /* Filtering for the Texture */
        struct Filtering
        {
            enum class Mode
            {
                kFilterModeLinear,
                kFilterModeNearest,
                COUNT
            };
            Mode minFilter = Mode::kFilterModeNearest;
            Mode magFilter = Mode::kFilterModeNearest;

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
                "FilterModeLinear",
                "FilterModeNearest"};

        RAZIX_ENUM_NAMES_ASSERT(FitleringModeNames, Filtering::Mode);

        //-----------------------------------------------------------------------------------

    }    // namespace Gfx
}    // namespace Razix