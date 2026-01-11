// clang-format off
#include "rzxpch.h"
#include <AssetSystem/RZAssetBase.h>
// clang-format on

#include "RZAssetDB.h"

#include "Razix/Core/RZThreadCore.h"
#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"
#include "Razix/Core/Profiling/RZProfiling.h"

#include <type_traits>

namespace Razix {

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
        const u64 payloadSize = GetAssetPayloadSize<AssetT>();
        RAZIX_CORE_ASSERT(payloadSize > 0, "[AssetSystem] Invalid slot size while computing capacity.");

        // Account for freelist storage that lives in the same slice: one u32 per slot.
        const u64 slotBytes = payloadSize + sizeof(u32);

        u32 capacity = static_cast<u32>(sliceBytes / slotBytes);
        if (capacity == 0)
            capacity = 1;

        return capacity;
    }

    template<typename AssetT>
    void InitPoolWithSlice(RZAssetPool<AssetT>& pool, Memory::RZHeapAllocator& allocator, const char* label, u64 sliceBytes)
    {
        const u32 capacity       = ComputeCapacityForSlice<AssetT>(sliceBytes);
        const u64 payloadSize    = GetAssetPayloadSize<AssetT>() * capacity;
        const u64 freelistSize   = sizeof(u32) * capacity;
        const u64 allocationSize = payloadSize + freelistSize;

        void* memory = allocator.allocate(allocationSize);
        RAZIX_CORE_ASSERT(memory != NULL, "[AssetSystem] Failed to allocate memory for {}", label);

        RZAssetPoolConfig config = {};
        config.poolType          = Memory::RZ_MEM_POOL_TYPE_ASSET_POOL;
        config.debugLabel        = label;
        config.hooks.onAllocate  = OnAssetAllocate;
        config.hooks.onRelease   = OnAssetRelease;

        pool.init(memory, capacity, config);

        RAZIX_CORE_INFO("[AssetSystem][{}] Initialized | capacity: {} | memory slice: {} bytes", label, capacity, allocationSize);
    }

    static u32 ComputeHeaderCapacity(u64 sliceBytes)
    {
        const u64 slotBytes = sizeof(RZAsset) + sizeof(RZAssetColdData) + sizeof(u32);
        RAZIX_CORE_ASSERT(slotBytes > 0, "[AssetSystem] Invalid header slot size while computing capacity.");

        u32 capacity = static_cast<u32>(sliceBytes / slotBytes);
        if (capacity == 0)
            capacity = 1;

        return capacity;
    }

    static void InitHeaderPoolWithSlice(RZAssetHeaderPool& pool, Memory::RZHeapAllocator& allocator, u64 sliceBytes)
    {
        const u32 capacity       = ComputeHeaderCapacity(sliceBytes);
        const u64 slotBytes      = sizeof(RZAsset) + sizeof(RZAssetColdData) + sizeof(u32);
        const u64 allocationSize = slotBytes * capacity;

        void* memory = allocator.allocate(allocationSize);
        RAZIX_CORE_ASSERT(memory != NULL, "[AssetSystem] Failed to allocate memory for Header Asset Pool");

        pool.init(memory, capacity);

        RAZIX_CORE_INFO("[AssetSystem][Header Asset Pool] Initialized | capacity: {} | memory slice: {} bytes", capacity, allocationSize);
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

        // Reserve a slice up front for the header pool; split the rest across payload pools.
        const u64 headerReserveBytes = budgetInBytes / (numPayloadPools + 1);
        u64       bytesRemaining     = budgetInBytes - headerReserveBytes;
        u32       poolsRemaining     = numPayloadPools;

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

        // Size header pool to cover all payload slots, within the reserved budget (plus any leftover from slicing).
        const u64 headerSlotBytes       = sizeof(RZAsset) + sizeof(RZAssetColdData) + sizeof(u32);
        const u64 headerBytesNeeded     = headerSlotBytes * totalPayloadSlots;
        const u64 headerBytesAvailable  = headerReserveBytes + bytesRemaining;
        const u64 headerBytesToAllocate = std::min(headerBytesNeeded, headerBytesAvailable);
        RAZIX_CORE_ASSERT(headerBytesAvailable > 0, "[AssetSystem] Header pool reserved zero bytes; increase asset budget.");
        RAZIX_CORE_ASSERT(headerBytesToAllocate >= headerSlotBytes, "[AssetSystem] Header pool allocation below one slot ({} bytes available).", headerBytesAvailable);
        if (headerBytesToAllocate < headerBytesNeeded) {
            RAZIX_CORE_WARN("[AssetSystem] Header pool budget ({0} bytes) below required ({1} bytes); header capacity will be capped.", headerBytesAvailable, headerBytesNeeded);
        }

        InitHeaderPoolWithSlice(m_HeaderPool, assetAllocator, headerBytesToAllocate);
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
