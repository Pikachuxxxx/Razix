#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

#include <cstdint>

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            using RZFrameGraphResource = int32_t;

            /**
             * skaarj1989 named it AccessDeclaration, it kinda makes sense as we have declaration on how to access the FrameGraphResource
             * but I feel having a name like Frame Graph Resource Access View makes it more easily readable
             */
            struct RZFrameGraphResourceAcessView
            {
                RZFrameGraphResource id;
                u32                  flags;

                bool operator==(const RZFrameGraphResourceAcessView &) const = default;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix