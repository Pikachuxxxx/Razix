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

namespace Razix {
    namespace Memory {
        class RZHeapAllocator;
    }
}    // namespace Razix

#define RAZIX_MAX_ASSETS 4096    // 1 Million assets max for now

namespace Razix {

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

        template<typename T>
        rz_asset_handle allocateAsset()
        {
            RZScopedCriticalSection lock(m_AssetDBLock);
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
            RZScopedCriticalSection lock(m_AssetDBLock);
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
    };

}    // namespace Razix

#endif    // _RZ_ASSET_DB_H_
