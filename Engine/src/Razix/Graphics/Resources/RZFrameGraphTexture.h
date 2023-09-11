#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"

namespace Razix {
    namespace Graphics {

        struct RZTextureDesc;

        namespace FrameGraph {

            /**
             * When an extra wrapper of RZTexture and RZTextureHandle?
             * 
             * It's messy to enforce the Type Erasure concepts on the Graphics API directly,
             * by using an external class it decouples FG and Graphics API
             * 
             * uses TypeErasure to wrap different API handles under a common interface
             * 
             * This is a fake typeless interface for various types of resources used by the FG
             */

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