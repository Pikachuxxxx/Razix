#pragma once

#include "Razix/Graphics/Diana/RZRenderDataStructs.h"

namespace Razix {

    class RZScene;
    class RZWorld;
    class RZSceneView;

    namespace Graphics {

        // [Source]: https://github.com/skaarj1989/SupernovaEngine

        /**
         * Diana: High Level Renderer for Scene to RHI data conversion for Razix Engine. 
         * "Diana" is not just a name; it's a symbol of elegance, compassion, and innovation. Inspired by the legacy of Princess Diana, 
         * known for her grace and humanitarian efforts, this High Level Render API embodies these qualities in its approach to rendering technology.
         * Much like Princess Diana's ability to connect with people from all walks of life, Diana, the API, aims to bridge the gap between
         * high-level scene data and low-level rendering APIs. Diana's commitment to optimization and performance is akin to Princess Diana's dedication to making a positive impact.
         * Diana serves as a compassionate and capable intermediary in the world of rendering technology. 
         * By embodying the qualities of Princess Diana - grace, compassion, and innovation.
         */
        namespace Diana {

            RZSceneView* ExtractSceneViews(RZScene* scene);
            RZWorld*     BuildRazixWorld(RZScene* scenes, RZSceneView* views);
            Drawables    BuildDrawables(RZWorld* world);
            Batches      BuildBatches(Drawables* drawables);
        }    // namespace Diana
    }        // namespace Graphics
}    // namespace Razix