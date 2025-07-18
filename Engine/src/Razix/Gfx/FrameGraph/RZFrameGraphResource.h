#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

#include <cstdint>

namespace Razix {
    namespace Gfx {

        typedef i32 RZFrameGraphResource;

        constexpr u32 kInitFGResource = 0xDEAFBEEF;
        constexpr u32 kFlagsNone      = ~0;

        // TODO: Remove this as we hardly use it, we don't manage descriptors sets and resource views via FG its done via RHI itself
        // even if we make RZResourceView it will be managed via a global RHI API instead of embedding it this way
        /**
             * Dawid Kurek (skaarj1989) named it AccessDeclaration, it kinda makes sense as we have declaration on how to access the FrameGraphResource
             * but I feel having a name like Frame Graph Resource Access View makes it more readable
             */
        struct RAZIX_API RZFrameGraphResourceAcessView
        {
            RZFrameGraphResource id    = -1;         /* Unique ID of the resource                            */
            u32                  flags = kFlagsNone; /* Flags on how to view the resource from rendering POV */

            RZFrameGraphResourceAcessView(RZFrameGraphResource _id, u32 _flags)
                : id(_id), flags(_flags)
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
            kFGSampler
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