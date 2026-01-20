# [Draft-1] Serialization

In razix "asset is everything" for the first draft we only support binary file format. Later we can optionally support JSON/S-expressions for asset debuggability or a binary editor tool in pyqt.

Now how will this work?

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
- Use std::visit and overloadTs to customize serialization logic (or use a simple switch case?)
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

## Async Load mechanism
- Once a scene graph is loaded or parsed into a RZZone --> it knows what assets to load 
--> scene graph --> load Zone --> load Mesh/Material (install a placeholder asset and register for AssetLoaded event callback to update the scenegraph) --> push to worker jobs --> pre-defied asset load jobs

- kicks off RZAssetLoad/SaveAsyncJob --> reads file and check if needs to be compressed/decompressed --> kicks off RZAssetCompress/DecompressAsyncJob --> update the final asset and invoke the callback --> replace the placeholder asset
if it's a mesh/texture we pass it off to the RZResourceManager from withing in the RZGfxResLoadJob invoked by RZAssetCompress/DecompressAsyncJob.

- Since scene graph is converted to RZWorld and passed of to renderer with properly handles, Gfx doesn't care how it's done we need to replace it with valid handles that's it. simple.

Async Jobs: DiskIO --> Compression --> PostProcess --> GPU Upload (optional for GfxResoures) --> Callback to replace placeholder in SceneGraph

**TLDR;**
So we first kick of a bunch of DiskIO jobs once any on of the DiskIO job is done it'll add more Decompress jobs and once that is done if it has a post process it'll add them to worker threads and finally we can GPU upload jobs
Basically master DiskIO jobs can spawn child jobs onto the worker threads to steal. Only once the final ASSET_LOADED state is achieved and callback is invoked scenegraph sees this new asset and build new data for the renderer in the subsequent frame.

# Bonus content - SceneGraph format

Scenegraph in razix will use **S-expressions** with SIMD accelerated c-parser library(compiled into core for now, maybe seperate library in future or move it to Tools if blender needs it) from engine. 

[Source]: https://en.wikipedia.org/wiki/S-expression

.scenegraph file format:
- Zone directory at the top
- Lazy zone loading with mmap 
- SIMD S-expression parsing
- Transform pooling per zone

Each Zone directory has the zone name, idx, bounds, world position
Each Zone has the bounds, scene hierarchy for that zone, transforms pools for that zone.

```
(scene-file
  (header
    (version 1)
    (scene-bounds (min -1000 -1000 -1000) (max 1000 1000 1000))
    (zone-grid-dims 20 20 20)
    (zone-physical-size 100))
  
  (zone-directory
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