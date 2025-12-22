#pragma once

#include "Razix/Core/RZDepartments.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/string.h"

#include "Razix/Core/RZThreadCore.h"

#include "Razix/Core/UUID/RZUUID.h"

#include "Razix/Core/Utils/RZDateUtils.h"

#include "Razix/Events/RZEvent.h"

#define RAZIX_ASSET_INVALID_HANDLE 0xFFFFFFFFFFFFFFFFULL
#define RAZIX_ASSET_HOTDATA_MASK   0x00000000FFFFFFFFULL
#define RAZIX_ASSET_COLDDATA_MASK  0xFFFFFFFF00000000ULL

#ifdef RAZIX_DEBUG
    #define RAZIX_ASSET_PAYLOAD_HEADER \
        rz_asset_handle handle;        \
        u32             _debugMagic = 0xA55E7; /* "ASSET" in leet-speak */
#else
    #define RAZIX_ASSET_PAYLOAD_HEADER \
        rz_asset_handle handle;
#endif

#define RAZIX_ASSET RAZIX_ASSET_PAYLOAD_HEADER

typedef u64 rz_asset_handle;    // higher 32 bits for asset playload data index and lower 32 bits for hot/cold data index into the asset pools

namespace Razix {

    // trick to dynamically generate asset type enum

#define ASSET_TYPE_LIST \
    X(Transform)        \
    X(Camera)           \
    X(Light)            \
    X(Material)         \
    X(PhysicsMaterial)  \
    X(Mesh)             \
    X(Texture)          \
    X(Animation)        \
    X(Audio)            \
    X(LuaScript)        \
    X(AssetRef)         \
    X(VignerePuzzle)    \
    X(LovePuzzle)       \
    X(Cloth)            \
    X(GameData)

    enum class RZAssetType
    {
#define X(name) k##name,
        ASSET_TYPE_LIST
#undef X
            COUNT
    };

    static_assert((u32) RZAssetType::COUNT == 15, "More asset types have been added, make changes to apt places!");

    /**
     * Defined how the assets are stored in the database pools
     * MemoryBacked types are allocated memory by the RZAssetBuilder and stores there 
     * GPUBacked doesn't create any memory but provide with proxy views for lazy allocation using RZResourceManager
     * NonMemoryBacked resources are run-time assets in the engine instantiated by scripting etc. and are non-persistent
     */
    enum class RZAssetStorageType
    {
        kMemoryBacked,
        kGPUBacked,
        kRuntimeGenBacked,
        kNonMemoryBacked,
        kLazyMemoryBacked,
        kSceneGraphBacked,
        COUNT
    };

    RAZIX_ALIGN_TO(MEM_DEF_ALIGNMENT_16)
    typedef struct RZAssetVersion
    {
        RZUUID revisionID = {};    // unique ID for the asset revision
        u64    major      = 0;     // major version
        u64    _pad0      = 0;
    } RZAssetVersion;

    typedef enum RZAssetFlags : u8
    {
        RZ_ASSET_FLAG_NONE        = 0,
        RZ_ASSET_FLAG_READY       = 0,
        RZ_ASSET_FLAG_COMPRESSED  = 1 << 0,
        RZ_ASSET_FLAG_ENCRYPTED   = 1 << 1,
        RZ_ASSET_FLAG_READONLY    = 1 << 2,
        RZ_ASSET_FLAG_DIRTY       = 1 << 3,
        RZ_ASSET_FLAG_STREAMING   = 1 << 5,
        RZ_ASSET_FLAG_PLACEHOLDER = 1 << 6,
        RZ_ASSET_FLAG_COUNT       = 7,
    } RZAssetFlags;
    static_assert(RZ_ASSET_FLAG_COUNT <= 8, "Asset flags exceeded the u8 limit!");

    struct RAZIX_ALIGN_TO(MEM_DEF_ALIGNMENT_16) RZAssetMetadata
    {
        RZString         name;
        RZString         author;
        RZString         description;
        RZString         commitHash;    // git commit hash or perforce CL
        RZAssetVersion   version;
        RZPackedDateTime packlastModified;
        RZPackedDate     createdDate;
        Department       department;
        u8               _pad0[13];
    };

    struct RAZIX_ALIGN_TO(MEM_DEF_ALIGNMENT_16) RZAssetDependecy
    {
        RZUUID      assetID;
        RZAssetType type;
        u8          _pad0[12];
    };

    // RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE)
    // We could do this or instead keep this to 48 bytes and load the vtable pointer and cold data pointer together with this in a single cache line
    // This way we can avoid multiple cache line loads for accessing cold/vptr pointers, and still keep hot data active, ofc we will have very less hot data space left, 15 bytes for future use
    struct RAZIX_ALIGN_TO(8) RZAssetHotData
    {
        RZUUID             UUID;
        rz_asset_handle    handle;
        rz_atomic_u64      referenceCount;
        RZAssetType        type;
        RZAssetStorageType storagePreference;
        RZAssetFlags       flags;
        u8                 _pad0[15];    // remaining bytes for future use
    };
    static_assert(sizeof(RZAssetHotData) == 64 - RAZIX_COLD_DATA_PTR_SIZE_BYTES, "Hot data must be less than 64 bytes - RAZIX_COLD_DATA_PTR_SIZE_BYTES (8 bytes) ");

    struct RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE) RZAssetColdData
    {
        RZDynamicArray<RZAssetDependecy> dependencies;    // TODO: can we cap this and use a RZFixedArray? we will know once Tanu is ready
        Razix::RZEventDispatcher         eventDispatcher;
        RZAssetMetadata                  metadata;
        u8                               _pad0[48];
        rz_critical_section              CS; // forces cache line alignment
    };

    /**
     * RZAsset is the base class for the all assets in the engine
     * Razix follows the principle "Everything in an Asset"
     * This class helps in managing the asset metadata, events and reference counting
     * Each asset defined is bidirectionally linked with RZAsset via rz_asset_handle
     * Each asset maps to a RZAssetPool which manages the memory and lifecycle of the assets
     * The rz_asset_handle is a 64-bit value where the lower 32-bits represent the hot/cold data index in the asset pool
     * and the higher 32-bits represent the asset payload data index in the asset pool for the actual asset data
     * This indirection helps in easy management of assets and their data without enforcing inheritance or template-based designs
     * Handles to link and reference is a lot easier and efficient than pointers especially when dealing with asset databases and pools 
     * and is very data-oriented design friendly
     * 
     * We have a central RZAsset pool to manage the metadata etc. and assets and their data are stored in separate pools, the handle can easily refer on need to basis
     * 
     * Hot data will be loaded along with the asset handle in the asset pools for faster access
     * Cold data will be allocated separately and will be accessed via pointer indirection, in a corresponding sister pool
     * Hot and cold data share the same handle index for easy access
     * Similar to how RZResourceManager handles resources and their pCold data
     * 
     * TODO: If we find any cold data being accessed frequently, move them to hot data for faster access after complete design of Asset System
     */
    class RAZIX_ALIGN_TO(RAZIX_CACHE_LINE_SIZE) RZAsset final    // no one can derive from it
    {
    public:
        /* User cannot do this by their own */
        RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZAsset);
        RAZIX_DELETE_PUBLIC_CONSTRUCTOR(RZAsset);
        ~RZAsset() = delete;

        RZAsset(RZAssetType type, void* pColdDataMemory);

        // Destroys the asset and frees cold data, however hot data will be freed by the asset pools, cold data asset pool can use this util function
        void destroy();

        // Asset Events are defined in the central RZEvent.h file under EventType enum
        // Event management (listeners for asset changes)
        // Add an event listener for a specific event type
        template<typename EventT, typename EventCbFn>
        void addEventListener(EventCbFn&& callback)
        {
            m_pCold->eventDispatcher.registerCallback<EventT>(std::forward<EventCbFn>(callback));
        }

        // Notify listeners of a specific event
        inline void notifyListeners(RZEvent& event) { m_pCold->eventDispatcher.dispatch(event); }

        inline const RZUUID&      getUUID() const { return m_Hot.UUID; }
        inline void               setUUID(const RZUUID& uuid) { m_Hot.UUID = uuid; }
        inline RZAssetType        getType() const { return m_Hot.type; }
        inline void               setType(RZAssetType type) { m_Hot.type = type; }
        inline RZAssetStorageType getStoragePreference() const { return m_Hot.storagePreference; }
        inline void               setStoragePreference(RZAssetStorageType pref) { m_Hot.storagePreference = pref; }
        inline void               addReference() { rz_atomic64_increment(&m_Hot.referenceCount, RZ_MEMORY_ORDER_RELAXED); }
        inline void               removeReference() { rz_atomic64_decrement(&m_Hot.referenceCount, RZ_MEMORY_ORDER_RELAXED); }
        inline u64                getReferenceCount() { return rz_atomic64_load(&m_Hot.referenceCount, RZ_MEMORY_ORDER_RELAXED); }
        inline void               setReferenceCount(u64 count) { rz_atomic64_store(&m_Hot.referenceCount, count, RZ_MEMORY_ORDER_RELAXED); }
        inline RZAssetFlags       getFlags() const { return m_Hot.flags; }
        inline void               setFlags(RZAssetFlags flags) { m_Hot.flags = flags; }
        inline void               addFlags(RZAssetFlags flags) { m_Hot.flags = static_cast<RZAssetFlags>(m_Hot.flags | flags); }
        inline void               removeFlags(RZAssetFlags flags) { m_Hot.flags = static_cast<RZAssetFlags>(m_Hot.flags & ~flags); }
        inline bool               hasFlag(RZAssetFlags flag) const { return (m_Hot.flags & flag) != 0; }
        inline bool               hasAllFlags(RZAssetFlags flags) const { return (m_Hot.flags & flags) == flags; }
        inline bool               hasAnyFlag(RZAssetFlags flags) const { return (m_Hot.flags & flags) != 0; }
        inline bool               isCompressed() const { return hasFlag(RZ_ASSET_FLAG_COMPRESSED); }
        inline bool               isEncrypted() const { return hasFlag(RZ_ASSET_FLAG_ENCRYPTED); }
        inline bool               isReadOnly() const { return hasFlag(RZ_ASSET_FLAG_READONLY); }
        inline void               setCompressed(bool value) { value ? addFlags(RZ_ASSET_FLAG_COMPRESSED) : removeFlags(RZ_ASSET_FLAG_COMPRESSED); }
        inline void               setEncrypted(bool value) { value ? addFlags(RZ_ASSET_FLAG_ENCRYPTED) : removeFlags(RZ_ASSET_FLAG_ENCRYPTED); }
        inline void               setReadOnly(bool value) { value ? addFlags(RZ_ASSET_FLAG_READONLY) : removeFlags(RZ_ASSET_FLAG_READONLY); }
        inline bool               isReady() const { return hasFlag(RZ_ASSET_FLAG_READY); }
        inline void               setReady(bool value) { value ? addFlags(RZ_ASSET_FLAG_READY) : removeFlags(RZ_ASSET_FLAG_READY); }
        inline bool               isDirty() const { return hasFlag(RZ_ASSET_FLAG_DIRTY); }
        inline void               markDirty() { addFlags(RZ_ASSET_FLAG_DIRTY); }
        inline void               clearDirty() { removeFlags(RZ_ASSET_FLAG_DIRTY); }

        inline const RZAssetMetadata& getMetadata() const
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            return m_pCold->metadata;
        }

        inline RZAssetMetadata& getMetadata()
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            return m_pCold->metadata;
        }

        inline void setMetadata(const RZAssetMetadata& metadata)
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            m_pCold->metadata = metadata;
        }

        inline const RZDynamicArray<RZAssetDependecy>& getDependencies() const
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            return m_pCold->dependencies;
        }

        inline void addDependency(RZAssetType assetType, const RZUUID& assetID)
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            RZAssetDependecy dep = {};
            dep.type             = assetType;
            dep.assetID          = assetID;
            m_pCold->dependencies.push_back(dep);
        }

        inline void clearDependencies()
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            m_pCold->dependencies.clear();
        }

        inline size_t getDependencyCount() const
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            return m_pCold->dependencies.size();
        }

        inline bool hasDependencies() const
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            return !m_pCold->dependencies.empty();
        }

        inline rz_critical_section getCriticalSection()
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            return m_pCold->CS;
        }

        inline void lockAsset()
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            rz_critical_section_lock(&m_pCold->CS);
        }

        inline void unlockAsset()
        {
            RAZIX_ASSERT(m_pCold != NULL, "Cold data not initialized");
            rz_critical_section_unlock(&m_pCold->CS);
        }

        inline RZUUID operator()() { return m_Hot.UUID; }
        inline bool   operator==(RZAsset& other) { return m_Hot.UUID == other.m_Hot.UUID; }
        inline bool   operator!=(RZAsset& other) { return m_Hot.UUID != other.m_Hot.UUID; }

    protected:
        RZAssetHotData   m_Hot   = {};
        RZAssetColdData* m_pCold = NULL;
    };
    static_assert(sizeof(RZAsset) == RAZIX_CACHE_LINE_SIZE, "RZAsset size must be equal cache line size");

}    // namespace Razix
