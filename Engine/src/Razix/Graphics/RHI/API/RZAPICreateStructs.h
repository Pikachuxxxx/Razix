#pragma once

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Graphics/RHI/API/RZBufferData.h"
#include "Razix/Graphics/RHI/API/RZTextureData.h"

namespace Razix {
    namespace Graphics {

        class RZTexture2DCreateInfo
        {
            std::string                    name;
            u32                            width;
            u32                            height;
            void*                          data;
            RZTextureProperties::Format    format;
            RZTextureProperties::Wrapping  wrapping;
            RZTextureProperties::Filtering filtering;
        };

        class RZTexture2DArrayCreateInfo
        {
            std::string                    name;
            u32                            width;
            u32                            height;
            u32                            layers;
            RZTextureProperties::Format    format;
            RZTextureProperties::Wrapping  wrapping;
            RZTextureProperties::Filtering filtering;
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