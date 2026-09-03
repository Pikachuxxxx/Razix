#ifndef RZ_WORLD_H
#define RZ_WORLD_H

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/RZHandle.h"

#include "Razix/Gfx/Renderers/RZRendererSettings.h"

#include "Razix/Gfx/Cameras/RZCamera3D.h"

namespace Razix {

    class RZCamera3D;
    struct LightsData;

    // struct Batch
    // {
    //     rz_handle geometry;
    //     rz_handle pso;
    //     rz_handle material;
    //     u64       _pad0;
    // };

    struct DrawData
    {
        u32 RESERVED_drawBatchIdx = 0;
        u32 vertexCount           = 0;
        u32 vertexOffset          = 0;
        u32 indexCount            = 0;
        u32 indexOffset           = 0;
        u32 instanceCount         = 0;
        u32 transformIdx          = 0xFFFFFFFF;
        u32 geometryIdx           = 0xFFFFFFFF;
    };

    struct RZWorld
    {
        u32                            worldInFlightIdx = 0;    // index of the current world buffer in flight, used for debugging only
        RZDynamicArray<DrawData>       drawdata;
        RZDynamicArray<rz_handle>      lights;
        RZCamera3D                     primaryCamera;
        rz_handle                      sceneRenderTarget;    // TODO: do we really need this in RZWorld here?
        Gfx::RZRendererSettings        rendererSettings;
    };

}    // namespace Razix
#endif    // RZ_WORLD_H
