#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"

namespace Razix {
    namespace Graphics {

        struct RZTextureDesc;

        namespace FrameGraph {

            class RZFrameGraphTexture
            {
            public:
                typedef RZTextureDesc Desc;

                void create(const Desc& desc, void* allocator);
                void destroy(const Desc& desc, void* allocator);

                static std::string toString(const Desc& desc);

                Graphics::RZTexture* getHandle() { return m_Texture; }

            public:
                // TODO: Replace it with a RZTextureHandle
                Graphics::RZTexture* m_Texture;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix