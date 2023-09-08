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

                void create(const Desc& desc, void* transientAllocator);
                void destroy(const Desc& desc, void* transientAllocator);

                static std::string toString(const Desc& desc);

                void preRead(const Desc& desc, uint32_t flags);
                void preWrite(const Desc& desc, uint32_t flags);

                Graphics::RZTextureHandle getHandle() { return m_TextureHandle; }

            public:
                Graphics::RZTextureHandle m_TextureHandle;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix