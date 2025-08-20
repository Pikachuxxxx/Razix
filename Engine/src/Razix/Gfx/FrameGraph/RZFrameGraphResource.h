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

        constexpr u32 kInitFGResViewResInvalid  = 0xDEADBEEF;
        constexpr u32 kInitFGResViewResAutoFill = 0xF111FEED;
#define RZ_FG_RES_VIEW_RES_AUTO_POPULATE ((void*) kInitFGResViewResAutoFill)

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
            // Or GPUTrain will eliminate the need for this, until this sloppy solution is acceptable!
            rz_gfx_resource_view_desc   resViewDesc   = {};
            rz_gfx_resource_view_handle resViewHandle = {};

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
            kFGSampler,
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