#pragma once

namespace Razix {
    namespace Graphics {

        /**
         * FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
         * Adapted from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in 
         */
        class RAZIX_API RZFrameGraph
        {
            RZFrameGraph()  = default;
            ~RZFrameGraph() = default;
        };
    }    // namespace Graphics
}    // namespace Razix
