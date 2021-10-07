#pragma once

#include "Razix/Core/Log.h"

namespace Razix {
    namespace Graphics {

        /* Load options on how the texture is loaded */
        struct TextureLoadOptions
        {
            bool flipX;
            bool flipY;
            bool generateMipMaps;

            TextureLoadOptions() {
                flipX = false;
                flipY = false;
                generateMipMaps = true;
            }

            TextureLoadOptions(bool flipX, bool flipY, bool genMips = true)
                : flipX(flipX)
                , flipY(flipY)
                , generateMipMaps(genMips) {}
        };

        /* Texture Formats supported by the Engine */
        enum class TextureFormat
        {
            NONE,
            R8,
            RG8,
            RGB8,
            RGBA8,
            RGB16,
            RGBA16,
            RGB32,
            RGBA32,
            RGB,
            RGBA,
            DEPTH,
            STENCIL,
            DEPTH_STENCIL,
            SCREEN
        };

        /* Texture WrapMode */
        enum class TextureWrap
        {
            NONE,
            REPEAT,
            CLAMP,
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER
        };

        /* Texture Filter Mode */
        enum class TextureFilter
        {
            NONE,
            LINEAR,
            NEAREST
        };

        /* Encapsulation of all the texture parameters */
        struct TextureParameters
        {
            TextureFormat   format;
            TextureFilter   minFilter;
            TextureFilter   magFilter;
            TextureWrap     wrap;
            bool            srgb = false;

            TextureParameters() {
                format      = TextureFormat::RGBA8;
                minFilter   = TextureFilter::NEAREST;
                magFilter   = TextureFilter::NEAREST;
                wrap        = TextureWrap::REPEAT;
            }

            TextureParameters(TextureFormat format, TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrap)
                : format(format)
                , minFilter(minFilter)
                , magFilter(magFilter)
                , wrap(wrap) {}

            TextureParameters(TextureFilter minFilter, TextureFilter magFilter)
                : format(TextureFormat::RGBA8)
                , minFilter(minFilter)
                , magFilter(magFilter)
                , wrap(TextureWrap::CLAMP) {}

            TextureParameters(TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrap)
                : format(TextureFormat::RGBA8)
                , minFilter(minFilter)
                , magFilter(magFilter)
                , wrap(wrap) {}

            TextureParameters(TextureWrap wrap)
                : format(TextureFormat::RGBA8)
                , minFilter(TextureFilter::LINEAR)
                , magFilter(TextureFilter::LINEAR)
                , wrap(wrap) {}

            TextureParameters(TextureFormat format)
                : format(format)
                , minFilter(TextureFilter::LINEAR)
                , magFilter(TextureFilter::LINEAR)
                , wrap(TextureWrap::CLAMP) {}
        };

        /* Razix Engine Texture class */
        class RAZIX_API Texture
        {
        public:
            virtual ~Texture() {}

            virtual void Bind(uint32_t slot = 0) const = 0;
            virtual void Unbind(uint32_t = 0) const = 0;

            virtual void SetData(const void* pixels) = 0;

        };

    }
}