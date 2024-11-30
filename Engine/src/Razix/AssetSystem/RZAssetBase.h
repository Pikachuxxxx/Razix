#pragma once

#include "Razix/Core/RZDepartments.h"
#include "Razix/Events/RZEvent.h"

namespace Razix {
    namespace AssetSystem {

        /* Lightweight proxy view for RZAsset */
        class RZAssetProxy;

        // trick to dynamically generate asset type enum

#define ASSET_TYPE_LIST                                                                                                                                                            \
    X(Undefined)                                                                                                                                                                   \
    X(Transform)                                                                                                                                                                   \
    X(Camera)                                                                                                                                                                      \
    X(Light)                                                                                                                                                                       \
    X(Material)                                                                                                                                                                    \
    X(Mesh)                                                                                                                                                                        \
    X(Texture)                                                                                                                                                                     \
    X(Animation)                                                                                                                                                                   \
    X(Audio)                                                                                                                                                                       \
    X(LuaScript)                                                                                                                                                                   \
    X(UserDataContainer)    // Generic asset type to store some generic data for gameplay/scripting (ex. PlayerHealthStats/EnemyNPCTypes/Weapons/RegionalPowerups)

        enum class AssetType
        {
#define X(name) k##name,
            ASSET_TYPE_LIST
#undef X
                COUNT
        };

        // Asset Events are defined in the central RZEvent.h file under EventType enum

        /**
         * Defined how the assets are stored in the database pools
         * MemoryBacked types are allocated memory by the RZAssetBuilder and stores there 
         * GPUBacked doesn't create any memory but provide with proxy views for lazy allocation using RZResourceManager
         * NonMemoryBacked resources are run-time assets in the engine instantiated by scripting etc. and are non-persistent
         */
        enum class AssetStorageType
        {
            kMemoryBacked,       // Stored in memory
            kGPUBacked,          // Requires GPU memory for representation, uses Proxy views for translation
            kNonMemoryBacked,    // Runtime or procedurally generated (e.g., external URLs, noise textures) not stored to disk
            COUNT
        };

        struct RAZIX_MEM_ALIGN_16 AssetVersion
        {
            int         major        = 0; /* Major version (e.g., breaking changes) */
            int         minor        = 0; /* Minor version (e.g., added features, backwards-compatible) */
            int         patch        = 0; /* Patch version (e.g., bug fixes) */
            int         _padding     = {0};
            std::string revisionID   = ""; /* Unique ID for this version (e.g., hash, GUID) */
            int         _padding2[2] = {0, 0};

            AssetVersion(int major = 1, int minor = 0, int patch = 0, const std::string& revID = "") : major(major), minor(minor), patch(patch), revisionID(revID) {}

            std::string toString() const
            {
                return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch) + (revisionID.empty() ? "" : (" (" + revisionID + ")"));
            }
        };

        struct RAZIX_MEM_ALIGN_16 AssetMetadata
        {
            std::string              name;         /* Name of the asset                                         */
            std::string              category;     /* Category (e.g., "Texture", "Material", "Audio")           */
            std::vector<std::string> tags;         /* Tags for easy searching and filtering                     */
            AssetVersion             version;      /* Version info                                              */
            std::string              author;       /* Author of the asset                                       */
            std::string              lastModified; /* Date/Time of last modification (e.g., "2024-11-24 15:30") */
            std::string              createdDate;  /* Date/Time when the asset was originally created           */
            std::string              description;  /* Short description of the asset                            */
            std::string              commitHash;   /* Version control ID/commit hash                            */
            Department               department;   /* Department responsible for the asset                      */
            int                      _padding;
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

            RAZIX_INLINE const RZUUID&        getUUID() const { return m_UUID; }
            RAZIX_INLINE AssetType            getType() const { return m_Type; }
            RAZIX_INLINE void                 addReference() { m_ReferenceCount.fetch_add(1, std::memory_order_relaxed); }
            RAZIX_INLINE void                 removeReference() { m_ReferenceCount.fetch_sub(1, std::memory_order_relaxed); }
            RAZIX_INLINE int                  getReferenceCount() const { return m_ReferenceCount.load(std::memory_order_relaxed); }
            RAZIX_INLINE void                 markDirty() { m_IsDirty = true; }
            RAZIX_INLINE void                 clearDirty() { m_IsDirty = false; }
            RAZIX_INLINE bool                 isDirty() const { return m_IsDirty; }
            RAZIX_INLINE void                 setMetadata(const AssetMetadata& metadata) { m_Metadata = metadata; }
            RAZIX_INLINE const AssetMetadata& getMetadata() const { return m_Metadata; }
            RAZIX_INLINE void                 acquireLockOnAsset() { m_Mutex.lock(); }
            RAZIX_INLINE void                 releaseLockOnAsset() { m_Mutex.unlock(); }
            RAZIX_INLINE void                 addDependency(const RZUUID& assetID) { m_Dependencies.push_back(assetID); }
            RAZIX_INLINE const std::vector<RZUUID>& getDependencies() const { return m_Dependencies; }

            RZUUID operator()() { return m_UUID; }
            bool   operator==(RZAsset& other) { return m_UUID == other.m_UUID; }
            bool   operator!=(RZAsset& other) { return m_UUID != other.m_UUID; }

        protected:
            RZUUID                   m_UUID;              /* Unique identifier                              */
            AssetType                m_Type;              /* Type of the asset (e.g., "Texture", "Model")   */
            std::atomic<u32>         m_ReferenceCount;    /* Reference count for ownership tracking         */
            std::vector<RZUUID>      m_Dependencies;      /* Other assets this asset depends on             */
            AssetMetadata            m_Metadata;          /* Additional metadata                            */
            AssetStorageType         m_StoragePreference; /* Asset memory storage type                      */
            bool                     m_IsDirty;           /* Flag for tracking modifications                */
            Razix::RZEventDispatcher m_EventDispatcher;   /* Listeners for asset updates                    */
            mutable std::mutex       m_Mutex;             /* Thread safety lock                             */
            bool                     m_AllocateLazy;      /* This resource storage will be done lazily      */
        };
    }    // namespace AssetSystem
}    // namespace Razix
