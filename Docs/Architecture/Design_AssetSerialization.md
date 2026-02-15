# [Draft-1] Asset System Design 

## Razix Serialization

In razix "asset is everything" for the first draft we only support binary file format. Later we can optionally support JSON/S-expressions for asset debuggability or a binary editor tool in pyqt.

Traditional ECS is boring, anyways this started more of an idea while I was high one day. Now idk what this will lead to but I'll be explaining my intuition and final design goals, idk how good will this design be and if it can be better than others. Here we go:

_Unlike having separate disk-time asset and runtime entity, in this design "everything in an asset". Runtime and disk time both call them assets. They are basically having same representation on disk vs runtime. Keep metadata and compressed data together + no components, assets reference other assets and we build a scene graphs of UUID refs that maps to linear pools of different asset types._

### Some rules
- SceneGraph maintains the hierarchy relationship between different asset nodes. 
- Now how nodes can be tagged is decided by a ISceneGraphRule. 
- Only script and transform assets can exist as first level nodes in the scene graph. 
- You get the idea, a mesh asset should be child of transform asset, and material will be child of mesh asset. 
- All ref asset are child nodes. 
- All assets stored in linear pools (MeshPool, TexturePool, MaterialPool, …).
- Uses a freelist internally to track what is free, no deferred references unlike Gfx resource so no need to track generation as we cannot have stale assets.
- References use rz_asset_handle (hader _ payload inedx)
- Child assets grouped together in child slabs to improve traversal locality.
- Assets with hierarchy level are grouped together in memory, maybe use a RB tree with memory addressed cached? not sure will be thought out during implementation.
- Textures & Meshes compressed to BCn/custom format --> metadata will store this info

### Misc
- scene graph parsing code in RAW ASM! only rules in C++. 
- Job system to load assets, placeholder defaults until then, CBs to refresh loads.
- Models are broken/contructed by scene graph inside blender. 
- no House renderer until V.0.8.0.Dev
- write a doc for blender workflow.
- RZthread for multi-threaded asset loading.


### Now how will this work?

RZAssetDB will take care of this, RZAssetPool is only for runtime management. Once we call serialize/deserialize on RZAssetDB. It picks up each of the pool and then uses the RZReflectionRegistry to write them into the *.rzasset files.

*.rzasset file format --> defined in RZAssetSpec.h
- **[Header]** starts with RZAsset and it's RZAssetColdData metadata ==> Header/Metadata serialization, this can be hand-written functions without reflection or use it idc. Buy basically that.
- **[Payload]** Next we have the asset payload, this is the actual important thing, this can be compressed blob, the header will tell us that and RZAssetDB can call schedule relevant compression/decompression Job on it.

Use the RZReflectionRegistry to parse through the members and write them as the structs need them, we do it as per the ABI format. Each data type is stored as per the datatype size of the platforms. Hopefull all x64 has the same ABI requirements.

Data Types handling:
- Blobs: As for pointer/blob data we use a blob_size to load a blob of pointer data and can be type-casted as needed. 
- Strings: again it's just like a pointer, we store the str_size and the blob of data, basically behaves like pointer blobs
- Floats/Doubles: copy 4/8 bytes
- Primitives: written as-is for i32/u32/char/unsigned char/bool/bitset/enums flags
- Arrays: uses a RZSerializedArrayHeader that has type info, size and element count again similar to Blobs, everything is a blob if you look closely
- HashMaps: Serialize keys/value/occupied arrays as Arrays 
- Clean POD structs: needs to track alignment and we can drirectly write the serialized Blobs
- Compression is handles per SerialzedBlob instead of at asset level
- for trivially capable types we just memcpy
- for more complex types we define macros walk through TypeMetaData members and serialize them one by one
- We can use heuristics to choose to compress based on Blob payloads
- If the type has all primitive datatypes they are trivially copyable, and however big never compressed just memcpy on filer read.
- Use custom macros for each reflection type ex. REFLECT_PRIMITIVE, REFLECT_STRUCT, REFLECT_STRING etc. to handle metadata generation and make intent more explicit and this also helps with easier extension in future

As for iterating through members:
```
u8* base = reinterpret_cast<u8*>(obj);
void* fieldPtr = base + m.offset;
```
I can then check the TypeMetaData serialization type and pass to a switch and fill these SerializedXXX structs and write to disk

```
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
    SerializedBlob data;                    // blob of all elements
    uint32_t element_count;
    uint32_t element_type_hash;
    uint8_t  element_size;
    uint8_t  reserved[3];
};

// HashMap
struct SerializedHashMap {
    SerializedBlob keys;                    // blob of all keys
    SerializedBlob values;                  // blob of all values
    SerializedBlob occupied;                // blob of all occupancy flags
    uint32_t capacity;
    uint32_t count;
    uint32_t index;
};

// String
struct SerializedString {
    SerializedBlob data;                    // blob of characters
    uint32_t length;
    uint8_t  encoding;                      // UTF-8, UTF-16, ASCII
    uint8_t  reserved[3];
};

// Struct instance - only works for POD/simple structs without pointers. Marked as clean POD in reflection registry.
struct SerializedObject {
    SerializedBlob data;                    // blob of struct bytes
    uint32_t type_hash;
    uint32_t size;
};

// Array of objects - only works for POD/simple structs without pointers. Marked as clean POD in reflection registry.
struct SerializedObjectArray {
    SerializedBlob data;                    // blob containing array of blobs
    uint32_t element_count;
    uint32_t element_type_hash;
};

// Vector<T>
template<typename T>
struct SerializedVector {
    SerializedBlob data;                    // blob of elements
    uint32_t count;
    uint32_t capacity;
};

// Map/HashMap<K, V>
template<typename K, typename V>
struct SerializedMap {
    SerializedBlob keys;                    // blob of keys
    SerializedBlob values;                  // blob of values
    uint32_t count;
};
```

## Compression
File format: RZCompressedArchive uses deferred payloads unlike the RZBinaryArchive
```
[RZFileHeader][HeaderSection][PayloadSection]
```


```C++
// Changes to RZSerializable
template<typename Derived, typename Archive>
class RZSerializable;
```

Both archives must provide:

```C++
void write(const void* src, size_t size);
void read(void* dst, size_t size);

static constexpr bool kUsesOutOfLineBlobs;
```

Only `RZCompressedArchive` additionally provides:
```C++

void registerBlob(size_t headerOffset,
                  const void* payload,
                  u32 payloadSize,
                  rz_compression_type compression);

void finalize();
```

Serializer code branches with `if constexpr (Archive::kUsesOutOfLineBlobs)`.

```C++
struct RZPendingBlob {
    size_t              headerOffset;   // where the RZSerializedBlob lives in HeaderSection
    const void*         payload;        // original data pointer
    u32                 payloadSize;    // uncompressed size
    rz_compression_type compression;
};
```
- payloads are deferred
- offsets are patched after compression
- Walk reflection metadata and defer SerializedBlobs
- compress/decompress blobs and patch the header using headerOffset once compression is done
- use if constexpr to switch b/w different Arhive mechanisms, ofc still use custom read/write into archive.


## Async Load mechanism
- Once a scene graph is loaded or parsed into a RZZone --> it knows what assets to load 
--> scene graph --> load Zone --> load Mesh/Material (install a placeholder asset and register for AssetLoaded event callback to update the scenegraph) --> push to worker jobs --> pre-defied asset load jobs
- kicks off RZAssetLoad/SaveAsyncJob --> reads file and check if needs to be compressed/decompressed --> kicks off RZAssetCompress/DecompressAsyncJob --> update the final asset and invoke the callback --> replace the placeholder asset
if it's a mesh/texture we pass it off to the RZResourceManager from withing in the RZGfxResLoadJob invoked by RZAssetCompress/DecompressAsyncJob.
- Since scene graph is converted to RZWorld and passed of to renderer with properly handles, Gfx doesn't care how it's done we need to replace it with valid handles that's it. simple.
- user calls createAsset<T> or destroyAsset<T> to create new asset the inturn calls allocate/release asset and registers the UUID into registry map and updates the assetName
- but things like scenegraph can use the assetName to to load in *.rzasset files (how de we resolve name IDK yet, maybe store node name in sexpression files) 
- They call requestAssetLoadFromDisk/requestAssetLoadFromPak APIs and provide a eventlistern to register with RZAsset (scenegraph and use this to mark nodes dirty for update later)
- So once I get a handle with default payload I can have SceneGraph subscribe on the RZAsset header
- This sill return a default asset payload (from the curated list), now every one of these calls is templated so idk how load works properly
- Also save all to disk doesn't make sense, I can have SceneGraphe export one by one as I save at end when the scenegraph is about to go out of bussiness and we call save on it.

Async Jobs: DiskIO --> Compression --> PostProcess --> GPU Upload (optional for GfxResoures) --> Callback to replace placeholder in SceneGraph

**TLDR;**
So we first kick of a bunch of DiskIO jobs once any on of the DiskIO job is done it'll add more Decompress jobs and once that is done if it has a post process it'll add them to worker threads and finally we can GPU upload jobs
Basically master DiskIO jobs can spawn child jobs onto the worker threads to steal. Only once the final ASSET_LOADED state is achieved and callback is invoked scenegraph sees this new asset and build new data for the renderer in the subsequent frame.

**So each serialized type is inline header and it's payload on how to save it's SerializedBlob.**

```C++
bool saveAssetToDisk(rz_asset_handle handle) const;
        template<typename T>
        rz_asset_handle requestAssetLoadFromDisk(RZUUID assetUUID)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            RZString assetName = requestAssetLoadFromDiskInternal(assetUUID, typeid(T));
            // TODO: update name, asset load state and asset type
            // TODO:!!! Create a default asset per asset type to return here while the actual asset is being loaded async, so that the game can start
            // Create a dummy asset with default places holder
            rz_asset_handle handle = createAsset<T>(assetName);

            // Set in loading state
            RZAsset* pAsset = getAssetHeaderMutable(handle);
            pAsset->addFlags(RZ_ASSET_FLAG_PLACEHOLDER);
            pAsset->addFlags(RZ_ASSET_FLAG_STREAMING);
            // No longer dirty once we make it a placeholder
            pAsset->removeFlags(RZ_ASSET_FLAG_DIRTY);

            return handle;
        }
        RZString requestAssetLoadFromDiskInternal(RZUUID assetUUID, std::type_index typeIdx);
```
### Async FAQ

#### Q: How will someone know if the asset is READY?
**A:** If you want a callback when an asset is ready, you can use the handle to get pointers to `RZAsset` and install your own events on whoever needs to know.
The SceneGraph can subscribe to the events on `pRZAsset` and use that to mark Scene Nodes dirty and update things as needed.

#### Q: How to load all assets from disk? What is the workflow?
As we parse the scenegraph, when we get a node and parse its atom, we know the type. It will call an opaque function that calls the templated function inside, and we are good to go.

#### Q: How to register a callback for asset events?
Whoever created the asset can use the handle to get the pointer and get `RZAsset`, then subscribe for events on that. `AssetDB` or other systems will dispatch events on `RZAssets`.

## Definitions

**SceneGraph** - Every zone has it's own scene graph, each Razix World/Level start of with many Zones as first class nodes in the SceneGraph and a SceneGraph is made up using S-expressions to mimic a tree like data structure. 

**Zone** - logical seperation of assets and nodes in runtime, we seperate nodes in the level scene graph per zone and load/stream the *.pak file containing the assets for that zone.

**Pak** - All the assets for that zone are stored in this file, we also have 2 types of asset scoping per zone and shared, we can either store them as zone local or in a shared.pak file, also to patch things _patched.pak will take precedence and replace the older paks.

## Naming Conventions

SceneGraphss - SCENENAME.scenegraph
Zones inside SceneGraph - Zone_X_Y (X, Y) is the 2D coords in the world
Zone Pak - SCENENAME_Zone_Z_Y.pak

[Development Builds]
assetdb.bin - stores the entire map of UUID --> ASSET_PATH references, ASSET_PATH is just plain string relative to //Asset, these are the VFS paths, that can be easily edited.

ASSET_TYPE_PREFIX_AssetName*.rzasset - name of every asset file start with asset type prefix.
ex. [MAT, TEX, AUDIO, FX, MESH, etc.], we can define the comprehensive list later here for validation.

## Assets and Pools

- There is a central RZAsset pool to manage metadata.
- Actual asset data (payloads) are stored in separate, type-specific RZAssetPools which manage the memory and lifecycle of the assets.
- The handle can easily refer to these pools on a need-to-basis.
-  Each defined asset is bidirectionally linked with RZAsset via rz_asset_handle.
- This indirection facilitates easy management of assets and their data without enforcing inheritance or template-based designs.

The rz_asset_handle is a 64-bit value designed for efficient lookups:
- Lower 32-bits: Represents the hot/cold data index in the central asset pool.
- Higher 32-bits: Represents the asset payload data index in the specific asset pool for the actual asset data.

Similar to how RZResourceManager handles resources, the Asset System splits data for cache efficiency. **The entirely of RZAsset fits in a single cache line** (with still 8 bytes left in hot data for future use).

We maintain a central RZAsset pool specifically to manage high-frequency metadata (UUIDs, reference counts, flags).
Each actual asset starts with define that is a rz_asset_handle. So each asset type has it's own pool and same goes for RZAsset as well, link data via handles but keep similar data together. This is the base for this data-oriented design.

## Asset Database and Streaming

**Engine Startup**
- Load storage backend (registry or Shared.pak)
- Load Level.graph
- Extract zones and build Zone list

RZAssetDB has 2 main jobs
1. Asset Serialization:
    - In Development builds build the assetdb.bin file just a hashmap of UUID and VFS asset file path string, it writes the *.rzasset files into respective folders as-usual.
    - Engine has a folder called Asset inside game folder to load *.rzasset from using relative VFS paths, as for raw assets we can have Raw in the same directory as Assets.
    - In Shipping builds use the PakRead/Write APIs to write *.rzasset files and Pak API will take care of compressing them or streaming etc., we just launch Jobs and use APIs here.
2. Async Asset loading
    - In Development builds, it checks the assetdb.bin loaded hashmap in runtime and async loads it by issuing a Job
    - In Shipping builds, it checks the UUID <--> Pak hashmap instead to load that pak into memory by issuing a stream and decompress job.
    - These hashmaps are owned by RZAssetDB for now, current zone or zones to load can be done by the poking into the SceneGraph and checking. 
- Different scenegraphs are loaded by the scene manager I suppose not really important now.

### Dev Build

- Instantiate scenegraph is done first before loading assets.

- Load assetdb.bin
- Zone Activate:
    - Instantiate scenegraph subtree
    - For each UUID -> load loose asset -> Async fills slots
- Zone Deactivate:
    - Release asset references

### Shipping Build

- Open Shared.pak
- We load 
- Register UUID map
- Zone Activate:
    - Open zone pak
    - Register UUID -> Pak mapping -> Async load all chunks (initial version)

- Zone Deactivate:
    - Free chunks
    - Close pak --> Unregister UUIDs

**Streaming granularity = Zone** (no streaming in Development builds)

## SceneGraph format

Scenegraph in razix will use **S-expressions** with SIMD accelerated c-parser library(compiled into core for now, maybe seperate library in future or move it to Tools if blender needs it) from engine. 

[Source]: https://en.wikipedia.org/wiki/S-expression

.scenegraph file format:
- Zone directory at the top
- Per-Zone Scenegraph hierarchy of nodes
- Lazy zone loading with mmap 
- SIMD S-expression parsing
- Transform pooling per zone (Num of transforms, rotations, scales, world matrices etc. or just matrices that we can decompose)
    - each zone has a bunch of data about scene node transforms, that are loaded into the transform pool
    - Each transform asset actually has just index
    - Transform assets are packed into scene files without compression but we pack them IDK why tho. convoluted this.
Each Zone directory has the zone name, idx, bounds, world position
Each Zone has the bounds, scene hierarchy for that zone, transforms pools for that zone.

```
(scene-file
  (header
    (version 1)
    (scene-bounds (min -1000 -1000 -1000) (max 1000 1000 1000))
    (zone-grid-dims 20 20 20)
    (zone-physical-size 100))
  
    (zone "zone_0_0_0" 
      (grid-pos 0 0 0)                           ; Grid coordinates
      (world-bounds (min 0 0 0) (max 100 100 100))
      (offset 2048)                              ; File offset
      (size 8192))                               ; Zone data size
    
    (zone "zone_0_0_1"
      (grid-pos 0 0 1)
      (world-bounds (min 0 0 100) (max 100 100 200))
      (offset 10240)
      (size 7456))
    
    (zone "zone_1_0_0"
      (grid-pos 1 0 0)
      (world-bounds (min 100 0 0) (max 200 100 100))
      (offset 17696)
      (size 9120))
    
    ... more entries ...
  )
  
  ; Everything below this point is zone data
  ; Never parsed unless zone is needed! 
  
  @zone_0_0_0_data
  (zone "zone_0_0_0"
    (transforms ...)
    (nodes ...))
  
  @zone_0_0_1_data
  (zone "zone_0_0_1"
    (transforms ...)
    (nodes ...))
  
  ...  rest of zones ...)
```

Design FAQs?
Q. how will zone handle scene hierarchies?does each zone have a root node? or can we have scene hierarchies part of the zone? which is better?
==> Each zone has its own independent scene hierarchy with its own root node. Each zone is completely self-contained and can load/unload without affecting other zones.

```
(zone "zone_0_0_0"
  (bounds (min 0 0 0) (max 100 100 100))
  (grid-pos 0 0 0)
  (transforms ...)
  
  (root-node "zone_0_0_0_root"      ; ← Zone's own root
    (transform-id 0)
    (children
      (node "building_1" (transform-id 1) (children ...))
      (node "light_1" (transform-id 2) (component "Light"))
      (node "enemy_group" (transform-id 3) (children ...)))))

(zone "zone_0_0_1"
  (bounds (min 0 0 100) (max 100 100 200))
  (grid-pos 0 0 1)
  (transforms ...)
  
  (root-node "zone_0_0_1_root"      ; ← Different root
    (transform-id 0)
    (children
      (node "forest_trees" (transform-id 1) (children ...))
      (node "creatures" (transform-id 2) (children ...)))))
```

Q. What about cross-zone mission triggers?
==> Use global triggers for this create a new system in scene graph file and handle it. As of now let's see. We will try to develop this with the constrains of independent zones.

```
(global-triggers
    (trigger "enter_forest"
      (bounds (min 90 0 90) (max 110 100 110))
      (event "mission_forest_ambush"))
    (trigger "reach_city"
      (bounds (min 180 0 -20) (max 220 100 20))
      (event "mission_city_gates")))
```

## Zones
Each level loads a bunch of zones, mainly the central zone and some neightbouring zones, as for streaming we can load the entire pak file and load the assets into memory.

## Compression per zone/pak

### Development builds
- Each dev build stores lose *.rzasset files on disk. 
- We will have a assetdb.bin to mape UUID --> Asset file paths, we can store as we like. But we will maintain storing meshes/textures/audio etc. together for sanity.

## Shipping builds
- Each *.rzasset is written into 1/4/256 MB chunks (will decide after profiling) and then compressed and multiple chunks will be written into a *.pak file **per ZONE**
- We can stream the corresponding zone and decompress the chunks per zone pak into memory. Granulity is suubject to profiling.
- Each chunk independently decompressible
- No per-asset compression
- Compression only at chunk level
- Offsets relative to decompressed buffer

## Asset Validation 

### [Development Builds]
- We can verify the asset UUIDs (16-byte data) in a *.scenegraph file with the UUIDs in the assetdb.bin file to make sure they exist. 
- Stale resources can be removed or deleted or proned or replaces with default assets.

### [Shipping Builds]
- We can verify each of the PAK file and read it's RZPakFileEntry and check that UUID against the *.scenegraph file.

We can do this in the RazixAssetManager tool, where we can validate them and also peek at the pak files and loose *.rzasset files via loading the assetdb.bin file.

Zone are merely a virtual concept no concrete seperation in files exist, except for the *.scenegraph file.

# Pak and some data structures

```C++
struct RZZone
{
    char name[64];
    AABB bounds;

    RZPak* pak;          // nullptr in dev
    SceneGraph graph;

    ZoneState state;     // Unloaded, Loading, Loaded
};
```
Pak Disk Layout
```
[ PakHeader ]
[ FileEntry[] ]
[ ChunkEntry[] ]
[ ChunkData... ]
```

```C++
struct RZPakHeader
{
    u32 magic;
    u32 version;
    u32 fileCount;
    u32 chunkCount;
    u64 fileTableOffset;
    u64 chunkTableOffset;
    u32 chunkSize;
    u32 flags;
};

struct RZPakFileEntry
{
    UUID assetID;
    u32 chunkIndex;
    u32 offsetInChunk;
    u32 size;
};

struct RZPakChunkEntry
{
    u64 fileOffset;
    u32 compressedSize;
    u32 uncompressedSize;
    u8 compression; // 0=None, 1=LZ4
};
```

## Implementation Milestones

Phase - 1
0. AssetDB and Pool and Asset types etc.
1. BinaryArhive serialization
2. RZAssetDB --> assetdb.bin for development builds to map UUIDs to *.rzasset --> simple hashmap of 16byte UUIDs mapped to VFS relative paths from //Assets
3. S-expression based SceneGraph parsing and build nodes
    0. SceneManager to load and store scene, probably owned by the application? or a singleton? Simple Class in 
    1. Zone as first class nodes
    2. SIMD accelerated tokenization for s-expressions
    3. load basic tree nodes fom s-expression tokenization results etc.
    4. ISceneGraphRules for nodes tagging and validation
    5. SceneGraph stores the active zone
    6. Updating scenegraph transforms etc. and uupdates/querying all that mumbo jumbo
    7. SceneGraph file per zone transforms shit
4. Async asset loading for *.rzasset, and update stuff in the scenegraph node.
5. RZPak reader and write API + #define to tweak chunking size frequency
    1. basic pak write/read API, add chunks and create paylaods after memory frequcny
    2. Compress and write to file and decompress and readback into RZAssetPools using asset types
6. Blender Integration

Phase - 2
7. Debug tool to vis *.rzasset and validation:
    - scenegraph via comparing with assedtb.bin and for pak files header peaking and use the UUIDs to match against scenegraph files.
8. Zones streaming via RZAssetDB --> calling in RZAssetStreaming APIs and using the scenegraph to predict next active zones to load etc. using player positions
9. Hook up RZPak with Async asset loading

