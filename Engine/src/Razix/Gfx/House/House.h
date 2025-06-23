#pragma once

#include "Razix/Gfx/GfxData.h"

namespace Razix {

    class RZScene;
    class RZWorld;
    class RZSceneView;

    namespace Gfx {

        // [Source]: https://github.com/skaarj1989/SupernovaEngine

        /**
         * House: High-Level Renderer for converting Asset/SceneGraph data into RHI-ready render commands for the Razix Engine.
         *
         * "House" is not just a name — it's an attitude. Inspired by Dr. Gregory House, M.D., this system approaches rendering the same way he approaches diagnostics:
         * with brutal precision, zero patience for inefficiency, and an unshakable commitment to finding the *real* problem — whether you like it or not.
         *
         * Acting as the intermediary between your scene and the GPU, House processes scene graphs, materials, cameras, and drawables into tightly batched,
         * performance-friendly data for the low-level rendering backend. It diagnoses the scene, isolates what's important, strips out what's irrelevant, and 
         * delivers a clean prescription: optimal data for the RHI layer.
         *
         *              OR
         *
         * I DON’T CARE IF IT’S A DEFERRED PIPELINE OR A FORWARD+ PATH — IT’S NEVER LUPUS.
         * I JUST REALLY LOVE HOUSE, OKAY? LET ME NAME SOMETHING AFTER HIM!!!
         */
        namespace House {

            RZWorld*  BuildRazixWorld(RZScene* scenes, RZSceneView* views);
            Drawables BuildDrawables(RZWorld* world);
            Batches   BuildBatches(Drawables* drawables);

            RZSceneView* ExtractSceneViews(RZScene* scene);
            DrawData*    ExtractBatchDrawData(Batch* batch);
            DrawData*    ExtractDrawableDrawData(Drawable* drawable);
        }    // namespace House
    }        // namespace Gfx
}    // namespace Razix