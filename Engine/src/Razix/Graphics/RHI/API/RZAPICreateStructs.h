#pragma once

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Graphics/RHI/API/RZTextureData.h"

namespace Razix {
    namespace Graphics {

        struct RZTexture2DCreateInfo
        {
            std::string          name;
            u32                  width;
            u32                  height;
            void*                data;
            RZTexture::Format    format;
            RZTexture::Wrapping  wrapping;
            RZTexture::Filtering filtering;
        };

        struct RZTexture2DArrayCreateInfo
        {
            std::string          name;
            u32                  width;
            u32                  height;
            u32                  layers;
            RZTexture::Format    format;
            RZTexture::Wrapping  wrapping;
            RZTexture::Filtering filtering;
        };

        /* Defines how the buffer is used */
        enum class BufferUsage
        {
            STATIC,
            DYNAMIC,
            STREAM
        };

        struct RZVertexBufferCreateInfo
        {
            u32         size;
            void*       data;
            BufferUsage usage;
        };

        struct RZIndexBufferCreateInfo
        {
            u32         count;
            u32         data;
            BufferUsage usage;
        };

    }    // namespace Graphics
}    // namespace Razix