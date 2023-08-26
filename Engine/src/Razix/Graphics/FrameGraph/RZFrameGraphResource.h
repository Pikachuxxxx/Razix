#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) David, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

#include <cstdint>

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            using RZFrameGraphResource = int32_t;
        }
    }
}    // namespace Razix