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
         * I DON’T CARE IF IT’S A DEFERRED PIPELINE OR A FORWARD+ PATH — IT’S NEVER LUPUS.
         */
        namespace House {

#if 0

        struct Drawable
        {
            rz_handle material;
            rz_handle     mesh;
            uint32_t         transformID;
        };

        using Drawables = RZFixedArray<Drawable, 1024>; // 1024 drawables per job


        struct Batch
        {
            rz_handle vertexBuffer;
            rz_handle  indexBuffer;
            rz_handle     pso;
        };

        using Batches = RZFixedArray<Batch, 1024>; // 1024 batches per job


        struct DrawData
        {
            u32 drawBatchIdx  = 0;
            u32 drawableIdx   = 0;
            u32 vertexCount   = 0;
            u32 vertexOffset  = 0;
            u32 indexCount    = 0;
            u32 indexOffset   = 0;
            u32 instanceCount = 0;
            u32 _padding      = 0;
        };

        struct DrawCommandLists
        {
        };

#endif

            //RZWorld*  BuildRazixWorld(RZScene* scenes, RZSceneView* views);
            //Drawables BuildDrawables(RZWorld* world);
            //Batches   BuildBatches(Drawables* drawables);
            //
            //RZSceneView* ExtractSceneViews(RZScene* scene);
            //DrawData*    ExtractBatchDrawData(Batch* batch);
            //DrawData*    ExtractDrawableDrawData(Drawable* drawable);
        }    // namespace House
    }    // namespace Gfx
}    // namespace Razix