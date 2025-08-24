#pragma once

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        /**
             * When an extra wrapper of RZTexture and rz_texture_handle?
             * 
             * It's messy to enforce the Type Erasure concepts on the Graphics API directly,
             * by using an external class it decouples FG and Graphics API
             * 
             * uses TypeErasure to wrap different API handles under a common interface
             * 
             * This is a fake typeless interface for various types of resources used by the FG
             */

        struct RAZIX_API RZFrameGraphTexture
        {
            typedef rz_gfx_texture_desc Desc;

            void create(const std::string& name, const Desc& desc, u32 id, const void* transientAllocator);
            void destroy(u32 id, const void* transientAllocator);

            void preRead(const Desc& desc, u32 flags);
            void preWrite(const Desc& desc, u32 flags);

            void resize(u32 width, u32 height);

            static std::string toString(const Desc& desc);

            rz_gfx_texture_handle getRHIHandle() const { return m_TextureHandle; }

            // public for initializer list support
            rz_gfx_texture_handle m_TextureHandle;
        };
    }    // namespace Gfx
}    // namespace Razix