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

    constexpr bool bCapCapacityToMaxAssets = true;

    constexpr u32 kRZAssetHeaderSlotBytes = sizeof(RZAsset) + sizeof(RZAssetColdData) + sizeof(u32);

    template<typename AssetT>
    constexpr u64 GetAssetPayloadSize()
    {
        if constexpr (std::is_same_v<AssetT, RZTransformAsset>) {
            return sizeof(RZTransformAsset) + sizeof(float4) * 3 + sizeof(float4x4) * 2;
        } else {
            return sizeof(AssetT);
        }
    }

    static constexpr u64 CalculateMaxSlotMemForPayloads()
    {
        // + u32 for freelist pool entry per slot
        return GetAssetPayloadSize<RZTransformAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZMeshAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZMaterialAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZTextureAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZAnimationAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZAudioAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZLuaScriptAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZPhysicsMaterialAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZCameraAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZLightAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZAssetRefAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZVignerePuzzleAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZClothAsset>() + sizeof(u32) +
               GetAssetPayloadSize<RZGameDataAsset>() + sizeof(u32);
    }

    u64 RZAssetDB::ComputeMinPoolBudgetBytesForMaxAssets(u64 maxAssets)
    {
        constexpr u64 maxPayloadSlotBytes = CalculateMaxSlotMemForPayloads();
        RAZIX_CORE_ASSERT(maxPayloadSlotBytes > 0, "[AssetSystem] Invalid slot sizing for pool budget computation.");
        return maxAssets * (maxPayloadSlotBytes);
    }

    u64 RZAssetDB::ComputeMinHeaderBudgetBytesForMaxAssets(u64 maxAssets)
    {
        RAZIX_CORE_ASSERT(kRZAssetHeaderSlotBytes > 0, "[AssetSystem] Invalid slot sizing for header budget computation.");
        u64 raw_size = rz_mem_align(maxAssets * kRZAssetHeaderSlotBytes, RAZIX_CACHE_LINE_ALIGN);
        u64 power2Size = rz_next_power_of_two(raw_size);

        RAZIX_CORE_TRACE("[AssetSystem] Rounded to next power of 2: {} bytes ({} MB / {} GB)",
            power2Size,
            in_Mib(power2Size),
            in_Gib(power2Size));

        return power2Size;
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

    static u32 CalculateHeaderPoolCapacityForMemSlice(u64 sliceBytes)
    {
        u32 capacity = static_cast<u32>(sliceBytes / kRZAssetHeaderSlotBytes);
        if (capacity == 0)
            capacity = 1;

        while (capacity > 0 && (static_cast<u64>(kRZAssetHeaderSlotBytes * capacity)) > sliceBytes)
            --capacity;

        RAZIX_CORE_ASSERT(capacity > 0, "[AssetSystem] Invalid header capacity for slice: {} bytes", sliceBytes);
        return capacity;
    }

    static void InitHeaderPoolWithSlice(RZAssetHeaderPool& pool, Memory::RZHeapAllocator& allocator, u64 sliceBytes)
    {
        const u32 capacity = CalculateHeaderPoolCapacityForMemSlice(sliceBytes);
        RAZIX_CORE_ASSERT(capacity == RAZIX_MAX_ASSETS, "[AssetSystem] HeaderPool compute capacity doesn't match slice bytes capacity");
        u64 allocationSize = kRZAssetHeaderSlotBytes * capacity;

        void* memory = allocator.allocate(allocationSize);
        RAZIX_CORE_ASSERT(memory != NULL, "[AssetSystem] Failed to allocate memory for Header Asset Pool");

        pool.init(memory, capacity);

        RAZIX_CORE_INFO("[AssetSystem] [Header Asset Pool] Initialized | capacity: {0} | memory slice: {1} KiB", capacity, in_Kib(allocationSize));
    }

    template<typename AssetT>
    u32 CalculatePayloadPoolCapacityForMemSlice(u64 sliceBytes, bool capToMaxAssets)
    {
        constexpr u64 slotBytes = GetAssetPayloadSize<AssetT>() + sizeof(u32);    // payload + freelist entry per slot
        RAZIX_CORE_ASSERT(slotBytes > 0, "[AssetSystem] Invalid slot size while computing capacity.");

        u32 capacity = static_cast<u32>(sliceBytes / slotBytes);
        if (capacity == 0)
            capacity = 1;

        if (capToMaxAssets) {
            const u32 maxAssetsCap = static_cast<u32>(RAZIX_MAX_ASSETS);
            // RAZIX_CORE_TRACE("[AssetSystem] Calculated capacity: {} for slice: {} bytes and slot size: {} bytes", capacity, sliceBytes, slotBytes);
            RAZIX_CORE_ASSERT(capacity >= maxAssetsCap, "[AssetSystem] Pool capacity {} below RAZIX_MAX_ASSETS {}. Increase asset pool budget or reduce pool count.", capacity, maxAssetsCap);
            capacity = maxAssetsCap;
        }

        return capacity;
    }

    template<typename AssetT>
    void InitPoolWithSlice(RZAssetPool<AssetT>& pool, Memory::RZHeapAllocator& allocator, const char* label, u64 sliceBytes)
    {
        constexpr u64 slotBytes      = GetAssetPayloadSize<AssetT>() + sizeof(u32);
        const u32     capacity       = CalculatePayloadPoolCapacityForMemSlice<AssetT>(sliceBytes, bCapCapacityToMaxAssets);
        const u64     allocationSize = slotBytes * capacity;
        RAZIX_CORE_ASSERT(allocationSize <= sliceBytes, "[AssetSystem] Allocation size exceeds slice for {}", label);

        void* memory = allocator.allocate(allocationSize);
        RAZIX_CORE_ASSERT(memory != NULL, "[AssetSystem] Failed to allocate memory for {}", label);

        RZAssetPoolConfig config = {};
        config.poolType          = Memory::RZ_MEM_POOL_TYPE_ASSET_POOL;
        config.debugLabel        = label;
        config.hooks.onAllocate  = OnAssetAllocate;
        config.hooks.onRelease   = OnAssetRelease;

        pool.init(memory, capacity, config);

        RAZIX_CORE_INFO("[AssetSystem] [{}] Initialized | capacity: {} | memory slice: {} KiB", label, capacity, in_Kib(allocationSize));
    }

    //-------------------------------------------------------------------------

    void RZAssetDB::Startup(Memory::RZHeapAllocator& assetAllocator, Memory::RZHeapAllocator& assetHeaderAllocator)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        m_AssetAllocator       = &assetAllocator;
        m_AssetHeaderAllocator = &assetHeaderAllocator;

        m_AssetDBLock = rz_critical_section_create();

        const Memory::MemoryPoolBudget assetBudget = Memory::GetMemoryPoolBudget(Memory::RZ_MEM_POOL_TYPE_ASSET_POOL);
        m_BudgetInMB                               = assetBudget.HeapSizeMB;

        const u64 budgetInBytes = Mib(static_cast<u64>(assetBudget.HeapSizeMB));
        RAZIX_CORE_ASSERT(budgetInBytes > 0, "[AssetSystem] Asset pool budget is 0 bytes!");

        const u32 numPayloadPools = static_cast<u32>(RZAssetType::COUNT);
        RAZIX_CORE_ASSERT(numPayloadPools > 0, "[AssetSystem] No asset pools configured!");

        const u64 headerSlotBytes       = sizeof(RZAsset) + sizeof(RZAssetColdData) + sizeof(u32);
        const u64 maxAssetsBudget       = static_cast<u64>(RAZIX_MAX_ASSETS);
        u64       headerBytesToAllocate = headerSlotBytes * maxAssetsBudget;
        if (headerBytesToAllocate > budgetInBytes)
            headerBytesToAllocate = budgetInBytes / (numPayloadPools + 1);    // fallback to an even slice if over budget, highly unlikely

        RAZIX_CORE_ASSERT(headerBytesToAllocate > 0, "[AssetSystem] Header pool reserved zero bytes; increase asset budget.");
        RAZIX_CORE_ASSERT(headerBytesToAllocate >= headerSlotBytes, "[AssetSystem] Header pool allocation below one slot ({} bytes available).", headerBytesToAllocate);
        InitHeaderPoolWithSlice(m_HeaderPool, assetHeaderAllocator, headerBytesToAllocate);

        u64 bytesRemaining = budgetInBytes - headerBytesToAllocate;
        u32 poolsRemaining = numPayloadPools;

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
            const u32 capacity = CalculatePayloadPoolCapacityForMemSlice<AssetType>(slice, bCapCapacityToMaxAssets);
            RAZIX_CORE_ASSERT(capacity > 0, "[AssetSystem] Computed zero capacity for {}", label);
            totalPayloadSlots += capacity;
            RAZIX_CORE_INFO("[AssetSystem] Payload slice for {}: slice={} KiB, remaining={} KiB, capacity={}", label, in_Kib(slice), in_Kib(bytesRemaining), capacity);
            InitPoolWithSlice<AssetType>(pool, assetAllocator, label, slice);
        };

        initPayloadPool(m_TransformAssetPool, "Transform Asset Pool");
        initPayloadPool(m_CameraAssetPool, "Camera Asset Pool");
        initPayloadPool(m_LightAssetPool, "Light Asset Pool");
        initPayloadPool(m_MaterialAssetPool, "Material Asset Pool");
        initPayloadPool(m_PhysicsMaterialAssetPool, "Physics Material Asset Pool");
        initPayloadPool(m_MeshAssetPool, "Mesh Asset Pool");
        initPayloadPool(m_TextureAssetPool, "Texture Asset Pool");
        initPayloadPool(m_AnimationAssetPool, "Animation Asset Pool");
        initPayloadPool(m_AudioAssetPool, "Audio Asset Pool");
        initPayloadPool(m_LuaScriptAssetPool, "Lua Script Asset Pool");
        initPayloadPool(m_AssetRefAssetPool, "Asset Ref Pool");
        initPayloadPool(m_VignerePuzzleAssetPool, "Vignere Puzzle Asset Pool");
        initPayloadPool(m_ClothAssetPool, "Cloth Asset Pool");
        initPayloadPool(m_GameDataAssetPool, "Game Data Asset Pool");

        RAZIX_CORE_TRACE("[AssetSystem] Asset Pools memory usage: Header Pool: {0} KiB | Payload Pools: {1} KiB | Total Budget: {2} KiB | Leftover Budget: {3}",
            in_Kib(headerBytesToAllocate),
            in_Kib(totalPayloadSlots),
            in_Kib(budgetInBytes),
            in_Kib(budgetInBytes - headerBytesToAllocate - totalPayloadSlots));

        // If payload pools exceed header capacity, warn that header slots cap total assets to allocate.
        const u64 headerSlotsAvailable = CalculateHeaderPoolCapacityForMemSlice(headerBytesToAllocate);
        if (totalPayloadSlots > headerSlotsAvailable) {
            RAZIX_CORE_WARN("[AssetSystem] Header pool slots ({0}) below total payload capacity ({1}); allocations will cap at header capacity.", headerSlotsAvailable, totalPayloadSlots);
        }
    }

    void RZAssetDB::Shutdown()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        m_AssetHeaderAllocator->deallocate(m_HeaderPool.getBackingMemoryMutablePtr());

        m_AssetAllocator->deallocate(m_AnimationAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_AssetRefAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_AudioAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_CameraAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_ClothAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_GameDataAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_LightAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_LuaScriptAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_MaterialAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_MeshAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_PhysicsMaterialAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_TextureAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_TransformAssetPool.getBackingMemoryMutablePtr());
        m_AssetAllocator->deallocate(m_VignerePuzzleAssetPool.getBackingMemoryMutablePtr());

        m_HeaderPool.destroy();
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

        rz_critical_section_destroy(&m_AssetDBLock);
    }

}    // namespace Razix
