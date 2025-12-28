// clang-format off
#include "rzxpch.h"
#include <AssetSystem/RZAssetBase.h>
#include <Core/Memory/RZMemoryFunctions.h>
// clang-format on
#include "RZAssetPool.h"

namespace Razix {

    void RZAssetHeaderPool::init(u32 capacity)
    {
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

    void RZAssetHeaderPool::destroy()
    {
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
        rz_asset_handle handle = RAZIX_ASSET_INVALID_HANDLE;
        if (m_FreeListHead == UINT32_MAX) {
            // TODO: Resize pool?
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
    }

    const RZAsset* RZAssetHeaderPool::get(rz_asset_handle handle) const
    {
        return getMutablePtr(handle);
    }

    const RZAssetColdData* RZAssetHeaderPool::getColdData(rz_asset_handle handle) const
    {
        return getColdDataMutablePtr(handle);
    }

    RZAsset* RZAssetHeaderPool::getMutablePtr(rz_asset_handle handle) const
    {
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
        u32 index = (u32) (handle & RAZIX_ASSET_HOTDATA_MASK);
        RAZIX_CORE_ASSERT(index != RAZIX_ASSET_INVALID_HANDLE, "[AssetSystem] Invalid asset handle");
        if (index >= m_Capacity) {
            RAZIX_CORE_ERROR("[AssetySystem] pool is of capacity {}", m_Capacity);
            return NULL;
        }
        return &m_ColdData[index];
    }

    //----------------------------------------------------------------------------
    // Asset pool
    //----------------------------------------------------------------------------

}    // namespace Razix
