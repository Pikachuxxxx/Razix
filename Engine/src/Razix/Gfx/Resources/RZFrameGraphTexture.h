#pragma once

namespace Razix {
    namespace Gfx {

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

            struct RZFrameGraphTexture
            {
                typedef RZTextureDesc Desc;

                void create(const Desc& desc, void* transientAllocator);
                void destroy(const Desc& desc, void* transientAllocator);

                void preRead(const Desc& desc, u32 flags);
                void preWrite(const Desc& desc, u32 flags);

                void resize(u32 width, u32 height);

                static std::string toString(const Desc& desc);

                Gfx::RZTextureHandle getHandle() const { return m_TextureHandle; }

                // public for initializer list support
                Gfx::RZTextureHandle m_TextureHandle;
            };
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix