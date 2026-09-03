#pragma once

#include "Razix/Gfx/RZWorld.h"

struct rz_scene_graph;

namespace Razix {

    namespace Gfx {

        // [Source]: Inspited by my dear friend Dawid's https://github.com/skaarj1989/SupernovaEngine

        /**
         * House: High-Level Renderer for converting Asset/SceneGraph data into RHI-ready render commands for the Razix Engine.
         * I DON'T CARE IF IT'S A DEFERRED PIPELINE OR A FORWARD+ PATH IT'S NEVER LUPUS.
         *
         * How and where is this API used?
         * As a first step this is used in the GameThread to convert the scene graph data into RZWorld to handoff to the RenderThread 
         * Once that is done RenderThead can use the RZWorld internally to build drawable, batches and final draw data for more optimal data
         * before any framegraph execution happens we do further processing of the RZWorld data inside the render thread and hand off 
         * to final frame graph execution.
         * '
         */
        namespace House {

#if 0
        // These structs are only used by the Render Thread for building RHI commands 

#endif

            RZWorld BuildRazixWorldFromSceneData(const rz_scene_graph* sceneGraph);

            // TODO: Will be implemented when asset system is done and Render thread is in full effect
            //Drawables BuildDrawables(RZWorld* world);
            //Batches   BuildBatches(Drawables* drawables);
            //
            //RZSceneView* ExtractSceneViews(RZScene* scene);
            //DrawData*    ExtractBatchDrawData(Batch* batch);
            //DrawData*    ExtractDrawableDrawData(Drawable* drawable);
        }    // namespace House
    }    // namespace Gfx
}    // namespace Razix
