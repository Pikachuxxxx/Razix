# [Draft-1] Asset System Design 

## Table of Contents
1. [Razix Serialization Philosophy](#razix-serialization-philosophy)
2. [Definitions](#definitions)
3. [Assets and Pools Architecture](#assets-and-pools-architecture)
4. [Serialization Mechanics](#serialization-mechanics)
5. [Serialization Data Structures](#serialization-data-structures)
6. [Archive & Compression](#archive--compression)
7. [Asynchronous Load Mechanism](#asynchronous-load-mechanism)
8. [SceneGraph Format & Grammar](#scenegraph-format--grammar)
9. [Asset Database & Streaming Workflows](#asset-database--streaming-workflows)
10. [Naming & Validation](#naming--validation)
11. [Pak and Storage Data Structures](#pak-and-storage-data-structures)
12. [Implementation Milestones](#implementation-milestones)
13. [Design FAQs](#design-faqs)

---

## Razix Serialization Philosophy

In razix **"asset is everything"**. For the first draft, we only support binary file formats. Later we can optionally support JSON/S-expressions for asset debuggability or a binary editor tool in pyqt.

Traditional ECS is boring; anyways this started more of an idea while I was high one day. Now idk what this will lead to but I'll be explaining my intuition and final design goals. I don't know how good this design will be or if it can be better than others, but here we go:

_Unlike having separate disk-time assets and runtime entities, in this design **"everything is an asset"**. Runtime and disk time both call them assets. They basically have the same representation on disk vs runtime. Keep metadata and compressed data together + no components; assets reference other assets and we build a scene graphs of UUID refs that maps to linear pools of different asset types._

### Some Rules
- **Hierarchy**: SceneGraph maintains the relationship between different asset nodes.
- **Tagging**: How nodes are tagged is decided by `ISceneGraphRule`. 
- **Roots**: Only script and transform assets can exist as first-level nodes in the scene graph. 
- **Logic**: A mesh asset should be a child of a transform asset, and a material will be a child of a mesh asset. 
- **Storage**: All assets are stored in linear pools (MeshPool, TexturePool, MaterialPool, …).
- **Handles**: References use `rz_asset_handle` (header + payload index).
- **Locality**: Child assets are grouped together in "child slabs" to improve traversal locality.
- **Compression**: Textures & Meshes are compressed to BCn/custom formats; metadata stores this info.

### Misc
- Scene graph parsing code in **RAW ASM**! Only rules in C++. 
- Job system to load assets with placeholder defaults until ready.
- Models are broken/constructed by the scene graph inside Blender. 
- No House renderer until V.0.8.0.Dev.
- Multi-threaded asset loading via `RZthread`.

---

## Definitions

**SceneGraph** - Every zone has its own scene graph. Each Razix World/Level starts with many Zones as first-class nodes. The SceneGraph uses S-expressions to mimic a tree-like data structure.

**Zone** - Logical separation of assets and nodes in runtime. We separate nodes in the level scene graph per zone and load/stream the corresponding `*.pak` file.

**Pak** - Storage file for all assets in a zone. Supports two scopes: "zone-local" and "shared". Patching is handled via `_patched.pak` which takes precedence.

---

## Assets and Pools Architecture

- There is a central `RZAsset` pool to manage metadata.
- Actual asset data (payloads) are stored in separate, type-specific `RZAssetPools`.
- Each defined asset is bidirectionally linked with `RZAsset` via `rz_asset_handle`.
- This indirection facilitates management without enforcing inheritance or template-based designs.

### The 64-bit Handle
The `rz_asset_handle` is designed for efficient lookups:
- **Lower 32-bits**: Hot/Cold data index in the central asset pool.
- **Higher 32-bits**: Asset payload data index in the specific type pool.

Similar to `RZResourceManager`, the system splits data for cache efficiency. **The entirety of `RZAsset` fits in a single cache line** (with 8 bytes left in hot data for future use).

- **Hot Data**: High-frequency metadata (UUIDs, reference counts, flags).
- **Cold Data**: Sister pool for infrequent metadata, linked via handles to keep similar data together.

---

## Serialization Mechanics

`RZAssetDB` handles the heavy lifting, while `RZAssetPool` is only for runtime management. When `serialize/deserialize` is called on `RZAssetDB`, it picks up each pool and uses the `RZReflectionRegistry` to write them into `*.rzasset` files.

### The `*.rzasset` Format (Defined in `RZAssetSpec.h`)
- **[Header]**: Starts with `RZAsset` and its `RZAssetColdData` metadata. This can be hand-written or use reflection.
- **[Payload]**: The actual asset data (e.g., compressed mesh blob). `RZAssetDB` schedules relevant compression/decompression jobs based on the header info.

### Data Types Handling
We use the reflection registry to parse members and write them per the **ABI format**. Each data type is stored per the size requirements of the platform (assuming x64 ABI consistency).

- **Blobs/Strings**: Stored as a `size` followed by the data blob; type-casted as needed.
- **Primitives**: Written as-is (i32, u32, bool, bitsets, etc.).
- **Arrays**: Uses `RZSerializedArrayHeader` (type info, size, element count).
- **HashMaps**: Serialized as Arrays of keys/values/occupied flags.
- **Clean PODs**: Alignment is tracked and serialized as direct blobs.
- **Compression**: Handled per `SerializedBlob` rather than at the asset level.
- **Logic**: If a type consists only of primitives, it is "trivially copyable" and never compressed—just a direct `memcpy` on file read.

---

## Serialization Data Structures

As for iterating through members:
```cpp
u8* base = reinterpret_cast<u8*>(obj);
void* fieldPtr = base + m.offset;
```

I can then check the `TypeMetaData` serialization type and pass to a switch to fill these structs and write to disk:

```cpp
struct SerializedBlob {
    uint32_t offset;
    uint32_t size;
    uint32_t type_hash;
    uint8_t  compression;
    uint8_t  reserved[3];
    uint32_t decompressed_size;
};

// Array of anything
struct SerializedArray {
    SerializedBlob data;
    uint32_t element_count;
    uint32_t element_type_hash;
    uint8_t  element_size;
    uint8_t  reserved[3];
};

// HashMap
struct SerializedHashMap {
    SerializedBlob keys;
    SerializedBlob values;
    SerializedBlob occupied;
    uint32_t capacity;
    uint32_t count;
    uint32_t index;
};

// String
struct SerializedString {
    SerializedBlob data;
    uint32_t length;
    uint8_t  encoding; // UTF-8, UTF-16, ASCII
    uint8_t  reserved[3];
};

// Struct instance (Clean POD only)
struct SerializedObject {
    SerializedBlob data;
    uint32_t type_hash;
    uint32_t size;
};
```

---

## Archive & Compression

Razix uses a dual-archive approach. `RZCompressedArchive` uses deferred payloads unlike the `RZBinaryArchive`.

**File format:** `[RZFileHeader][HeaderSection][PayloadSection]`

```cpp
// Both archives must provide:
void write(const void* src, size_t size);
void read(void* dst, size_t size);
static constexpr bool kUsesOutOfLineBlobs;

// RZCompressedArchive additionally provides:
void registerBlob(size_t headerOffset, const void* payload, u32 payloadSize, rz_compression_type compression);
void finalize();
```

### Deferred Payloads
- Offsets are patched *after* compression.
- Walk reflection metadata and defer `SerializedBlobs`.
- `if constexpr (Archive::kUsesOutOfLineBlobs)` is used to switch mechanisms at compile time.

---

## Asynchronous Load Mechanism

1. **Discovery**: Once a scene graph is parsed into an `RZZone`, it identifies necessary assets.
2. **Placeholders**: The system installs a placeholder asset and registers for an `AssetLoaded` callback.
3. **Jobs**: Kicks off `RZAssetLoad/SaveAsyncJob`.
    - If compressed, it triggers `RZAssetCompress/DecompressAsyncJob`.
    - For Gfx resources, it passes data to the `RZResourceManager`.
4. **Resolution**: Once the job completes, the callback replaces the placeholder and updates the SceneGraph.

**TLDR;**
DiskIO jobs spawn child Decompress/PostProcess jobs onto worker threads. Only once the final `ASSET_LOADED` state is achieved does the SceneGraph see the new asset and build new data for the renderer.

```cpp
template<typename T>
rz_asset_handle requestAssetLoadFromDisk(RZUUID assetUUID)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);
    RZString assetName = requestAssetLoadFromDiskInternal(assetUUID, typeid(T));

    // Create a dummy asset with default placeholder
    rz_asset_handle handle = createAsset<T>(assetName);

    RZAsset* pAsset = getAssetHeaderMutable(handle);
    pAsset->addFlags(RZ_ASSET_FLAG_PLACEHOLDER | RZ_ASSET_FLAG_STREAMING);
    pAsset->removeFlags(RZ_ASSET_FLAG_DIRTY);

    return handle;
}
```

---

## SceneGraph Format & Grammar

SceneGraph uses **S-expressions** with a SIMD-accelerated C parser.

### `.scenegraph` Layout
- **Zone Directory**: Top-level coordinates and bounds.
- **Hierarchy**: Per-zone node tree.
- **Transform Pooling**: Rotation/Scale/Translation data is pooled per zone. Transform assets simply point to an index in this pool.

```lisp
(scene-file
  (header
    (version 1)
    (scene-bounds (min -1000 -1000 -1000) (max 1000 1000 1000)))
  
    (zone "zone_0_0_0" 
      (grid-pos 0 0 0)
      (world-bounds (min 0 0 0) (max 100 100 100))
      (offset 2048) (size 8192))
)

(zone_0_0
  (transforms
    (0 0 0 0 0 0 0 1 1 1 1))

  (transform world_root 0
    (transform player_spawn 1
      (mesh player_body uuid...)
      (material player_skin uuid...))))
```

### Scene Graph Grammar
```bnf
scene       := (scene VERSION bounds grid zone*)
bounds      := (bounds MIN_X MIN_Y MIN_Z MAX_X MAX_Y MAX_Z)
grid        := (grid DIM_X DIM_Y DIM_Z ZONE_SIZE)

zone        := (zone NAME GRID_X...MAX_Z transforms node*)
transforms  := (transforms xform* node*)
xform       := (INDEX)

node        := transform_node | asset_node

transform_node := (transform NAME XFORM_INDEX node*)
asset_node     := (TYPE NAME UUID node*)

TYPE        := mesh | material | light | camera | script | audio | fx
```

---

## Asset Database & Streaming Workflows

### Dev Build
- **Storage**: Loose `*.rzasset` files on disk.
- **Registry**: `assetdb.bin` stores the map of `UUID -> VFS Path`.
- **Action**: On Zone Activate, the system instantiates the scenegraph subtree and async-fills slots via the VFS paths.

### Shipping Build
- **Storage**: Zone-specific `*.pak` files.
- **Registry**: `UUID -> Pak` mapping.
- **Action**: On Zone Activate, the system opens the zone pak and async-loads chunks.
- **Streaming**: Granularity is at the **Zone** level.

---

## Naming & Validation

- **Naming**: `ASSET_TYPE_PREFIX_AssetName*.rzasset` (e.g., `MAT_Gold.rzasset`).
- **Validation (Dev)**: Verify UUIDs in `.scenegraph` exist in `assetdb.bin`.
- **Validation (Shipping)**: Cross-reference `RZPakFileEntry` against the SceneGraph.

---

## Pak and Storage Data Structures

```cpp
struct RZPakHeader {
    u32 magic;
    u32 version;
    u32 fileCount;
    u32 chunkCount;
    u64 fileTableOffset;
    u64 chunkTableOffset;
    u32 chunkSize;
};

struct RZPakFileEntry {
    UUID assetID;
    u32 chunkIndex;
    u32 offsetInChunk;
    u32 size;
};

struct RZPakChunkEntry {
    u64 fileOffset;
    u32 compressedSize;
    u32 uncompressedSize;
    u8 compression; // 0=None, 1=LZ4
};
```

---

## Implementation Milestones

### Phase 1
- AssetDB and Pool foundations.
- BinaryArchive serialization.
- `assetdb.bin` for development UUID mapping.
- S-expression parsing and Zone nodes.
- Async asset loading for `*.rzasset`.
- Basic Pak reader/writer APIs.

### Phase 2
- Debug tools for `*.rzasset` visualization.
- Zone streaming based on player position.
- Blender workflow integration.

---

## Design FAQs

**Q: How will someone know if the asset is READY?**
**A:** Use the handle to get the `RZAsset` pointer and subscribe to events. The SceneGraph uses these events to mark nodes dirty and trigger updates.

**Q: How to load all assets from disk? What is the workflow?**
**A:** As the scenegraph is parsed, we identify the atom type. This calls an opaque function that triggers the internal templated load, and we are good to go.

**Q: How will zones handle scene hierarchies?**
**A:** Each zone has its own independent scene hierarchy with its own root node. They are completely self-contained and can load/unload without affecting others.

**Q: What about cross-zone mission triggers?**
**A:** Use global triggers defined in the scene graph file. We handle these as a separate system outside the independent zone hierarchy.
