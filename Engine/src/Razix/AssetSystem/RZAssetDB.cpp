// clang-format off
#include "rzxpch.h"
#include <AssetSystem/RZAssetBase.h>
// clang-format on

#include "RZAssetDB.h"

#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"
#include "Razix/Core/Profiling/RZProfiling.h"
#include "Razix/Core/RZThreadCore.h"

#include <type_traits>

namespace Razix {

    // TODO: move to more apt place, idk where
    static inline u64 rz_max_u64(u64 a, u64 b)
    {
        return (a > b) ? a : b;
    }

    static inline u32 rz_min_u32(u32 a, u32 b)
    {
        return (a < b) ? a : b;
    }
    
    //-------------------------------------------------------------------------
    
    static void OnAssetAllocate(rz_asset_handle handle, const char* label)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        RAZIX_CORE_INFO("[AssetSystem][{}] Allocated Asset with handle: {}", label, handle);
    }

    static void OnAssetRelease(rz_asset_handle handle, const char* label)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        RAZIX_CORE_INFO("[AssetSystem][{}] Released Asset with handle: {}", label, handle);
    }

    //-------------------------------------------------------------------------

    template<typename AssetT>
    constexpr u64 GetAssetPayloadSize()
    {
        if constexpr (std::is_same_v<AssetT, RZTransformAsset>) {
            return sizeof(RZTransformAsset) + sizeof(float4) * 3 + sizeof(float4x4) * 2;
        } else {
            return sizeof(AssetT);
        }
    }

    template<typename AssetT>
    u32 ComputeCapacityForSlice(u64 sliceBytes)
    {
        const u64 slotBytes = GetAssetPayloadSize<AssetT>() + sizeof(u32);    // payload + freelist entry per slot
        RAZIX_CORE_ASSERT(slotBytes > 0, "[AssetSystem] Invalid slot size while computing capacity.");

        u32 capacity = static_cast<u32>(sliceBytes / slotBytes);
        if (capacity == 0)
            capacity = 1;

        const u32 maxAssetsCap = static_cast<u32>(RAZIX_MAX_ASSETS);
        RAZIX_CORE_ASSERT(capacity >= maxAssetsCap, "[AssetSystem] Pool capacity {} below RAZIX_MAX_ASSETS {}. Increase asset pool budget or reduce pool count.", capacity, maxAssetsCap);
        capacity = maxAssetsCap;//rz_min_u32(capacity, maxAssetsCap);

        return capacity;
    }

    template<typename AssetT>
    void InitPoolWithSlice(RZAssetPool<AssetT>& pool, Memory::RZHeapAllocator& allocator, const char* label, u64 sliceBytes)
    {
        const u64 slotBytes      = GetAssetPayloadSize<AssetT>() + sizeof(u32);
        const u32 capacity       = ComputeCapacityForSlice<AssetT>(sliceBytes);
        const u64 allocationSize = slotBytes * capacity;
        RAZIX_CORE_ASSERT(allocationSize <= sliceBytes, "[AssetSystem] Allocation size exceeds slice for {}", label);

        void* memory = allocator.allocate(allocationSize);
        RAZIX_CORE_ASSERT(memory != NULL, "[AssetSystem] Failed to allocate memory for {}", label);

        RZAssetPoolConfig config = {};
        config.poolType          = Memory::RZ_MEM_POOL_TYPE_ASSET_POOL;
        config.debugLabel        = label;
        config.hooks.onAllocate  = OnAssetAllocate;
        config.hooks.onRelease   = OnAssetRelease;

        pool.init(memory, capacity, config);

        RAZIX_CORE_INFO("[AssetSystem][{}] Initialized | capacity: {} | memory slice: {} KiB", label, capacity, in_Kib(allocationSize));
    }

    static u32 ComputeHeaderCapacity(u64 sliceBytes)
    {
//        auto align_up = [](u64 value, u64 alignment) {
//            const u64 mask = alignment - 1;
//            return (value + mask) & ~mask;
//        };

        auto computeAllocBytes = [&](u32 cap) {
            u64 offset = sizeof(RZAsset) * cap;
//            offset     = align_up(offset, alignof(RZAssetColdData));
            offset += sizeof(RZAssetColdData) * cap;
//            offset = align_up(offset, alignof(u32));
            offset += sizeof(u32) * cap;
            return offset;
        };

        u32 capacity = static_cast<u32>(sliceBytes / (sizeof(RZAsset) + sizeof(RZAssetColdData) + sizeof(u32)));
        if (capacity == 0)
            capacity = 1;

        while (capacity > 0 && computeAllocBytes(capacity) > sliceBytes)
            --capacity;

        RAZIX_CORE_ASSERT(capacity > 0, "[AssetSystem] Invalid header capacity for slice: {} bytes", sliceBytes);
        return capacity;
    }

    static void InitHeaderPoolWithSlice(RZAssetHeaderPool& pool, Memory::RZHeapAllocator& allocator, u64 sliceBytes)
    {
//        auto align_up = [](u64 value, u64 alignment) {
//            const u64 mask = alignment - 1;
//            return (value + mask) & ~mask;
//        };

        const u32 capacity = ComputeHeaderCapacity(sliceBytes);

        u64 allocationSize = sizeof(RZAsset) * capacity;
//        allocationSize     = align_up(allocationSize, alignof(RZAssetColdData));
        allocationSize += sizeof(RZAssetColdData) * capacity;
//        allocationSize = align_up(allocationSize, alignof(u32));
        allocationSize += sizeof(u32) * capacity;

        void* memory = allocator.allocate(allocationSize);
        RAZIX_CORE_ASSERT(memory != NULL, "[AssetSystem] Failed to allocate memory for Header Asset Pool");

        pool.init(memory, capacity);

        RAZIX_CORE_INFO("[AssetSystem][Header Asset Pool] Initialized | capacity: {} | memory slice: {} KiB", capacity, in_Kib(allocationSize));
    }

    static u64 ComputeHeaderSlotBytesAligned()
    {
//        auto align_up = [](u64 value, u64 alignment) {
//            const u64 mask = alignment - 1;
//            return (value + mask) & ~mask;
//        };

        u64 bytes = sizeof(RZAsset);
//        bytes     = align_up(bytes, alignof(RZAssetColdData));
        bytes += sizeof(RZAssetColdData);
//        bytes  = align_up(bytes, alignof(u32));
        bytes += sizeof(u32);
        return bytes;
    }

    static u64 ComputeMaxPayloadSlotBytes()
    {
        const u64 transformSlot = GetAssetPayloadSize<RZTransformAsset>() + sizeof(u32);
        const u64 meshSlot      = GetAssetPayloadSize<RZMeshAsset>() + sizeof(u32);
        const u64 materialSlot  = GetAssetPayloadSize<RZMaterialAsset>() + sizeof(u32);
        const u64 textureSlot   = GetAssetPayloadSize<RZTextureAsset>() + sizeof(u32);
        const u64 animationSlot = GetAssetPayloadSize<RZAnimationAsset>() + sizeof(u32);
        const u64 audioSlot     = GetAssetPayloadSize<RZAudioAsset>() + sizeof(u32);
        const u64 scriptSlot    = GetAssetPayloadSize<RZLuaScriptAsset>() + sizeof(u32);
        const u64 physicsSlot   = GetAssetPayloadSize<RZPhysicsMaterialAsset>() + sizeof(u32);
        const u64 cameraSlot    = GetAssetPayloadSize<RZCameraAsset>() + sizeof(u32);
        const u64 lightSlot     = GetAssetPayloadSize<RZLightAsset>() + sizeof(u32);
        const u64 assetRefSlot  = GetAssetPayloadSize<RZAssetRefAsset>() + sizeof(u32);
        const u64 puzzleSlot    = GetAssetPayloadSize<RZVignerePuzzleAsset>() + sizeof(u32);
        const u64 clothSlot     = GetAssetPayloadSize<RZClothAsset>() + sizeof(u32);
        const u64 gameDataSlot  = GetAssetPayloadSize<RZGameDataAsset>() + sizeof(u32);

        u64 maxValue = transformSlot;
        maxValue     = rz_max_u64(maxValue, meshSlot);
        maxValue     = rz_max_u64(maxValue, materialSlot);
        maxValue     = rz_max_u64(maxValue, textureSlot);
        maxValue     = rz_max_u64(maxValue, animationSlot);
        maxValue     = rz_max_u64(maxValue, audioSlot);
        maxValue     = rz_max_u64(maxValue, scriptSlot);
        maxValue     = rz_max_u64(maxValue, physicsSlot);
        maxValue     = rz_max_u64(maxValue, cameraSlot);
        maxValue     = rz_max_u64(maxValue, lightSlot);
        maxValue     = rz_max_u64(maxValue, assetRefSlot);
        maxValue     = rz_max_u64(maxValue, puzzleSlot);
        maxValue     = rz_max_u64(maxValue, clothSlot);
        maxValue     = rz_max_u64(maxValue, gameDataSlot);

        return maxValue;
    }

    u64 RZAssetDB::ComputeMinBudgetBytesForMaxAssets(u64 maxAssets)
    {
        const u64 headerSlotBytes     = ComputeHeaderSlotBytesAligned();
        const u64 maxPayloadSlotBytes = ComputeMaxPayloadSlotBytes();

        RAZIX_CORE_ASSERT(headerSlotBytes > 0 && maxPayloadSlotBytes > 0, "[AssetSystem] Invalid slot sizing for budget computation.");
        return maxAssets * (headerSlotBytes + maxPayloadSlotBytes);
    }

    void RZAssetDB::Startup(Memory::RZHeapAllocator& assetAllocator)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        m_AssetDBLock = rz_critical_section_create();

        const Memory::MemoryPoolBudget assetBudget = Memory::GetMemoryPoolBudget(Memory::RZ_MEM_POOL_TYPE_ASSET_POOL);
        m_BudgetInMB                               = assetBudget.HeapSizeMB;

        const u64 budgetInBytes = Mib(static_cast<u64>(assetBudget.HeapSizeMB));
        RAZIX_CORE_ASSERT(budgetInBytes > 0, "[AssetSystem] Asset pool budget is 0 bytes!");

        const u32 numPayloadPools = static_cast<u32>(RZAssetType::COUNT);
        RAZIX_CORE_ASSERT(numPayloadPools > 0, "[AssetSystem] No asset pools configured!");

        const u64 headerSlotBytes       = sizeof(RZAsset) + sizeof(RZAssetColdData) + sizeof(u32);
        const u64 maxAssetsBudget       = static_cast<u64>(RAZIX_MAX_ASSETS);
        const u64 headerBytesNeeded     = headerSlotBytes * maxAssetsBudget;
        u64       headerBytesToAllocate = headerBytesNeeded;
        if (headerBytesToAllocate > budgetInBytes)
            headerBytesToAllocate = budgetInBytes / (numPayloadPools + 1);    // fallback to an even slice if over budget

        RAZIX_CORE_ASSERT(headerBytesToAllocate > 0, "[AssetSystem] Header pool reserved zero bytes; increase asset budget.");
        RAZIX_CORE_ASSERT(headerBytesToAllocate >= headerSlotBytes, "[AssetSystem] Header pool allocation below one slot ({} bytes available).", headerBytesToAllocate);
        RAZIX_CORE_INFO("[AssetSystem] Header slice: need={} KiB, alloc={} KiB, totalBudget={} KiB", in_Kib(headerBytesNeeded), in_Kib(headerBytesToAllocate), in_Kib(budgetInBytes));
        InitHeaderPoolWithSlice(m_HeaderPool, assetAllocator, headerBytesToAllocate);

        u64 payloadBudgetBytes = budgetInBytes - headerBytesToAllocate;
        u64 bytesRemaining     = payloadBudgetBytes;
        u32 poolsRemaining     = numPayloadPools;

        auto nextPayloadSlice = [&]() {
            RAZIX_CORE_ASSERT(poolsRemaining > 0, "[AssetSystem] Invalid pool slice request.");
            const u64 slice = (poolsRemaining == 1) ? bytesRemaining : bytesRemaining / poolsRemaining;
            bytesRemaining -= slice;
            --poolsRemaining;
            return slice;
        };

        u64 totalPayloadSlots = 0;

        auto initPayloadPool = [&](auto& pool, const char* label) {
            const u64 slice    = nextPayloadSlice();
            using PoolType     = std::decay_t<decltype(pool)>;
            using AssetType    = typename PoolType::value_type;
            const u32 capacity = ComputeCapacityForSlice<AssetType>(slice);
            RAZIX_CORE_ASSERT(capacity > 0, "[AssetSystem] Computed zero capacity for {}", label);
            totalPayloadSlots += capacity;
            RAZIX_CORE_INFO("[AssetSystem] Payload slice for {}: slice={} KiB, remaining={} KiB, capacity={}", label, in_Kib(slice), in_Kib(bytesRemaining), capacity);
            InitPoolWithSlice<AssetType>(pool, assetAllocator, label, slice);
        };

         initPayloadPool(m_TransformAssetPool, "Transform Asset Pool");
        // initPayloadPool(m_CameraAssetPool, "Camera Asset Pool");
        // initPayloadPool(m_LightAssetPool, "Light Asset Pool");
        // initPayloadPool(m_MaterialAssetPool, "Material Asset Pool");
        // initPayloadPool(m_PhysicsMaterialAssetPool, "Physics Material Asset Pool");
        // initPayloadPool(m_MeshAssetPool, "Mesh Asset Pool");
        // initPayloadPool(m_TextureAssetPool, "Texture Asset Pool");
        // initPayloadPool(m_AnimationAssetPool, "Animation Asset Pool");
        // initPayloadPool(m_AudioAssetPool, "Audio Asset Pool");
        // initPayloadPool(m_LuaScriptAssetPool, "Lua Script Asset Pool");
        // initPayloadPool(m_AssetRefAssetPool, "Asset Ref Pool");
        // initPayloadPool(m_VignerePuzzleAssetPool, "Vignere Puzzle Asset Pool");
        // initPayloadPool(m_ClothAssetPool, "Cloth Asset Pool");
        // initPayloadPool(m_GameDataAssetPool, "Game Data Asset Pool");

        // If payload pools exceed header capacity, warn that header slots cap total assets.
        const u64 headerSlotsAvailable = ComputeHeaderCapacity(headerBytesToAllocate);
        if (totalPayloadSlots > headerSlotsAvailable) {
            RAZIX_CORE_WARN("[AssetSystem] Header pool slots ({0}) below total payload capacity ({1}); allocations will cap at header capacity.", headerSlotsAvailable, totalPayloadSlots);
        }
    }

    void RZAssetDB::Shutdown()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        m_AnimationAssetPool.destroy();
        m_AssetRefAssetPool.destroy();
        m_AudioAssetPool.destroy();
        m_CameraAssetPool.destroy();
        m_ClothAssetPool.destroy();
        m_GameDataAssetPool.destroy();
        m_LightAssetPool.destroy();
        m_LuaScriptAssetPool.destroy();
        m_MaterialAssetPool.destroy();
        m_MeshAssetPool.destroy();
        m_PhysicsMaterialAssetPool.destroy();
        m_TextureAssetPool.destroy();
        m_TransformAssetPool.destroy();
        m_VignerePuzzleAssetPool.destroy();
        m_HeaderPool.destroy();

        rz_critical_section_destroy(&m_AssetDBLock);
    }

}    // namespace Razix
