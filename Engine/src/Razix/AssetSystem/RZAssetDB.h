#ifndef _RZ_ASSET_DB_H_
#define _RZ_ASSET_DB_H_

#include <Core/Containers/hash_map.h>
#include <type_traits>

#include "Razix/Core/RZThreadCore.h"
#include "Razix/Core/Utils/TRZSingleton.h"

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/AssetSystem/RZAssetPool.h"

#include "Razix/Asset/RZAnimationAsset.h"
#include "Razix/Asset/RZAssetRefAsset.h"
#include "Razix/Asset/RZAudioAsset.h"
#include "Razix/Asset/RZCameraAsset.h"
#include "Razix/Asset/RZClothAsset.h"
#include "Razix/Asset/RZGameDataAsset.h"
#include "Razix/Asset/RZLightAsset.h"
#include "Razix/Asset/RZLuaScriptAsset.h"
#include "Razix/Asset/RZMaterialAsset.h"
#include "Razix/Asset/RZMeshAsset.h"
#include "Razix/Asset/RZPhysicsMaterialAsset.h"
#include "Razix/Asset/RZTextureAsset.h"
#include "Razix/Asset/RZTransformAsset.h"
#include "Razix/Asset/RZVignerePuzzleAsset.h"

#include "Razix/Core/Containers/string_utils.h"

#define RAZIX_ASSET_DB_REGISTRY_FILE      "//Assets/assetdb.bin"
#define RAZIX_ASSET_DB_REGISTRY_FILE_NAME "assetdb.bin"
#define RAZIX_MAX_ASSETS                  4096    // 1 Million assets max for now
#define RAZIX_ASSET_MAX_FILE_PATH_LENGTH  256
#define RAZIX_ASSSET_FILE_MAGIC           0x46415A52    // 'R','Z','A','F' (Razix Archive File) = 0x525A4146 [echo "RZAF" | xxd]
#define RAZIX_DATABASE_FILE_MAGIC         0x42445A52    // 'R','Z','D','B' (Razix Database File) = 0x525A4442 [echo "RZDB" | xxd]

// AssetBD.bin format
// [u32 count]
// repeated:
//   [16 byte UUID]
//   [u32 string length]
//   [char[] path]

namespace Razix {
    namespace Memory {
        class RZHeapAllocator;
    }
}    // namespace Razix

namespace Razix {

    class RZPakFile;

    struct RZAssetAsyncLoadJobData
    {
        RZAsset*    pAsset;
        RZAssetType AssetType;
        RZString    VFSFilePath;
    };

    class RAZIX_API RZAssetDB final : public RZSingleton<RZAssetDB>
    {
    public:
        RZAssetDB()  = default;
        ~RZAssetDB() = default;

        void Startup(Memory::RZHeapAllocator& assetAllocator, Memory::RZHeapAllocator& assetHeaderAllocator);
        void Shutdown();

        static u64 ComputeMinPoolBudgetBytesForMaxAssets(u64 maxAssets);
        static u64 ComputeMinHeaderBudgetBytesForMaxAssets(u64 maxAssets);

        template<typename T>
        RZAssetPool<T>& GetAssetPoolRef()
        {
            if constexpr (std::is_same_v<T, RZTransformAsset>) {
                return m_TransformAssetPool;
            } else if constexpr (std::is_same_v<T, RZMeshAsset>) {
                return m_MeshAssetPool;
            } else if constexpr (std::is_same_v<T, RZMaterialAsset>) {
                return m_MaterialAssetPool;
            } else if constexpr (std::is_same_v<T, RZTextureAsset>) {
                return m_TextureAssetPool;
            } else if constexpr (std::is_same_v<T, RZAnimationAsset>) {
                return m_AnimationAssetPool;
            } else if constexpr (std::is_same_v<T, RZAudioAsset>) {
                return m_AudioAssetPool;
            } else if constexpr (std::is_same_v<T, RZLuaScriptAsset>) {
                return m_LuaScriptAssetPool;
            } else if constexpr (std::is_same_v<T, RZPhysicsMaterialAsset>) {
                return m_PhysicsMaterialAssetPool;
            } else if constexpr (std::is_same_v<T, RZAssetRefAsset>) {
                return m_AssetRefAssetPool;
            } else if constexpr (std::is_same_v<T, RZCameraAsset>) {
                return m_CameraAssetPool;
            } else if constexpr (std::is_same_v<T, RZLightAsset>) {
                return m_LightAssetPool;
            } else if constexpr (std::is_same_v<T, RZGameDataAsset>) {
                return m_GameDataAssetPool;
            } else if constexpr (std::is_same_v<T, RZClothAsset>) {
                return m_ClothAssetPool;
            } else if constexpr (std::is_same_v<T, RZVignerePuzzleAsset>) {
                return m_VignerePuzzleAssetPool;
            } else {
                static_assert(false, "Unsupported asset type for asset pool retrieval");
            }
        }

        template<typename T>
        static constexpr RZAssetType GetAssetTypeTag()
        {
            if constexpr (std::is_same_v<T, RZTransformAsset>) {
                return RZAssetType::kTransform;
            } else if constexpr (std::is_same_v<T, RZMeshAsset>) {
                return RZAssetType::kMesh;
            } else if constexpr (std::is_same_v<T, RZMaterialAsset>) {
                return RZAssetType::kMaterial;
            } else if constexpr (std::is_same_v<T, RZTextureAsset>) {
                return RZAssetType::kTexture;
            } else if constexpr (std::is_same_v<T, RZAnimationAsset>) {
                return RZAssetType::kAnimation;
            } else if constexpr (std::is_same_v<T, RZAudioAsset>) {
                return RZAssetType::kAudio;
            } else if constexpr (std::is_same_v<T, RZLuaScriptAsset>) {
                return RZAssetType::kLuaScript;
            } else if constexpr (std::is_same_v<T, RZPhysicsMaterialAsset>) {
                return RZAssetType::kPhysicsMaterial;
            } else if constexpr (std::is_same_v<T, RZAssetRefAsset>) {
                return RZAssetType::kAssetRef;
            } else if constexpr (std::is_same_v<T, RZCameraAsset>) {
                return RZAssetType::kCamera;
            } else if constexpr (std::is_same_v<T, RZLightAsset>) {
                return RZAssetType::kLight;
            } else if constexpr (std::is_same_v<T, RZGameDataAsset>) {
                return RZAssetType::kGameData;
            } else if constexpr (std::is_same_v<T, RZClothAsset>) {
                return RZAssetType::kCloth;
            } else if constexpr (std::is_same_v<T, RZVignerePuzzleAsset>) {
                return RZAssetType::kVignerePuzzle;
            } else {
                static_assert(!std::is_same_v<T, T>, "Unsupported asset type for asset type tag retrieval");
            }
        }

        void* GetAssetPoolFromType(RZAssetType type)
        {
            switch (type) {
                case RZAssetType::kTransform: return &m_TransformAssetPool;
                case RZAssetType::kCamera: return &m_CameraAssetPool;
                case RZAssetType::kLight: return &m_LightAssetPool;
                case RZAssetType::kMaterial: return &m_MaterialAssetPool;
                case RZAssetType::kPhysicsMaterial: return &m_PhysicsMaterialAssetPool;
                case RZAssetType::kMesh: return &m_MeshAssetPool;
                case RZAssetType::kTexture: return &m_TextureAssetPool;
                case RZAssetType::kAnimation: return &m_AnimationAssetPool;
                case RZAssetType::kAudio: return &m_AudioAssetPool;
                case RZAssetType::kLuaScript: return &m_LuaScriptAssetPool;
                case RZAssetType::kAssetRef: return &m_AssetRefAssetPool;
                case RZAssetType::kVignerePuzzle: return &m_VignerePuzzleAssetPool;
                case RZAssetType::kCloth: return &m_ClothAssetPool;
                case RZAssetType::kGameData: return &m_GameDataAssetPool;
                default:
                    RAZIX_CORE_ASSERT(false, "Unsupported asset type");
                    return nullptr;
            }
        }

        template<typename T>
        RZAssetPool<T>& GetAssetPoolFromType(RZAssetType type)
        {
            return *static_cast<RZAssetPool<T>*>(GetAssetPoolFromType(type));
        }

        static RZAssetType GetAssetTypeFromTypeIndex(std::type_index type)
        {
            if (type == typeid(RZTransformAsset))
                return RZAssetType::kTransform;
            else if (type == typeid(RZMeshAsset))
                return RZAssetType::kMesh;
            else if (type == typeid(RZMaterialAsset))
                return RZAssetType::kMaterial;
            else if (type == typeid(RZTextureAsset))
                return RZAssetType::kTexture;
            else if (type == typeid(RZAnimationAsset))
                return RZAssetType::kAnimation;
            else if (type == typeid(RZAudioAsset))
                return RZAssetType::kAudio;
            else if (type == typeid(RZLuaScriptAsset))
                return RZAssetType::kLuaScript;
            else if (type == typeid(RZPhysicsMaterialAsset))
                return RZAssetType::kPhysicsMaterial;
            else if (type == typeid(RZAssetRefAsset))
                return RZAssetType::kAssetRef;
            else if (type == typeid(RZCameraAsset))
                return RZAssetType::kCamera;
            else if (type == typeid(RZLightAsset))
                return RZAssetType::kLight;
            else if (type == typeid(RZGameDataAsset))
                return RZAssetType::kGameData;
            else if (type == typeid(RZClothAsset))
                return RZAssetType::kCloth;
            else if (type == typeid(RZVignerePuzzleAsset))
                return RZAssetType::kVignerePuzzle;

            RAZIX_CORE_ASSERT(false, "Unsupported asset type");
            return RZAssetType::kTransform;
        }

        template<typename T>
        rz_asset_handle allocateAsset()
        {
            const RZAssetType       assetType   = GetAssetTypeTag<T>();
            u32                     headerIndex = m_HeaderPool.allocate(assetType);
            if (headerIndex == RAZIX_ASSET_INVALID_HANDLE)
                return RAZIX_ASSET_INVALID_HANDLE;

            RZAssetPool<T>& pool         = GetAssetPoolRef<T>();
            u32             payloadIndex = pool.allocate(assetType);
            if (payloadIndex == RAZIX_ASSET_INVALID_HANDLE) {
                m_HeaderPool.release(headerIndex);
                return RAZIX_ASSET_INVALID_HANDLE;
            }

            rz_asset_handle handle = (static_cast<rz_asset_handle>(payloadIndex) << RAZIX_ASSET_PAYLOAD_SHIFT_INDEX) | static_cast<rz_asset_handle>(headerIndex);
            return handle;
        }

        template<typename T>
        void releaseAsset(rz_asset_handle handle)
        {
            u32                     headerIndex  = static_cast<u32>(handle & RAZIX_ASSET_HOTDATA_MASK);
            u32                     payloadIndex = static_cast<u32>((handle & RAZIX_ASSET_PAYLOLAD_INDEX_MASK) >> RAZIX_ASSET_PAYLOAD_SHIFT_INDEX);

            RZAssetPool<T>& pool = GetAssetPoolRef<T>();
            pool.release(payloadIndex);
            m_HeaderPool.release(headerIndex);
        }

        template<typename T>
        const T* getAssetResourcePtr(rz_asset_handle handle) const
        {
            const RZAssetPool<T>& pool  = GetAssetPoolRef<T>();
            u32                   index = static_cast<u32>((handle & RAZIX_ASSET_PAYLOLAD_INDEX_MASK) >> RAZIX_ASSET_PAYLOAD_SHIFT_INDEX);
            return pool.get(index);
        }

        template<typename T>
        T* getAssetResourceMutablePtr(rz_asset_handle handle)
        {
            RZAssetPool<T>& pool  = GetAssetPoolRef<T>();
            u32             index = static_cast<u32>((handle & RAZIX_ASSET_PAYLOLAD_INDEX_MASK) >> RAZIX_ASSET_PAYLOAD_SHIFT_INDEX);
            return pool.get(index);
        }

        const RZAsset* getAssetHeader(rz_asset_handle handle) const
        {
            u32 headerIndex = static_cast<u32>(handle & RAZIX_ASSET_HOTDATA_MASK);
            return m_HeaderPool.get(headerIndex);
        }

        RZAsset* getAssetHeaderMutable(rz_asset_handle handle)
        {
            u32 headerIndex = static_cast<u32>(handle & RAZIX_ASSET_HOTDATA_MASK);
            return m_HeaderPool.getMutablePtr(headerIndex);
        }

        template<typename T>
        rz_asset_handle createAsset(const RZString& name)
        {
            RZScopedCriticalSection lock(m_AssetDBLock);

            rz_asset_handle handle = allocateAsset<T>();
            if (handle == RAZIX_ASSET_INVALID_HANDLE)
                return handle;

            RZAsset* pAsset = getAssetHeaderMutable(handle);
            RAZIX_CORE_ASSERT(pAsset, "[AssetDB] Invalid header after allocation.");
            pAsset->setHandle(handle);
            pAsset->setName(name);
            pAsset->setFlags(RZAssetFlags(RZ_ASSET_FLAG_DIRTY));

#if RAZIX_IS_DEVELOPMENT_BUILD
            const RZAssetType type = GetAssetTypeTag<T>();
            RZString          path = AssetTypeToVFSFilePath(type, name);

            m_AssetDBDevRegistry[pAsset->getUUID()] = path;
#endif    // RAZIX_IS_DEVELOPMENT_BUILD
            return handle;
        }

        template<typename T>
        void destroyAsset(rz_asset_handle handle)
        {
            RZScopedCriticalSection lock(m_AssetDBLock);

            RZAsset* header = m_HeaderPool.getMutablePtr(handle);
            if (!header) {
                RAZIX_CORE_ERROR("[AssetDB] Invalid asset handle for destruction.");
                return;
            }

#if RAZIX_IS_DEVELOPMENT_BUILD
            m_AssetDBDevRegistry.remove(header->getUUID());
#endif    // RAZIX_IS_DEVELOPMENT_BUILD
            releaseAsset<T>(handle);
        }

        // Unified load
        // Checks RZEngine::BuildMode and calls either loadAssetFromDisk or loadAssetFromPak
        template<typename T>
        rz_asset_handle requestAssetLoad(RZUUID assetUUID)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);
#if RAZIX_IS_DEVELOPMENT_BUILD
            // In development builds, we can load assets directly from disk using the registry for faster iteration
            return requestAssetLoadFromDisk<T>(assetUUID);
#else
            RAZIX_UNIMPLEMENTED_METHOD;
            return RAZIX_ASSET_INVALID_HANDLE;
#endif
        }

#if RAZIX_IS_DEVELOPMENT_BUILD
        // All are called in Async fashion, they immediately return with default handle, and once the asset is loaded/saved,
        // the handle is updated with the actual handle
        // Paks are owned by scenegraph, so they will call these functions to load/save assets from/to paks
        bool saveAssetToDisk(rz_asset_handle handle) const;
        template<typename T>
        rz_asset_handle requestAssetLoadFromDisk(RZUUID assetUUID)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if constexpr (std::is_same_v<T, RZTransformAsset>) {
                RAZIX_CORE_ERROR("[AssetSystem] Loading Transform assets from disk is not supported as they are managed and loaded by the SceneGraph.");
                return RAZIX_ASSET_INVALID_HANDLE;
            }

            // TODO: check if an asset handle with given UUID exists already before creating a new one
            auto                    it = m_AssetDBDevRegistry.find(assetUUID);
            if (it == m_AssetDBDevRegistry.end()) {
                RAZIX_CORE_WARN("[AssetSystem] Asset UUID {} not found in registry.", assetUUID.prettyString());
                return RAZIX_ASSET_INVALID_HANDLE;
            }

            rz_asset_handle handle = createAsset<T>(GetFileName(it->second));
            // Set in loading state
            RZAsset* pPlaceholderAsset = getAssetHeaderMutable(handle);
            pPlaceholderAsset->addFlags(RZ_ASSET_FLAG_PLACEHOLDER);
            pPlaceholderAsset->addFlags(RZ_ASSET_FLAG_STREAMING);
            // No longer dirty once we make it a placeholder, it will be marked dirty again once it's ready and scenegraph has not touched the updated version yet.
            pPlaceholderAsset->removeFlags(RZ_ASSET_FLAG_DIRTY);

            requestAssetLoadFromDiskInternal(assetUUID, typeid(T), pPlaceholderAsset);
            // TODO:!!! Create a default asset per asset type to return here while the actual asset is being loaded async, so that the game can start
            // Create a dummy asset with default places holder

            return handle;
        }
        void requestAssetLoadFromDiskInternal(RZUUID assetUUID, std::type_index typeIdx, RZAsset* pPlaceholderAsset);

        // In development builds, we can save/load the entire registry to a single file for faster iteration
        // called when AssetDB startup/shutsdown happens
        // No streaming or zone --> pak loading support for this, just a single
        // file with all assets in it, used for faster iteration during development
        bool loadAssetDBRegistry();
        void exportAssetDBRegistry() const;
#elif RAZIX_IS_SHIPPING_BUILD

        // Pak file variants
        bool            saveAssetToPak(rz_asset_handle handle, RZPakFile* pakFile) const;
        rz_asset_handle loadAssetFromPak(RZUUID assetUUID, RZPakFile* pakFile);

        // Called Async by the SceneManager when it loads new zones from main game thread,
        // once the pak file is loaded, it will call this function to load all assets from
        // the pak file into the asset db, so that they can be used by the scene, this final
        // step is done by calling buildAssetDBFromPak, which will read the pak file and populate
        // the asset db, then the scene can start using the assets from the asset db. In the meantime,
        // Since we can have Async loading of assets, the scene can start using the assets as they are loaded into the asset db.
        void buildAssetDBFromPak(RZPakFile* pakFile);
#endif

    private:
        RZAssetPool<RZAnimationAsset>       m_AnimationAssetPool;
        RZAssetPool<RZAssetRefAsset>        m_AssetRefAssetPool;
        RZAssetPool<RZAudioAsset>           m_AudioAssetPool;
        RZAssetPool<RZCameraAsset>          m_CameraAssetPool;
        RZAssetPool<RZClothAsset>           m_ClothAssetPool;
        RZAssetPool<RZGameDataAsset>        m_GameDataAssetPool;
        RZAssetPool<RZLightAsset>           m_LightAssetPool;
        RZAssetPool<RZLuaScriptAsset>       m_LuaScriptAssetPool;
        RZAssetPool<RZMaterialAsset>        m_MaterialAssetPool;
        RZAssetPool<RZMeshAsset>            m_MeshAssetPool;
        RZAssetPool<RZPhysicsMaterialAsset> m_PhysicsMaterialAssetPool;
        RZAssetPool<RZTextureAsset>         m_TextureAssetPool;
        RZAssetPool<RZTransformAsset>       m_TransformAssetPool;
        RZAssetPool<RZVignerePuzzleAsset>   m_VignerePuzzleAssetPool;
        RZAssetHeaderPool                   m_HeaderPool;

        rz_critical_section      m_AssetDBLock          = {};
        Memory::RZHeapAllocator* m_AssetAllocator       = NULL;
        Memory::RZHeapAllocator* m_AssetHeaderAllocator = NULL;
        u32                      m_BudgetInMB           = 0;

        //Not using union to avoid lifetime issues with non-trivial types
#ifdef RAZIX_IS_DEVELOPMENT_BUILD
        RZHashMap<RZUUID, RZString> m_AssetDBDevRegistry;    // Used in Development buillds
#elif RAZIX_IS_SHIPPING_BUILD
        RZHashMap<RZUUID, RZPakFile*> m_AssetPakRegistry;    // Used in Shipping builds
#endif    // RAZIX_IS_SHIPPING_BUILD
    };

}    // namespace Razix

#endif    // _RZ_ASSET_DB_H_
