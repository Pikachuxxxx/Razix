#pragma once

#include "Razix/Graphics/Pompeius/RZRenderDataStructs.h"

namespace Razix {

    class RZScene;
    class RZWorld;
    class RZSceneView;

    namespace Graphics {

        // [Source]: https://github.com/skaarj1989/SupernovaEngine

        /**
         * Pompeius: High Level Renderer for Scene to RHI data conversion for Razix Engine. 
         * "Pompeius" is inspired by the renowned Roman general and statesman, Pompey the Great, known for his military prowess and strategic acumen. 
         * In the context of our High Level Render API, Pompeius embodies the idea of leadership and optimization, reflecting its role in translating
         * complex scene data into efficient, RHI-friendly structures. 
         * Pompeius's strategic approach lies in its ability to abstract the complexities of rendering, much like Pompey's ability to navigate the complexities
         * of Roman politics and warfare. By acting as a bridge between high-level scene data and low-level rendering APIs, Pompeius streamlines the rendering 
         * pipeline, optimizing performance and enabling developers to achieve stunning visual results with greater ease.
         */
        namespace Pompeius {

            RZSceneView* ExtractSceneViews(RZScene* scene);
            RZWorld*     BuildRazixWorld(RZScene* scenes, RZSceneView* views);
            Drawables    BuildDrawables(RZWorld* world);
            Batches      BuildBatches(Drawables* drawables);

        }    // namespace Pompeius
    }        // namespace Graphics
}    // namespace Razix