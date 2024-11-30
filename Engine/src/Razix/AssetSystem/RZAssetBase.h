#pragma once

#include "Razix/Core/RZDepartments.h"
#include "Razix/Events/RZEvent.h"

namespace Razix {
    class RZApplication;
    namespace AssetSystem {

        /* Lightweight proxy view for RZAsset */
        class IRZAssetProxy;

        // trick to dynamically generate asset type enum

#define ASSET_TYPE_LIST \
    X(Undefined)        \
    X(Transform)        \
    X(Camera)           \
    X(Light)            \
    X(Material)         \
    X(Mesh)             \
    X(Texture)          \
    X(Animation)        \
    X(Audio)            \
    X(LuaScript)

        // Step 2: Enum Definition
        enum class AssetType
        {
#define X(name) k##name,
            ASSET_TYPE_LIST
#undef X
                COUNT
        };

        enum class AssetEventType
        {
            kCreated,
            kModified,
            kMemoryMoved,
            kCopied,
            kDeleted,
            COUNT
        };

        enum class AssetStorageType
        {
            MemoryBacked,       // Stored in memory
            NonMemoryBacked,    // Referenced or procedurally generated (e.g., external URLs, noise textures)
            GPUBacked           // Requires GPU memory for representation
        };

        struct AssetVersion
        {
            int         major;      /* Major version (e.g., breaking changes) */
            int         minor;      /* Minor version (e.g., added features, backwards-compatible) */
            int         patch;      /* Patch version (e.g., bug fixes) */
            std::string revisionID; /* Unique ID for this version (e.g., hash, GUID) */

            AssetVersion(int major = 1, int minor = 0, int patch = 0, const std::string& revID = "")
                : major(major), minor(minor), patch(patch), revisionID(revID) {}

            std::string toString() const
            {
                return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch) +
                       (revisionID.empty() ? "" : (" (" + revisionID + ")"));
            }
        };

        struct AssetMetadata
        {
            std::string              name;         /* Name of the asset */
            std::string              category;     /* Category (e.g., "Texture", "Material", "Audio") */
            std::vector<std::string> tags;         /* Tags for easy searching and filtering */
            AssetVersion             version;      /* Versioning info */
            std::string              author;       /* Author of the asset */
            Department               department;   /* Department responsible for the asset */
            std::string              lastModified; /* Date/Time of last modification (e.g., "2024-11-24 15:30") */
            std::string              createdDate;  /* Date/Time when the asset was originally created */
            std::string              description;  /* Short description of the asset */
            std::string              commitHash;   /* Version control ID/commit hash */
        };

        /**
         * RZAsset is the base class for the all assets in the engine
         * Razix follows the princinple "Everything in an Asset"
         * This class helps maintaing a common base interface different between different assets
         * and also help with serializarino and reflection and proxy views.
         */
        class RZAsset
        {
        public:
            /* User cannot do this by their own */
            RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZAsset);

            RZAsset(const RZUUID& uuid, AssetType type);

            // UUID getter
            RAZIX_INLINE const RZUUID& getUUID() const { return m_UUID; }
            // Asset type getter
            AssetType getType() const { return m_Type; }

            virtual void serialize() const = 0;
            virtual void deserialize()     = 0;

            // Reference management
            void addReference() { m_ReferenceCount.fetch_add(1, std::memory_order_relaxed); }
            void removeReference() { m_ReferenceCount.fetch_sub(1, std::memory_order_relaxed); }
            int  getReferenceCount() const { return m_ReferenceCount.load(std::memory_order_relaxed); }

            // Dependency management
            void                       addDependency(const RZUUID& assetID) { m_Dependencies.push_back(assetID); }
            const std::vector<RZUUID>& getDependencies() const { return m_Dependencies; }

            // Dirty flag management
            RAZIX_INLINE void markDirty() { m_IsDirty = true; }
            RAZIX_INLINE void clearDirty() { m_IsDirty = false; }
            RAZIX_INLINE bool isDirty() const { return m_IsDirty; }

            // Event management (listeners for asset changes)
            // Add an event listener for a specific event type
            template<typename EventT, typename EventCbFn>
            void addEventListener(EventCbFn&& callback)
            {
                m_EventDispatcher.registerCallback<EventT>(std::forward<EventCbFn>(callback));
            }

            // Notify listeners of a specific event
            void notifyListeners(RZEvent& event)
            {
                m_EventDispatcher.dispatch(event);
            }

            // Metadata
            void                 setMetadata(const AssetMetadata& metadata) { m_Metadata = metadata; }
            const AssetMetadata& getMetadata() const { return m_Metadata; }

            // Thread safety (locking utilities)
            void acquireLockOnAsset() { m_Mutex.lock(); }
            void releaseLockOnAsset() { m_Mutex.unlock(); }

            RZUUID operator()() { return m_UUID; }
            bool   operator==(RZAsset& other) { return m_UUID == other.m_UUID; }
            bool   operator!=(RZAsset& other) { return m_UUID != other.m_UUID; }

        protected:
            RZUUID                   m_UUID;               // Unique identifier
            AssetType                m_Type;               // Type of the asset (e.g., "Texture", "Model")
            std::atomic<u32>         m_ReferenceCount;     // Reference count for ownership tracking
            std::vector<RZUUID>      m_Dependencies;       // Other assets this asset depends on
            AssetMetadata            m_Metadata;           // Additional metadata
            bool                     m_IsDirty;            // Flag for tracking modifications
            Razix::RZEventDispatcher m_EventDispatcher;    // Listeners for asset updates
            mutable std::mutex       m_Mutex;              // Thread safety lock
            bool                     m_AllocateLazy;
        };
    }    // namespace AssetSystem
}    // namespace Razix
