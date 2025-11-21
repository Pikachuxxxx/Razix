#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        typedef i32 RZFrameGraphResource;

#if INTPTR_MAX == INT64_MAX
        constexpr uintptr_t kFGResViewResInvalidTag       = 0x0000'DEAD'BEEF'BAADull;
        constexpr uintptr_t kFGResViewResAutoFillTag      = 0x0000'F111'FEED'BEEFull;
        constexpr uintptr_t kFGResViewResIgnoreResViewTag = 0x0000'FEE1'BAAD'BEEFull;
        ;
#elif INTPTR_MAX == INT32_MAX
        constexpr uintptr_t kFGResViewResInvalidTag       = 0xDEAD'BEEFu;
        constexpr uintptr_t kFGResViewResAutoFillTag      = 0xF111'FEEDu;
        constexpr uintptr_t kFGResViewResIgnoreResViewTag = 0xBAAD'BEEFu;
#else
    #error "Unsupported pointer size"
#endif

        inline const rz_gfx_texture* RZ_FG_TEX_RES_INVALID       = reinterpret_cast<const rz_gfx_texture*>(kFGResViewResInvalidTag);
        inline const rz_gfx_texture* RZ_FG_TEX_RES_AUTO_POPULATE = reinterpret_cast<const rz_gfx_texture*>(kFGResViewResAutoFillTag);
        inline const rz_gfx_texture* RZ_FG_TEX_RES_VIEW_IGNORE   = reinterpret_cast<const rz_gfx_texture*>(kFGResViewResIgnoreResViewTag);

        inline const rz_gfx_buffer* RZ_FG_BUF_RES_INVALID       = reinterpret_cast<const rz_gfx_buffer*>(kFGResViewResInvalidTag);
        inline const rz_gfx_buffer* RZ_FG_BUF_RES_AUTO_POPULATE = reinterpret_cast<const rz_gfx_buffer*>(kFGResViewResAutoFillTag);
        inline const rz_gfx_buffer* RZ_FG_BUF_RES_VIEW_IGNORE   = reinterpret_cast<const rz_gfx_buffer*>(kFGResViewResIgnoreResViewTag);

        template<typename T>
        inline bool fg_is_tagged(const T* p, uintptr_t tag)
        {
            return reinterpret_cast<uintptr_t>(p) == tag;
        }

        inline bool fg_is_tex_auto(const rz_gfx_texture* p)
        {
            return fg_is_tagged(p, kFGResViewResAutoFillTag);
        }
        inline bool fg_is_tex_invalid(const rz_gfx_texture* p)
        {
            return fg_is_tagged(p, kFGResViewResInvalidTag);
        }

        inline bool fg_is_buf_auto(const rz_gfx_buffer* p)
        {
            return fg_is_tagged(p, kFGResViewResAutoFillTag);
        }
        inline bool fg_is_buf_invalid(const rz_gfx_buffer* p)
        {
            return fg_is_tagged(p, kFGResViewResInvalidTag);
        }

        /**
          * Dawid Kurek (skaarj1989) named it AccessDeclaration, it kinda makes sense as we have declaration on how to access the FrameGraphResource
          * but I feel having a name like Frame Graph Resource Access View makes it more readable
          */
        // rz_gfx_resource_view will be owned by per pass
        struct RAZIX_API RZFrameGraphResourceAcessView
        {
            RZFrameGraphResource id = -1;
            // FIXME: Even RZResourceEntry Model class stores Desc
            // we can't get away without storing both the desc and resource
            // RHI is smart but engine isn't unless I come up with a brilliant idea
            // Or GPUTrain will eliminate the need for this, until then this sloppy solution is acceptable!
            rz_gfx_resource_view_desc   resViewDesc   = {};
            rz_gfx_resource_view_handle resViewHandle = {};

            RZFrameGraphResourceAcessView()
            {
                // By default set invalid resources, pBuffer is enough to early exit
                resViewDesc.bufferViewDesc.pBuffer = RZ_FG_BUF_RES_INVALID;
            }

            RZFrameGraphResourceAcessView(RZFrameGraphResource _id, rz_gfx_resource_view_desc viewDesc)
                : id(_id), resViewDesc(viewDesc)
            {
            }

            bool operator==(const RZFrameGraphResourceAcessView& view) const
            {
                return id == view.id;
            }
        };

        enum class LifeTimeMode
        {
            kCoarse,
            kImported,
            kRead,     // Fine grained
            kWrite,    // Fine grained
        };

        enum FGResourceType
        {
            kUknownResource,
            kFGTexture,
            kFGBuffer,
        };

        struct RZResourceLifetime
        {
            u32          ResourceEntryID;
            u32          StartPassID;
            u32          EndPassID;
            LifeTimeMode Mode;
        };
    }    // namespace Gfx
}    // namespace Razix