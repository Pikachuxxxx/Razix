#ifndef _RZ_SCENE_GRAPH_H_
#define _RZ_SCENE_GRAPH_H_

#include "RAzix/Core/RZDataTypes.h"
#include "Razix/Core/RZCore.h"

#include "Razix/Core/UUID/RZUUID.h"

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Math/RZAABB.h"

//-----------------------------------------------------------------------------
// Constants & Defines
//-----------------------------------------------------------------------------

#define RZ_SCENE_NODE_NULL     (-1)
#define RZ_ZONE_NAME_MAX       64
#define RZ_NODE_NAME_MAX       64
#define RZ_ZONE_MAX            512
#define RZ_SCENE_GRAPH_VERSION 1

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

//-----------------------------------------------------------------------------
// Enums & Flags
//-----------------------------------------------------------------------------

typedef enum rz_node_flags
{
    RZ_NODE_FLAG_NONE        = 0,
    RZ_NODE_FLAG_DIRTY       = (1 << 0),    // transform or asset changed
    RZ_NODE_FLAG_VISIBLE     = (1 << 1),
    RZ_NODE_FLAG_STATIC      = (1 << 2),    // never moves at runtime, even it's chilren are static
    RZ_NODE_FLAG_PLACEHOLDER = (1 << 3),    // asset still loading async
} rz_node_flags;

//-----------------------------------------------------------------------------
// Internal Structures
//-----------------------------------------------------------------------------

// Gather List: Per-type flat array of active node indices in a zone

typedef struct rz_gather_list
{
    u32* pIndices;    // node indices of this asset type
    u32  count;
    u32  capacity;
} rz_gather_list;

// Dirty Entry: Packed for cache-efficient sorting
typedef struct rz_dirty_entry
{
    u32 nodeIdx;
    u16 depth;
    u16 _pad0;    // Keep struct 8-byte aligned for SIMD/Copy efficiency
} rz_dirty_entry;

// Dirty Transform List: Sorted by depth for correct parent-before-child world matrix recomputation

typedef struct rz_dirty_list
{
    rz_dirty_entry* pEntries;
    rz_dirty_entry* pTempEntries; // Double buffer for Radix Sort
    u32             count;
    u32             capacity;
} rz_dirty_list;

//-----------------------------------------------------------------------------
// Main Structures
//-----------------------------------------------------------------------------

// Scene Node: Lightweight hierarchy node. Data is stored in Asset Pools.

typedef struct rz_scene_node
{
    char            name[RZ_NODE_NAME_MAX];
    rz_uuid         uuid;      
    rz_asset_handle handle;    
    u32             flags;     // RZNodeFlags bitmask
    u16             depth;     // distance from root
    u16             _pad0;
    i32             parent;    // indices into zone->nodes[], -1 = null
    i32             firstChild;
    i32             nextSibling;
} rz_scene_node;

// Zone State

typedef enum rz_zone_state
{
    RZ_ZONE_UNLOADED = 0,
    RZ_ZONE_LOADING  = 1,
    RZ_ZONE_LOADED   = 2,
} rz_zone_state;

// Zone: Logical separation of assets and nodes for streaming

typedef struct rz_zone
{
    char           name[RZ_ZONE_NAME_MAX];
    i32            gridPos[3];
    rz_aabb        bounds;
    rz_zone_state  state;
    rz_scene_node* pNodes;
    u32            nodeCount;
    u32            nodeCapacity;
    u32            rootNodeIndex;    // always 0
    u32*           pNodeFreelist;
    u32            nodeFreelistTop;
    rz_gather_list gather[RAZIX_ASSET_TYPE_COUNT];
    rz_dirty_list  dirtyTransforms;
    struct rz_pak* pPak;    // NULL in development builds
    u64            fileOffset;
    u64            fileSize;
} rz_zone;

// Global Trigger

typedef struct rz_global_trigger
{
    char    name[RZ_NODE_NAME_MAX];
    rz_aabb bounds;
    u32     eventHash;
} rz_global_trigger;

// Scene Graph

typedef struct rz_scene_graph
{
    u32                version;
    rz_aabb sceneBounds;
    u32                gridDims[3];
    f32                zonePhysicalSize;
    rz_zone            zones[RZ_ZONE_MAX];
    u32                zoneCount;
    u32                activeZoneIndex;
    rz_global_trigger* pTriggers;
    u32                triggerCount;
    void*              pMmapBase;
    u64                mmapSize;
} rz_scene_graph;

// Scene Graph Manager

typedef struct rz_scene_graph_manager
{
    rz_scene_graph*     pActiveScene;
    rz_scene_graph*     pQueuedScenes[8];
    u32                 queuedCount;
} rz_scene_graph_manager;

//-----------------------------------------------------------------------------
// API Functions
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Lifecycle
RAZIX_API rz_scene_graph* rz_scene_graph_create(const void* pFileData, u64 fileSize);
RAZIX_API void            rz_scene_graph_destroy(rz_scene_graph* sg);

// Master update — called once per frame by game thread
RAZIX_API void rz_scene_graph_update(rz_scene_graph* sg,
    float3                                          observerPosition,
    f32                                              deltaTime);

// Zone Lifecycle
RAZIX_API bool rz_zone_activate(rz_scene_graph* sg, u32 zoneIdx);
RAZIX_API void rz_zone_deactivate(rz_scene_graph* sg, u32 zoneIdx);

RAZIX_API rz_zone*       rz_zone_get(rz_scene_graph* sg, u32 zoneIdx);
RAZIX_API const rz_zone* rz_zone_get_const(const rz_scene_graph* sg, u32 zoneIdx);
RAZIX_API i32            rz_zone_find_by_name(const rz_scene_graph* sg, const char* name);
RAZIX_API i32            rz_zone_find_by_grid_pos(const rz_scene_graph* sg, i32 x, i32 y, i32 z);
RAZIX_API i32            rz_scene_graph_resolve_zone_index(const rz_scene_graph* sg, float3 pos);
RAZIX_API u32            rz_scene_graph_get_active_zone_index(const rz_scene_graph* sg);
RAZIX_API rz_zone*       rz_scene_graph_get_active_zone(rz_scene_graph* sg);

// Node Operations

// Attach pre-created asset to tree
RAZIX_API i32 rz_scene_graph_attach_asset(rz_scene_graph* sg,
    i32                                                   parentNodeIdx,
    const char*                                           name,
    rz_uuid                                               uuid,
    rz_asset_handle                                       handle);

// Convenience: create asset + attach to tree
RAZIX_API i32 rz_scene_graph_create_node(rz_scene_graph* sg,
    struct rz_asset_db*                                  pAssetDb,
    i32                                                  parentNodeIdx,
    const char*                                          name,
    rz_uuid                                              uuid,
    Razix::RZAssetType                                        type);

// Detach node from tree (keeps asset alive)
RAZIX_API void rz_scene_graph_detach_node(rz_scene_graph* sg, i32 nodeIdx);

// Detach + destroy asset
RAZIX_API void rz_scene_graph_destroy_node(rz_scene_graph* sg,
    struct rz_asset_db*                                    pAssetDb,
    i32                                                    nodeIdx);

// Node Queries
RAZIX_API rz_scene_node*       rz_scene_graph_get_node(rz_scene_graph* sg, i32 nodeIdx);
RAZIX_API const rz_scene_node* rz_scene_graph_get_node_const(const rz_scene_graph* sg, i32 nodeIdx);

RAZIX_API i32 rz_scene_graph_find_node(const rz_scene_graph* sg, rz_uuid uuid);
RAZIX_API i32 rz_scene_graph_find_node_by_name(const rz_scene_graph* sg, const char* name);
RAZIX_API i32 rz_scene_graph_find_node_global(const rz_scene_graph* sg, rz_uuid uuid);

// Transforms

// Get the transform asset for this node (walks up tree if node is not a transform)
RAZIX_API rz_asset_handle rz_scene_graph_get_transform(const rz_scene_graph* sg, i32 nodeIdx);

RAZIX_API void rz_scene_graph_set_position(rz_scene_graph* sg, i32 nodeIdx, float3 pos);
RAZIX_API void rz_scene_graph_set_rotation(rz_scene_graph* sg, i32 nodeIdx, quat rot);
RAZIX_API void rz_scene_graph_set_scale(rz_scene_graph* sg, i32 nodeIdx, float3 scale);

// Internal: recompute dirty transforms in active zones
RAZIX_API void rz_scene_graph_update_transforms(rz_scene_graph* sg);

// Marks a node and its entire subtree as dirty (iterative propagation).
// Adds them to the dirty list for batch processing.
RAZIX_API void rz_scene_graph_mark_dirty(rz_scene_graph* sg, u32 nodeIdx);

// Sorts the dirty list by depth using LSB Radix Sort (2 passes for u16 depth).
// This ensures parents are processed before children, enabling a linear,
// non-recursive update loop.
// Complexity: O(N) where N is dirty count.
RAZIX_API void rz_scene_graph_sort_dirty_list(rz_scene_graph* sg);

// Traversal

typedef bool (*rz_node_visit_fn)(rz_scene_graph* sg, i32 nodeIdx, u32 depth, void* user);

RAZIX_API void rz_scene_graph_traverse_depth_first(rz_scene_graph* sg, i32 startNode,
    rz_node_visit_fn visit, void* user);
RAZIX_API void rz_scene_graph_traverse_breadth_first(rz_scene_graph* sg, i32 startNode,
    rz_node_visit_fn visit, void* user);

// Asset Resolution

// Called by asset system when async load completes
RAZIX_API void rz_scene_graph_resolve_asset(rz_scene_graph* sg, i32 nodeIdx,
    rz_asset_handle resolvedHandle);

// Serialization
RAZIX_API u64 rz_zone_serialize_sexp(const rz_zone* zone, void* pOutBuf, u64 bufSize);
RAZIX_API u64 rz_scene_graph_serialize(const rz_scene_graph* sg, void* pOutBuf, u64 bufSize);

//-----------------------------------------------------------------------------
// Scene Graph Manager API
//-----------------------------------------------------------------------------

RAZIX_API rz_scene_graph_manager* rz_scene_graph_manager_create(struct rz_asset_db* pDb);
RAZIX_API void                    rz_scene_graph_manager_destroy(rz_scene_graph_manager* mgr);

RAZIX_API void rz_scene_graph_manager_load_scene(rz_scene_graph_manager* mgr, const char* path);
RAZIX_API void rz_scene_graph_manager_unload_scene(rz_scene_graph_manager* mgr);
RAZIX_API void rz_scene_graph_manager_queue_scene(rz_scene_graph_manager* mgr, const char* path);

//-----------------------------------------------------------------------------
// S-Expression Tokenizer (SIMD-accelerated)
//-----------------------------------------------------------------------------

typedef enum rz_sexp_token_type
{
    RZ_SEXP_LPAREN,
    RZ_SEXP_RPAREN,
    RZ_SEXP_ATOM,
    RZ_SEXP_STRING,
} rz_sexp_token_type;

typedef struct rz_sexp_token
{
    u32                offset;
    u32                length;
    rz_sexp_token_type type;
} rz_sexp_token;

typedef struct rz_sexp_tokenizer_result
{
    rz_sexp_token* pTokens;
    u32            count;
    u32            capacity;
} rz_sexp_tokenizer_result;

RAZIX_API rz_sexp_tokenizer_result rz_sexp_tokenize(const char* src, u64 srcSize);
RAZIX_API void                     rz_sexp_tokenizer_free(rz_sexp_tokenizer_result* result);

RAZIX_API bool rz_sexp_build_zone(rz_zone* zone, const char* src,
    const rz_sexp_tokenizer_result* tokens);

RAZIX_API bool rz_sexp_parse_scene_header(rz_scene_graph* sg, const char* src,
    const rz_sexp_tokenizer_result* tokens);

//-----------------------------------------------------------------------------

/* ------------------ SceneGraph Rules ------------------ */

static const u32 s_scene_graph_child_rules[(int)Razix::RZAssetType::COUNT] = {
    [(int)Razix::RZAssetType::kTransform] = (1 << (int)Razix::RZAssetType::kMesh) | (1 << (int)Razix::RZAssetType::kLight) | (1 << (int)Razix::RZAssetType::kCamera) | (1 << (int)Razix::RZAssetType::kTransform),
    [(int)Razix::RZAssetType::kLuaScript] = (1 << (int)Razix::RZAssetType::kLuaScript),
    [(int)Razix::RZAssetType::kMesh]      = (1 << (int)Razix::RZAssetType::kMaterial),
    // Leaf nodes
    [(int)Razix::RZAssetType::kMaterial]  = 0,
    [(int)Razix::RZAssetType::kTexture]   = 0,
    [(int)Razix::RZAssetType::kLight]     = 0,
    [(int)Razix::RZAssetType::kCamera]    = 0,
    [(int)Razix::RZAssetType::kAudio]     = 0,
    [(int)Razix::RZAssetType::kCloth]     = 0,
};

/* Only TRANSFORM and SCRIPT can be direct children of a zone root */
static const u32 s_scene_graph_root_valid_mask =
    (1 << (int)Razix::RZAssetType::kTransform) | (1 << (int)Razix::RZAssetType::kLuaScript);

/* internal validator */
static inline bool
rz_scene_graph_is_valid_child(Razix::RZAssetType parent, Razix::RZAssetType child)
{
    return (s_scene_graph_child_rules[(int)parent] & (1 << (int)child)) != 0;
}

static inline bool
rz_scene_graph_is_valid_root_child(Razix::RZAssetType type)
{
    return (s_scene_graph_root_valid_mask & (1 << (int)type)) != 0;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif    // _RZ_SCENE_GRAPH_H_
