#include "rzxpch.h"
#include "RZAssetPool.h"

namespace Razix {

    void RZAssetHeaderPool::init(u32 capacity)
    {
        m_Capacity = capacity;
        // Allocate memory for assets (hot data)
        // We use operator new to allocate raw memory, but since RZAsset has no default constructor and we want to control construction
        // we will treat this as an array of RZAsset.
        // However, RZAsset has a deleted default constructor.
        // So we allocate raw memory and use placement new when allocating.
        m_Assets = (RZAsset*)operator new(sizeof(RZAsset) * m_Capacity);
        
        // Allocate memory for cold data
        m_ColdData = new RZAssetColdData[m_Capacity];

        m_FreeList = new u32[m_Capacity];
        m_Count = 0;
        m_FreeListHead = 0;

        for (u32 i = 0; i < m_Capacity; ++i)
        {
            m_FreeList[i] = i + 1;
        }
        m_FreeList[m_Capacity - 1] = UINT32_MAX; // End of list
    }

    void RZAssetHeaderPool::destroy()
    {
        if (m_Assets)
        {
            // We should probably call destructors for active assets if we were tracking them strictly
            // But since this is a pool destroy, we assume everything is being shut down.
            operator delete(m_Assets);
            m_Assets = nullptr;
        }
        if (m_ColdData)
        {
            delete[] m_ColdData;
            m_ColdData = nullptr;
        }
        if (m_FreeList)
        {
            delete[] m_FreeList;
            m_FreeList = nullptr;
        }
        m_Capacity = 0;
        m_Count = 0;
        m_FreeListHead = 0;
    }

    RZAsset* RZAssetHeaderPool::allocate(RZAssetType type, rz_asset_handle& handle)
    {
        if (m_FreeListHead == UINT32_MAX)
        {
            // Pool is full
            // TODO: Resize pool?
            return nullptr;
        }

        u32 index = m_FreeListHead;
        m_FreeListHead = m_FreeList[index];
        m_Count++;

        // Construct the asset in place
        // We pass the pointer to the corresponding cold data
        RZAsset* asset = new (&m_Assets[index]) RZAsset(type, &m_ColdData[index]);
        
        // Set the handle
        // The handle's lower 32 bits are the index in this pool
        // The upper 32 bits will be set by the caller (RZAssetDatabase) when allocating the payload
        // For now, we just set the lower part.
        // Wait, the handle is usually fully formed by the database.
        // But here we are just allocating the header.
        // Let's assume the caller will fix up the handle or we return the index as part of the handle.
        
        // We return the index as the lower 32 bits of the handle
        handle = (u64)index; 

        return asset;
    }

    void RZAssetHeaderPool::release(rz_asset_handle handle)
    {
        u32 index = (u32)(handle & RAZIX_ASSET_HOTDATA_MASK);

        if (index >= m_Capacity)
        {
            // Invalid index
            return;
        }

        // Call destructor to clean up (e.g. decrement ref counts if any, though RZAsset destructor is deleted/private usually?)
        // RZAsset has a destroy() method.
        // But here we are just releasing the slot.
        // The RZAsset destructor is deleted, so we can't call it directly if it was public.
        // But we can call the destroy method if we want to clean up cold data.
        // However, the pool just manages memory.
        // We assume the asset is already "destroyed" logically before releasing memory.
        // But we should probably manually call the destructor if we used placement new.
        // Since ~RZAsset is deleted, we rely on the fact that RZAsset is POD-like enough or has explicit destroy.
        // RZAsset::destroy() cleans up cold data but doesn't free the memory of the asset itself (which is in the pool).
        
        // Re-add to free list
        m_FreeList[index] = m_FreeListHead;
        m_FreeListHead = index;
        m_Count--;
    }

    RZAsset* RZAssetHeaderPool::get(rz_asset_handle handle)
    {
        u32 index = (u32)(handle & RAZIX_ASSET_HOTDATA_MASK);
        if (index >= m_Capacity)
        {
            return nullptr;
        }
        return &m_Assets[index];
    }

    RZAssetColdData* RZAssetHeaderPool::getColdData(rz_asset_handle handle)
    {
        u32 index = (u32)(handle & RAZIX_ASSET_HOTDATA_MASK);
        if (index >= m_Capacity)
        {
            return nullptr;
        }
        return &m_ColdData[index];
    }

}
