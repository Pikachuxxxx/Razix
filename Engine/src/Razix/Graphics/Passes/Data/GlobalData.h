#pragma once

#include "Razix/Graphics/FrameGraph/Resources/RZTransientResources.h"
#include "Razix/Graphics/Lighting/LightData.h"

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

        struct SceneData
        {
            FrameGraph::RZFrameGraphResource outputHDR;
            FrameGraph::RZFrameGraphResource outputLDR;
            FrameGraph::RZFrameGraphResource depth;
            FrameGraph::RZFrameGraphResource passDoneSemaphore;
        };

        struct SceneLightsData
        {
            FrameGraph::RZFrameGraphResource lightsDataBuffer;
        };

        /**
         * Lights Data which will be uploaded to the GPU
         */
        struct GPULightsData
        {
            alignas(4) u32 numLights   = 0;
            alignas(4) u32 _padding[3] = {0, 0, 0};    // Will be consumed on GLSL so as to get 16 byte alignment, invisible variable on GLSL
            alignas(16) LightData lightData[MAX_LIGHTS];
        };
    }    // namespace Graphics
}    // namespace Razix