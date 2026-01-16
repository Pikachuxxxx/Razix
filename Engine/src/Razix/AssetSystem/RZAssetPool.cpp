// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAssetPool.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

namespace Razix {

    // FIXME: We compute capacity for the rest of the pool memory, what about other pools?
    // FIXME: I think we need to divide into slices and use slice to get capacity instead. Not use the whole pool
    u32 GetAssetPoolCapacityFromMemoryBudget(Memory::RZMemoryPoolType poolType, u32 slotSize)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        Memory::MemoryPoolBudget poolBudget = GetMemoryPoolBudget(poolType);

        if (poolBudget.HeapSizeMB == 0)
            return RAZIX_ASSETPOOL_DEFAULT_CAPACITY;

        // Convert MB to bytes
        u64 poolMemoryBudgetBytes = Mib(static_cast<u64>(poolBudget.HeapSizeMB));
        u32 capacity              = static_cast<u32>(poolMemoryBudgetBytes / slotSize);

        RAZIX_CORE_ASSERT(capacity > 0, "[AssetSystem] Pool: {0} has insufficient memory budget: {1} MB for asset pool of slot size: {2} bytes", poolBudget.PoolName, poolBudget.HeapSizeMB, slotSize);

        return capacity;
    }

    u32 ClampAssetPoolCapacity(Memory::RZMemoryPoolType poolType, u32 desiredCapacity, u32 slotSize)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        u32 maxCapacity = GetAssetPoolCapacityFromMemoryBudget(poolType, slotSize);

        if (desiredCapacity > maxCapacity) {
            Memory::MemoryPoolBudget poolBudget = GetMemoryPoolBudget(poolType);
            RAZIX_CORE_WARN("[AssetSystem] Pool: {0} | Desired Capacity:  {1} exceeds calculated capacity:  {2} based on memory budget.  Clamping to {2}.",
                poolBudget.PoolName,
                desiredCapacity,
                maxCapacity);
            return maxCapacity;
        }

        RAZIX_CORE_ASSERT(desiredCapacity > 0, "[AssetSystem] Desired capacity must be greater than 0");

        return desiredCapacity;
    }

    //------------------------------------------------------------------------------

    void RZAssetHeaderPool::init(u32 capacity)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        RAZIX_CORE_INFO("[AssetSystem] Initializing Asset Header Pool with capacity: {}", capacity);
        m_Capacity     = capacity;
        m_Assets       = static_cast<RZAsset*>(rz_malloc_aligned(sizeof(RZAsset) * m_Capacity));
        m_ColdData     = static_cast<RZAssetColdData*>(rz_malloc_aligned(sizeof(RZAssetColdData) * m_Capacity));
        m_FreeList     = static_cast<u32*>(rz_malloc_aligned(sizeof(u32) * m_Capacity));
        m_Count        = 0;
        m_FreeListHead = 0;

        for (u32 i = 0; i < m_Capacity; ++i)
            m_FreeList[i] = i + 1;
        m_FreeList[m_Capacity - 1] = UINT32_MAX;    // End of list
    }

    void RZAssetHeaderPool::init(void* where, u32 capacity)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        m_Capacity = capacity;
        u8* ptr    = static_cast<u8*>(where);

        m_Assets = reinterpret_cast<RZAsset*>(ptr);
        ptr += sizeof(RZAsset) * capacity;

        m_ColdData = reinterpret_cast<RZAssetColdData*>(ptr);
        ptr += sizeof(RZAssetColdData) * capacity;

        m_FreeList = reinterpret_cast<u32*>(ptr);

        m_Count        = 0;
        m_FreeListHead = 0;
        for (u32 i = 0; i < capacity - 1; ++i)
            m_FreeList[i] = i + 1;

        m_FreeList[capacity - 1] = UINT32_MAX;

        m_ExternalMemory = true;
    }

    void RZAssetHeaderPool::destroy()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        if (m_Assets && !m_ExternalMemory)
            rz_free(m_Assets);
        if (m_ColdData && !m_ExternalMemory)
            rz_free(m_ColdData);
        if (m_FreeList && !m_ExternalMemory)
            rz_free(m_FreeList);
        m_Capacity     = 0;
        m_Count        = 0;
        m_FreeListHead = 0;
    }

    u32 RZAssetHeaderPool::allocate(RZAssetType type)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        if (m_FreeListHead == UINT32_MAX) {
            // TODO: Resize pool?
            RAZIX_CORE_ERROR("[AssetSystem] Asset Pool is full! Cannot allocate more assets. Please edit the budget files to adjust meomry budget at engine ingition time. Game budget should always be pre-compute and fixed.");
            return RAZIX_ASSET_INVALID_HANDLE;
        }

        u32 index      = m_FreeListHead;
        m_FreeListHead = m_FreeList[index];
        m_Count++;

        // Construct the asset in place
        // We pass the pointer to the corresponding cold data
        RZAsset* asset = new (&m_Assets[index]) RZAsset(type, &m_ColdData[index]);
        RAZIX_UNUSED(asset);

        // Set the handle
        // The handle's lower 32 bits are the index in this pool
        // The upper 32 bits will be set by the caller (RZAssetDatabase) when allocating the payload
        // For now, we just set the lower part.
        // We return the index as the lower 32 bits of the handle
        return index;
    }

    void RZAssetHeaderPool::release(u32 index)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        if (index >= m_Capacity) {
            RAZIX_CORE_ERROR("[AssetSystem] Invalid asset handle index {}", index);
            return;
        }

        // TODO: Properly destroy cold data members if needed
        // RZAssetColdData* pColdData = &m_ColdData[index];
        RZAsset* pAsset = &m_Assets[index];
        pAsset->destroy();

        m_FreeList[index] = m_FreeListHead;
        m_FreeListHead    = index;
        m_Count--;

        rz_poison_memory(&m_Assets[index], sizeof(RZAsset));
        rz_poison_memory(&m_ColdData[index], sizeof(RZAssetColdData));
    }

    const RZAsset* RZAssetHeaderPool::get(rz_asset_handle handle) const
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        return getMutablePtr(handle);
    }

    const RZAssetColdData* RZAssetHeaderPool::getColdData(rz_asset_handle handle) const
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        return getColdDataMutablePtr(handle);
    }

    RZAsset* RZAssetHeaderPool::getMutablePtr(rz_asset_handle handle) const
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        u32 index = (u32) (handle & RAZIX_ASSET_HOTDATA_MASK);
        RAZIX_CORE_ASSERT(index != RAZIX_ASSET_INVALID_HANDLE, "[AssetSystem] Invalid asset handle");
        if (index >= m_Capacity) {
            RAZIX_CORE_ERROR("[AssetySystem] pool is of capacity {}", m_Capacity);
            return NULL;
        }
        return &m_Assets[index];
    }

    RZAssetColdData* RZAssetHeaderPool::getColdDataMutablePtr(rz_asset_handle handle) const
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        u32 index = (u32) (handle & RAZIX_ASSET_HOTDATA_MASK);
        RAZIX_CORE_ASSERT(index != RAZIX_ASSET_INVALID_HANDLE, "[AssetSystem] Invalid asset handle");
        if (index >= m_Capacity) {
            RAZIX_CORE_ERROR("[AssetySystem] pool is of capacity {}", m_Capacity);
            return NULL;
        }
        return &m_ColdData[index];
    }
}    // namespace Razix
