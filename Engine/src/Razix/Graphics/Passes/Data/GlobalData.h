#pragma once

#include "Razix/Graphics/FrameGraph/Resources/RZTransientResources.h"

namespace Razix {
    namespace Graphics {

        /**
         * Global light probes for PBR lighting
         */
        struct GlobalLightProbe
        {
            RZCubeMap* diffuse;
            RZCubeMap* specular;
        };

        // Default pass data types
        struct RTOnlyPassData
        {
            FrameGraph::RZFrameGraphResource outputRT;          /* Render Texture to which this pass is drawn to                                                */
            FrameGraph::RZFrameGraphResource passDoneSemaphore; /* Semaphore to notify the command buffers that this pass has completed execution on the GPU    */
        };

        struct RTDTPassData
        {
            FrameGraph::RZFrameGraphResource outputRT;
            FrameGraph::RZFrameGraphResource depthRT;
            FrameGraph::RZFrameGraphResource passDoneSemaphore;
        };
    }    // namespace Graphics
}    // namespace Razix