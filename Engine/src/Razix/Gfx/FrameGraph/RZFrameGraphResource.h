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
        namespace FrameGraph {

            using RZFrameGraphResource = int32_t;

            constexpr u32 kFlagsNone = ~0;

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
        }    // namespace FrameGraph
    }    // namespace Gfx
}    // namespace Razix