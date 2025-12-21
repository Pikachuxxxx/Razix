#pragma once

#include "Razix/Core/RZDepartments.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/string.h"

#include "Razix/Core/std/atomics.h"

#include "Razix/Core/UUID/RZUUID.h"

#include "Razix/Events/RZEvent.h"
#include <Core/RZCore.h>

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

    // Asset Events are defined in the central RZEvent.h file under EventType enum

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
        kNonMemoryBacked,
        COUNT
    };

    RAZIX_ALIGN_TO(MEM_DEF_ALIGNMENT_16)
    typedef struct RZAssetVersion
    {
        RZUUID revisionID = {};
        u64    major      = 0;
        u64    _pad0      = 0;
    } RZAssetVersion;

    RAZIX_ALIGN_TO(MEM_DEF_ALIGNMENT_16)
    struct RZAssetMetadata
    {
        RZString       name;
        RZString       category;
        RZString       author;
        RZString       lastModified;
        RZString       createdDate;
        RZString       description;
        RZString       commitHash; // TODO: make this a RZUUID?
        RZAssetVersion version;
        Department     department;
        u32            _pad0;
    };

    RAZIX_ALIGN_TO(MEM_DEF_ALIGNMENT_16)
    struct RZAssetDependecy
    {
        RZAssetType type;
        RZUUID      assetID;
    };

    /**
     * RZAsset is the base class for the all assets in the engine
     * Razix follows the principle "Everything in an Asset"
     * This class helps maintain a common base interface different between different assets
     * and also help with serialization and reflection and proxy views.
     */
    class RAZIX_MEM_ALIGN_16 RZAsset
    {
    public:
        /* User cannot do this by their own */
        RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZAsset);

        RAZIX_VIRTUAL_DESCTURCTOR(RZAsset);

        RZAsset(const RZUUID& uuid, AssetType type);

        virtual void serialize() const = 0;
        virtual void deserialize()     = 0;

        // Event management (listeners for asset changes)
        // Add an event listener for a specific event type
        template<typename EventT, typename EventCbFn>
        void addEventListener(EventCbFn&& callback)
        {
            m_EventDispatcher.registerCallback<EventT>(std::forward<EventCbFn>(callback));
        }

        // Notify listeners of a specific event
        void notifyListeners(RZEvent& event) { m_EventDispatcher.dispatch(event); }

        inline const RZUUID&                         getUUID() const { return m_UUID; }
        inline AssetType                             getType() const { return m_Type; }
        inline void                                  addReference() { rz_atomic64_increment(&m_ReferenceCount, RZ_MEMORY_ORDER_RELAXED); }
        inline void                                  removeReference() { rz_atomic64_decrement(&m_ReferenceCount, RZ_MEMORY_ORDER_RELAXED); }
        inline u64                                   getReferenceCount() { return rz_atomic64_load(&m_ReferenceCount, RZ_MEMORY_ORDER_RELAXED); }
        inline void                                  markDirty() { m_IsDirty = true; }
        inline void                                  clearDirty() { m_IsDirty = false; }
        inline bool                                  isDirty() const { return m_IsDirty; }
        inline void                                  setMetadata(const AssetMetadata& metadata) { m_Metadata = metadata; }
        inline const AssetMetadata&                  getMetadata() const { return m_Metadata; }
        inline void                                  acquireLockOnAsset() { m_Mutex.lock(); }
        inline void                                  releaseLockOnAsset() { m_Mutex.unlock(); }
        inline void                                  addDependency(AssetType assetType, const RZUUID& assetID) { m_Dependencies.push_back({assetType, assetID}); }
        inline const RZDynamicArray<AssetDependecy>& getDependencies() const { return m_Dependencies; }

        RZUUID operator()() { return m_UUID; }
        bool   operator==(RZAsset& other) { return m_UUID == other.m_UUID; }
        bool   operator!=(RZAsset& other) { return m_UUID != other.m_UUID; }

    protected:
        RZUUID                         m_UUID;              /* Unique identifier                              */
        AssetType                      m_Type;              /* Type of the asset (e.g., "Texture", "Model")   */
        rz_atomic_u64                  m_ReferenceCount;    /* Reference count for ownership tracking         */
        RZDynamicArray<AssetDependecy> m_Dependencies;      /* Other assets this asset depends on             */
        AssetMetadata                  m_Metadata;          /* Additional metadata                            */
        AssetStorageType               m_StoragePreference; /* Asset memory storage type                      */
        bool                           m_IsDirty;           /* Flag for tracking modifications                */
        Razix::RZEventDispatcher       m_EventDispatcher;   /* Listeners for asset updates                    */
        mutable std::mutex             m_Mutex;             /* Thread safety lock                             */
        bool                           m_AllocateLazy;      /* This resource storage will be done lazily      */
    };
}    // namespace Razix
