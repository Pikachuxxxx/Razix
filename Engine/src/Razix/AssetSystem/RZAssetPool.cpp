// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAssetPool.h"

namespace Razix {

    u32 GetDepartmentPoolCapacity(const RZAssetPoolConfig& config, u32 slotSize)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        using namespace Razix::Memory;
        BudgetInfo deptBudgetInfo = GetDepartmentBudgetInfo(config.department);
        if (deptBudgetInfo.MemoryBudget == 0)
            return RAZIX_ASSETPOOL_DEFAULT_CAPACITY;
        // Convert MB to Bytes
        u64 deptMemoryBudgetBytes = Mib(static_cast<u64>(deptBudgetInfo.MemoryBudget));
        u32 capacity              = static_cast<u32>(deptMemoryBudgetBytes / slotSize);
        RAZIX_CORE_INFO("[AssetSystem] Department: {} - {} | Memory Budget: {} MB | Slot Size: {} bytes | Calculated Capacity: {}",
            s_DepartmentInfo[config.department].debugName,
            s_DepartmentInfo[config.department].friendlyName,
            deptBudgetInfo.MemoryBudget,
            slotSize,
            capacity);

        RAZIX_CORE_ASSERT(capacity > 0, "[AssetSystem] Department: {} - {} has insufficient memory budget: {} MB for asset pool of slot size: {} bytes", s_DepartmentInfo[config.department].debugName, s_DepartmentInfo[config.department].friendlyName, deptBudgetInfo.MemoryBudget, slotSize);

        return capacity;
    }

    u32 ClampDepartmentPoolCapacity(const RZAssetPoolConfig& config, u32 desiredCapacity, u32 slotSize)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        u32 deptCapacity = GetDepartmentPoolCapacity(config, slotSize);
        if (desiredCapacity > deptCapacity) {
            RAZIX_CORE_WARN("[AssetSystem] Department: {} - {} | Desired Capacity: {} exceeds calculated capacity: {} based on memory budget. Clamping to calculated capacity.",
                s_DepartmentInfo[config.department].debugName,
                s_DepartmentInfo[config.department].friendlyName,
                desiredCapacity,
                deptCapacity);
            return deptCapacity;
        }

        RAZIX_CORE_INFO("[AssetSystem] Department: {} - {} | Desired Capacity: {} is within calculated capacity: {} based on memory budget.",
            s_DepartmentInfo[config.department].debugName,
            s_DepartmentInfo[config.department].friendlyName,
            desiredCapacity,
            deptCapacity);

        RAZIX_CORE_ASSERT(desiredCapacity > 0, "[AssetSystem] Department: {} - {} has insufficient desired capacity: {} for asset pool of slot size: {} bytes", s_DepartmentInfo[config.department].debugName, s_DepartmentInfo[config.department].friendlyName, desiredCapacity, slotSize);

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

        ptr        = (u8*) rz_align_ptr(ptr, alignof(RZAssetColdData));
        m_ColdData = reinterpret_cast<RZAssetColdData*>(ptr);
        ptr += sizeof(RZAssetColdData) * capacity;

        ptr        = (u8*) rz_align_ptr(ptr, alignof(u32));
        m_FreeList = reinterpret_cast<u32*>(ptr);

        m_Count        = 0;
        m_FreeListHead = 0;
        for (u32 i = 0; i < capacity - 1; ++i)
            m_FreeList[i] = i + 1;

        m_FreeList[capacity - 1] = UINT32_MAX;
    }

    void RZAssetHeaderPool::destroy()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        if (m_Assets)
            rz_free(m_Assets);
        if (m_ColdData)
            rz_free(m_ColdData);
        if (m_FreeList)
            rz_free(m_FreeList);
        m_Capacity     = 0;
        m_Count        = 0;
        m_FreeListHead = 0;
    }

    rz_asset_handle RZAssetHeaderPool::allocate(RZAssetType type)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        rz_asset_handle handle = RAZIX_ASSET_INVALID_HANDLE;
        if (m_FreeListHead == UINT32_MAX) {
            // TODO: Resize pool?
            RAZIX_CORE_ERROR("[AssetSystem] Asset Pool is full! Cannot allocate more assets. Please edit the budget files to adjust meomry budget at engine ingition time. Game budget should always be pre-compute and fixed.");
            return handle;
        }

        u32 index      = m_FreeListHead;
        m_FreeListHead = m_FreeList[index];
        m_Count++;

        // Construct the asset in place
        // We pass the pointer to the corresponding cold data
        RZAsset* asset = new (&m_Assets[index]) RZAsset(type, &m_ColdData[index]);

        // Set the handle
        // The handle's lower 32 bits are the index in this pool
        // The upper 32 bits will be set by the caller (RZAssetDatabase) when allocating the payload
        // For now, we just set the lower part.
        // We return the index as the lower 32 bits of the handle
        handle = (u64) index;
        return handle;
    }

    void RZAssetHeaderPool::release(rz_asset_handle handle)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        u32 index = (u32) (handle & RAZIX_ASSET_HOTDATA_MASK);

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