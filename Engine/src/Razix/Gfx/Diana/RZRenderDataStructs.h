#pragma once

#include "Razix/Gfx/RHI/API/RZAPIHandles.h"

// TODO: Remove this header!
#include <glm/glm.hpp>

namespace Razix {
    namespace Gfx {

        // [Source]: https://github.com/skaarj1989/SupernovaEngine

        /**
         * Drawable is a conversion of an actor/entity that will be rendered onto the scene
         */
        struct Drawable
        {
            RZMaterialHandle material;
            RZMeshHandle     mesh;
            glm::mat4        transform;
        };

        using Drawables = std::vector<Drawable>;

        /**
         * Drawables are batched by their common VB/IB and material pools
         */
        struct Batch
        {
            RZVertexBufferHandle vertexBuffer;
            RZIndexBufferHandle  indexBuffer;
            RZPipelineHandle     pso;
        };

        using Batches = std::vector<Batch>;

        /**
         * DrawData is use for Bindless Rendering 
         */
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

        /**
         * Used to cache render commands for drawing batches b/w frames.
         */
        struct DrawCommandLists
        {
        };
    }    // namespace Graphics
}    // namespace Razix