// clang-format off
#include "rzxpch.h"
#include <Core/Containers/string.h>
#include <Core/RZCore.h>
#include <cstring>
// clang-format on
#include "RZSceneGraph.h"

// Note: this file is written in cpp only to accomodate some C++ types, such as RZAssetType and other structs
// Most of the implementation will be done in C as much as we can, no C++ containers or features will be used
// As for SceneGraph update will be delegated to ASM on a need to basis.

// TODO: We are safe to use static_cast here for pointer casting, since this is C++

// SceneGraph Lifecycle APIs
rz_scene_graph* rz_scene_graph_create(Memory::RZHeapAllocator& heapAllocator, u32 gridDim)
{
    rz_scene_graph* sg = (rz_scene_graph*) heapAllocator.allocate(sizeof(rz_scene_graph));
    memset(sg, 0x00, sizeof(rz_scene_graph));

    u32 numZones = gridDim * gridDim;

    // This is dynamically updated based on the meshes in the scene, this is computed based on zones aabb
    // sg->sceneBounds

    sg->version          = RAZIX_SCENE_GRAPH_VERSION;
    sg->gridDims[0]      = gridDim;
    sg->gridDims[1]      = gridDim;
    sg->gridDims[2]      = 1;
    sg->zonePhysicalSize = RAZIX_ZONE_DIM_IN_METERS;
    sg->activeZoneIndex  = 0;    // yep this is the default zone

    sg->pZones = (rz_zone*) heapAllocator.allocate(sizeof(rz_zone) * numZones);

    for (u32 i = 0; i < numZones; ++i) {
        rz_zone* pZone = &sg->pZones[i];

        memset(pZone, 0, sizeof(rz_zone));

        rz_snprintf(pZone->name, RAZIX_ZONE_NAME_MAX, "<rz_zone_%u>", i);

        pZone->gridPos[0] = i % gridDim;
        pZone->gridPos[1] = i / gridDim;
        pZone->gridPos[2] = 1;
    }

    return sg;
}

rz_scene_graph* rz_scene_graph_create_from_file(Memory::RZHeapAllocator& heapAllocator, const char* pFileData, u64 fileSize)
{
    RAZIX_UNUSED(heapAllocator);
    RAZIX_UNUSED(pFileData);
    RAZIX_UNUSED(fileSize);

    RAZIX_UNIMPLEMENTED_METHOD;
    return NULL;
}

void rz_scene_graph_destroy(Memory::RZHeapAllocator& heapAllocator, rz_scene_graph* pSceneGraph)
{
    if (pSceneGraph) {
        heapAllocator.deallocate(pSceneGraph->pZones);
        heapAllocator.deallocate(pSceneGraph);
    }
}
