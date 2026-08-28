// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZSceneGraph.h"

// Note: this file is written in cpp only to accomodate some C++ types, such as RZAssetType and other structs
// Most of the implementation will be done in C as much as we can, no C++ containers or features will be used
// As for SceneGraph update will be delegated to ASM on a need to basis.

// TODO: We are safe to use static_cast here for pointer casting, since this is C++

// SceneGraph Lifecycle APIs
rz_scene_graph* rz_scene_graph_create(const char* name, Memory::RZHeapAllocator& heapAllocator, u32 gridDim)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(gridDim > 0, "[SceneGraph] gridDim must be > 0");

    rz_scene_graph* sg = (rz_scene_graph*) heapAllocator.allocate(sizeof(rz_scene_graph));
    if (!sg)
        return NULL;
    memset(sg, 0x00, sizeof(rz_scene_graph));

    const u32 numZones = gridDim * gridDim;

    memcpy(sg->name, name, RAZIX_ZONE_NAME_MAX);
    sg->version          = RAZIX_SCENE_GRAPH_VERSION;
    sg->gridDims[0]      = gridDim;
    sg->gridDims[1]      = gridDim;
    sg->gridDims[2]      = 1;    // Doesn't make sense in Tanu it'a a simple FPS 3d game, we don't have openworld skies.
    sg->zonePhysicalSize = RAZIX_ZONE_DIM_IN_METERS;
    sg->activeZoneIndex  = 0;
    sg->zoneCount        = numZones;

    sg->pZones = (rz_zone*) heapAllocator.allocate(sizeof(rz_zone) * numZones);
    if (!sg->pZones) {
        heapAllocator.deallocate(sg);
        return NULL;
    }

    for (u32 i = 0; i < numZones; ++i) {
        rz_zone* pZone = &sg->pZones[i];
        memset(pZone, 0, sizeof(rz_zone));

        rz_snprintf(pZone->name, RAZIX_ZONE_NAME_MAX, "<rz_zone_%u>", i);
        pZone->gridPos[0] = i % gridDim;
        pZone->gridPos[1] = i / gridDim;
        pZone->gridPos[2] = 0;
        pZone->state      = RZ_ZONE_UNLOADED;

        pZone->nodeCapacity  = RAZIX_MAX_SCENE_GRAPH_NODES;
        pZone->pNodes        = (rz_scene_node*) heapAllocator.allocate(sizeof(rz_scene_node) * pZone->nodeCapacity);
        pZone->pNodeFreelist = (u32*) heapAllocator.allocate(sizeof(u32) * pZone->nodeCapacity);

        pZone->dirtyTransforms.capacity     = RAZIX_MAX_SCENE_GRAPH_NODES;
        pZone->dirtyTransforms.pEntries     = (rz_dirty_entry*) heapAllocator.allocate(sizeof(rz_dirty_entry) * pZone->dirtyTransforms.capacity);
        pZone->dirtyTransforms.pTempEntries = (rz_dirty_entry*) heapAllocator.allocate(sizeof(rz_dirty_entry) * pZone->dirtyTransforms.capacity);

        RAZIX_CORE_ASSERT(pZone->pNodes && pZone->pNodeFreelist && pZone->dirtyTransforms.pEntries && pZone->dirtyTransforms.pTempEntries,
            "[SceneGraph] Failed to allocate zone buffers for zone {}",
            i);

        memset(pZone->pNodes, 0, sizeof(rz_scene_node) * pZone->nodeCapacity);
        memset(pZone->pNodeFreelist, 0, sizeof(u32) * pZone->nodeCapacity);
        memset(pZone->dirtyTransforms.pEntries, 0, sizeof(rz_dirty_entry) * pZone->dirtyTransforms.capacity);
        memset(pZone->dirtyTransforms.pTempEntries, 0, sizeof(rz_dirty_entry) * pZone->dirtyTransforms.capacity);

        pZone->rootNodeIndex   = 0;
        pZone->nodeCount       = 1;
        pZone->nodeFreelistTop = pZone->nodeCapacity - 1;
        for (u32 n = 0; n <= pZone->nodeCapacity - 1; ++n)
            pZone->pNodeFreelist[n] = n + 1;

        rz_scene_node* root = &pZone->pNodes[pZone->rootNodeIndex];
        root->handle        = RAZIX_ASSET_INVALID_HANDLE;
        root->flags         = RZ_NODE_FLAG_VISIBLE | RZ_NODE_FLAG_STATIC;
        root->depth         = 0;
        root->parent        = RAZIX_SCENE_NODE_NULL;
        root->firstChild    = RAZIX_SCENE_NODE_NULL;
        root->nextSibling   = RAZIX_SCENE_NODE_NULL;
    }

    return sg;
}

rz_scene_graph* rz_scene_graph_create_from_file(Memory::RZHeapAllocator& heapAllocator, const void* pFileData, u64 fileSize)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_UNUSED(heapAllocator);
    RAZIX_UNUSED(pFileData);
    RAZIX_UNUSED(fileSize);

    RAZIX_UNIMPLEMENTED_METHOD;

    RAZIX_CORE_WARN("[SceneGraph] rz_scene_graph_create_from_file() is not implemented yet");
    return NULL;
}

void rz_scene_graph_destroy(Memory::RZHeapAllocator& heapAllocator, rz_scene_graph* pSceneGraph)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(pSceneGraph, "SceneGraph is NULL, catastriphy incoming!");

    if (!pSceneGraph)
        return;

    for (u32 i = 0; i < pSceneGraph->zoneCount; ++i) {
        rz_zone* zone = &pSceneGraph->pZones[i];
        heapAllocator.deallocate(zone->pNodes);
        heapAllocator.deallocate(zone->pNodeFreelist);
        heapAllocator.deallocate(zone->dirtyTransforms.pEntries);
        heapAllocator.deallocate(zone->dirtyTransforms.pTempEntries);
    }

    heapAllocator.deallocate(pSceneGraph->pZones);
    heapAllocator.deallocate(pSceneGraph);
}

//-----------------------------------------------------------------------------
void rz_scene_graph_update(rz_scene_graph* sg, float3 observerPosition, f32 deltaTime)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_UNUSED(observerPosition);
    RAZIX_UNUSED(deltaTime);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");

    if (!sg)
        return;

    i32 currentPlayerActiveZone = rz_scene_graph_resolve_zone_index(sg, observerPosition);
    if (currentPlayerActiveZone != (i32) sg->activeZoneIndex) {
        RAZIX_CORE_ASSERT(currentPlayerActiveZone >= 0 && (u32) currentPlayerActiveZone < sg->zoneCount, "Resolved active zone index {} is out of bounds", currentPlayerActiveZone);

        RAZIX_CORE_INFO("[SceneGraph] Active zone changed from {} to {}", sg->activeZoneIndex, currentPlayerActiveZone);
        rz_zone_activate(sg, (u32) currentPlayerActiveZone);
        rz_zone_deactivate(sg, sg->activeZoneIndex);
    }

    rz_zone* activeZone = rz_scene_graph_get_active_zone(sg);
    if (!activeZone)
        return;
}
//-----------------------------------------------------------------------------

bool rz_zone_activate(rz_scene_graph* sg, u32 zoneIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg || zoneIdx >= sg->zoneCount)
        return false;

    sg->activeZoneIndex       = zoneIdx;
    sg->pZones[zoneIdx].state = RZ_ZONE_LOADED;
    return true;
}

void rz_zone_deactivate(rz_scene_graph* sg, u32 zoneIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg || zoneIdx >= sg->zoneCount)
        return;

    sg->pZones[zoneIdx].state = RZ_ZONE_UNLOADED;
}

rz_zone* rz_zone_get(rz_scene_graph* sg, u32 zoneIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    return (!sg || zoneIdx >= sg->zoneCount) ? NULL : &sg->pZones[zoneIdx];
}

const rz_zone* rz_zone_get_const(const rz_scene_graph* sg, u32 zoneIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    return (!sg || zoneIdx >= sg->zoneCount) ? NULL : &sg->pZones[zoneIdx];
}

i32 rz_zone_find_by_name(const rz_scene_graph* sg, const char* name)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    if (!sg || !name)
        return RAZIX_SCENE_NODE_NULL;

    for (u32 i = 0; i < sg->zoneCount; ++i)
        if (rz_strcmp(sg->pZones[i].name, name) == 0)
            return (i32) i;

    return RAZIX_SCENE_NODE_NULL;
}

i32 rz_zone_find_by_grid_pos(const rz_scene_graph* sg, i32 x, i32 y, i32 z)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg)
        return RAZIX_SCENE_NODE_NULL;

    for (u32 i = 0; i < sg->zoneCount; ++i) {
        const i32* p = sg->pZones[i].gridPos;
        if (p[0] == x && p[1] == y && p[2] == z)
            return (i32) i;
    }

    return RAZIX_SCENE_NODE_NULL;
}

i32 rz_scene_graph_resolve_zone_index(const rz_scene_graph* sg, float3 posInMeters)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg || sg->zonePhysicalSize <= 0.0f)
        return RAZIX_SCENE_NODE_NULL;

    // since position is in meters we need to convert it to zone grid coordinates by dividing by zonePhysicalSize
    const i32 x = (i32) floorf(posInMeters.x / sg->zonePhysicalSize);
    const i32 y = (i32) floorf(posInMeters.z / sg->zonePhysicalSize);

    if (x < 0 || y < 0 || x >= (i32) sg->gridDims[0] || y >= (i32) sg->gridDims[1])
        return RAZIX_SCENE_NODE_NULL;

    return y * (i32) sg->gridDims[0] + x;
}

u32 rz_scene_graph_get_active_zone_index(const rz_scene_graph* sg)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    return sg ? sg->activeZoneIndex : 0;
}

rz_zone* rz_scene_graph_get_active_zone(rz_scene_graph* sg)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    return (!sg || sg->activeZoneIndex >= sg->zoneCount) ? NULL : &sg->pZones[sg->activeZoneIndex];
}

i32 rz_scene_graph_attach_asset(rz_scene_graph* sg, i32 parentNodeIdx, const char* name, rz_uuid uuid, rz_asset_handle handle)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_UNUSED(name);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");

    rz_zone* zone = rz_scene_graph_get_active_zone(sg);
    if (!zone)
        return RAZIX_SCENE_NODE_NULL;

    if (parentNodeIdx == RAZIX_SCENE_NODE_NULL)
        parentNodeIdx = (i32) zone->rootNodeIndex;

    if (parentNodeIdx < 0 || (u32) parentNodeIdx >= zone->nodeCapacity)
        return RAZIX_SCENE_NODE_NULL;

    if (zone->nodeFreelistTop == 0) {
        RAZIX_CORE_ERROR("[SceneGraph] attach_asset failed, node pool exhausted in zone '{}'", zone->name);
        return RAZIX_SCENE_NODE_NULL;
    }

    const i32 nodeIdx = (i32) zone->pNodeFreelist[--zone->nodeFreelistTop];
    if ((u32) nodeIdx >= zone->nodeCapacity)
        return RAZIX_SCENE_NODE_NULL;

    rz_scene_node* node = &zone->pNodes[nodeIdx];
    memset(node, 0, sizeof(rz_scene_node));
    node->uuid        = uuid;
    node->handle      = handle;
    node->flags       = RZ_NODE_FLAG_VISIBLE | RZ_NODE_FLAG_DIRTY;
    node->depth       = (u16) (zone->pNodes[parentNodeIdx].depth + 1);
    node->parent      = parentNodeIdx;
    node->firstChild  = RAZIX_SCENE_NODE_NULL;
    node->nextSibling = zone->pNodes[parentNodeIdx].firstChild;

    // Update the parent to reference this child
    zone->pNodes[parentNodeIdx].firstChild = nodeIdx;
    if ((u32) nodeIdx >= zone->nodeCount)
        zone->nodeCount = (u32) nodeIdx + 1;

    return nodeIdx;
}

i32 rz_scene_graph_create_node(rz_scene_graph* sg, i32 parentNodeIdx, const char* name, rz_uuid uuid, Razix::RZAssetType type)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg)
        return RAZIX_SCENE_NODE_NULL;

    if (parentNodeIdx == RAZIX_SCENE_NODE_NULL && !rz_scene_graph_is_valid_root_child(type)) {
        RAZIX_CORE_ERROR("[SceneGraph] Invalid root child type {}", (u32) type);
        return RAZIX_SCENE_NODE_NULL;
    }

    const rz_asset_handle placeholder = RAZIX_ASSET_INVALID_HANDLE;
    const i32             nodeIdx     = rz_scene_graph_attach_asset(sg, parentNodeIdx, name, uuid, placeholder);
    if (nodeIdx != RAZIX_SCENE_NODE_NULL)
        sg->pZones[sg->activeZoneIndex].pNodes[nodeIdx].flags |= RZ_NODE_FLAG_PLACEHOLDER;

    return nodeIdx;
}

void rz_scene_graph_detach_node(rz_scene_graph* sg, i32 nodeIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    rz_zone* zone = rz_scene_graph_get_active_zone(sg);
    if (!zone || nodeIdx <= 0 || (u32) nodeIdx >= zone->nodeCapacity)
        return;

    rz_scene_node* node      = &zone->pNodes[nodeIdx];
    const i32      parentIdx = node->parent;
    if (parentIdx < 0 || (u32) parentIdx >= zone->nodeCapacity)
        return;

    i32* link = &zone->pNodes[parentIdx].firstChild;
    while (*link != RAZIX_SCENE_NODE_NULL && *link != nodeIdx)
        link = &zone->pNodes[*link].nextSibling;

    if (*link == nodeIdx)
        *link = node->nextSibling;

    memset(node, 0, sizeof(rz_scene_node));
    node->handle      = RAZIX_ASSET_INVALID_HANDLE;
    node->parent      = RAZIX_SCENE_NODE_NULL;
    node->firstChild  = RAZIX_SCENE_NODE_NULL;
    node->nextSibling = RAZIX_SCENE_NODE_NULL;

    if (zone->nodeFreelistTop < zone->nodeCapacity - 1)
        zone->pNodeFreelist[zone->nodeFreelistTop++] = (u32) nodeIdx;
}

void rz_scene_graph_destroy_node(rz_scene_graph* sg, struct rz_asset_db* pAssetDb, i32 nodeIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_UNUSED(pAssetDb);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    rz_scene_graph_detach_node(sg, nodeIdx);
}

rz_scene_node* rz_scene_graph_get_node(rz_scene_graph* sg, i32 nodeIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    rz_zone* zone = rz_scene_graph_get_active_zone(sg);
    return (!zone || nodeIdx < 0 || (u32) nodeIdx >= zone->nodeCapacity) ? NULL : &zone->pNodes[nodeIdx];
}

const rz_scene_node* rz_scene_graph_get_node_const(const rz_scene_graph* sg, i32 nodeIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg || sg->activeZoneIndex >= sg->zoneCount)
        return NULL;

    const rz_zone* zone = &sg->pZones[sg->activeZoneIndex];
    return (!zone || nodeIdx < 0 || (u32) nodeIdx >= zone->nodeCapacity) ? NULL : &zone->pNodes[nodeIdx];
}

i32 rz_scene_graph_find_node(const rz_scene_graph* sg, rz_uuid uuid)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg || sg->activeZoneIndex >= sg->zoneCount)
        return RAZIX_SCENE_NODE_NULL;

    const rz_zone* zone = &sg->pZones[sg->activeZoneIndex];
    for (u32 i = 0; i < zone->nodeCount; ++i)
        if (rz_uuid_compare(&zone->pNodes[i].uuid, &uuid) == 0)
            return (i32) i;

    return RAZIX_SCENE_NODE_NULL;
}

i32 rz_scene_graph_find_node_by_name(const rz_scene_graph* sg, const char* name)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    RAZIX_UNUSED(sg);
    RAZIX_UNUSED(name);
    RAZIX_CORE_WARN("[SceneGraph] find_node_by_name is not supported yet (node names are not stored in rz_scene_node)");
    return RAZIX_SCENE_NODE_NULL;
}

i32 rz_scene_graph_find_node_global(const rz_scene_graph* sg, rz_uuid uuid)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg)
        return RAZIX_SCENE_NODE_NULL;

    for (u32 z = 0; z < sg->zoneCount; ++z) {
        const rz_zone* zone = &sg->pZones[z];
        for (u32 i = 0; i < zone->nodeCount; ++i)
            if (rz_uuid_compare(&zone->pNodes[i].uuid, &uuid) == 0)
                return (i32) i;
    }

    return RAZIX_SCENE_NODE_NULL;
}

rz_asset_handle rz_scene_graph_get_transform(const rz_scene_graph* sg, i32 nodeIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    const rz_scene_node* node = rz_scene_graph_get_node_const(sg, nodeIdx);
    return node ? node->handle : RAZIX_ASSET_INVALID_HANDLE;
}

void rz_scene_graph_set_position(rz_scene_graph* sg, i32 nodeIdx, float3 posInMeters)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    RAZIX_UNUSED(posInMeters);
    rz_scene_graph_mark_dirty(sg, (u32) nodeIdx);
}

void rz_scene_graph_set_rotation(rz_scene_graph* sg, i32 nodeIdx, quat rot)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    RAZIX_UNUSED(rot);
    rz_scene_graph_mark_dirty(sg, (u32) nodeIdx);
}

void rz_scene_graph_set_scale(rz_scene_graph* sg, i32 nodeIdx, float3 scale)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    RAZIX_UNUSED(scale);
    rz_scene_graph_mark_dirty(sg, (u32) nodeIdx);
}

void rz_scene_graph_mark_dirty(rz_scene_graph* sg, u32 nodeIdx)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    if (!sg || sg->activeZoneIndex >= sg->zoneCount)
        return;

    rz_zone* zone = &sg->pZones[sg->activeZoneIndex];
    if (nodeIdx >= zone->nodeCount)
        return;

    for (u32 i = 0; i < zone->nodeCount; ++i) {
        i32 cursor = (i32) i;
        while (cursor != RAZIX_SCENE_NODE_NULL && cursor != (i32) nodeIdx)
            cursor = zone->pNodes[cursor].parent;

        if (cursor != (i32) nodeIdx)
            continue;

        zone->pNodes[i].flags |= RZ_NODE_FLAG_DIRTY;

        bool exists = false;
        for (u32 k = 0; k < zone->dirtyTransforms.count; ++k) {
            if (zone->dirtyTransforms.pEntries[k].nodeIdx == i) {
                exists = true;
                break;
            }
        }

        if (!exists && zone->dirtyTransforms.count < zone->dirtyTransforms.capacity) {
            rz_dirty_entry* entry = &zone->dirtyTransforms.pEntries[zone->dirtyTransforms.count++];
            entry->nodeIdx        = i;
            entry->depth          = zone->pNodes[i].depth;
            entry->_pad0          = 0;
        }
    }
}

void rz_scene_graph_resolve_asset(rz_scene_graph* sg, i32 nodeIdx, rz_asset_handle resolvedHandle)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_CORE_ASSERT(sg, "SceneGraph is NULL, catastriphy incoming!");
    rz_scene_node* node = rz_scene_graph_get_node(sg, nodeIdx);
    if (!node)
        return;

    node->handle = resolvedHandle;
    node->flags &= ~RZ_NODE_FLAG_PLACEHOLDER;
    node->flags |= RZ_NODE_FLAG_DIRTY;
}

//-----------------------------------------------------------------------------

rz_scene_graph_manager* rz_scene_graph_manager_create(Memory::RZHeapAllocator& heapAllocator)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    rz_scene_graph_manager* mgr = (rz_scene_graph_manager*) heapAllocator.allocate(sizeof(rz_scene_graph_manager));
    if (!mgr)
        return NULL;

    memset(mgr, 0, sizeof(rz_scene_graph_manager));
    return mgr;
}

void rz_scene_graph_manager_destroy(Memory::RZHeapAllocator& heapAllocator, rz_scene_graph_manager* mgr)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    if (mgr)
        heapAllocator.deallocate(mgr);
}

void rz_scene_graph_manager_load_scene(rz_scene_graph_manager* mgr, const char* path)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_UNUSED(mgr);
    RAZIX_CORE_WARN("[SceneGraph] load_scene not implemented yet for path '{}'", path ? path : "<null>");
}

void rz_scene_graph_manager_unload_scene(rz_scene_graph_manager* mgr)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    if (mgr)
        mgr->pActiveScene = NULL;
}

void rz_scene_graph_manager_queue_scene(rz_scene_graph_manager* mgr, const char* path)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    RAZIX_UNUSED(path);
    if (!mgr || mgr->queuedCount >= 8)
        return;

    mgr->pQueuedScenes[mgr->queuedCount++] = NULL;
}

void rz_scene_graph_manager_set_active_scene(rz_scene_graph_manager* mgr, rz_scene_graph* scene)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    if (mgr)
        mgr->pActiveScene = scene;
}

rz_scene_graph* rz_scene_graph_get_active_scene(const rz_scene_graph_manager* mgr)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    if (mgr)
        return mgr->pActiveScene;
    return NULL;
}
