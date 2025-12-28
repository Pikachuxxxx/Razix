#pragma once

#include "Razix/AssetSystem/RZAssetBase.h"

namespace Razix {

    /**
     * A pool for storing RZAsset headers (hot data) and cold data.
     * This pool manages the memory for RZAsset instances and their associated cold data.
     * It uses a free list to manage available slots. Same as RZResourceManager class.
     */
    class RAZIX_API RZAssetHeaderPool
    {
    public:
        RZAssetHeaderPool()  = default;
        ~RZAssetHeaderPool() = default;

        void init(u32 capacity);
        void destroy();

        rz_asset_handle  allocate(RZAssetType type);
        void             release(rz_asset_handle handle);
        RZAsset*         get(rz_asset_handle handle);
        RZAssetColdData* getColdData(rz_asset_handle handle);

        u32 getCapacity() const { return m_Capacity; }
        u32 getCount() const { return m_Count; }

    private:
        RZAsset*         m_Assets       = NULL;
        RZAssetColdData* m_ColdData     = NULL;
        u32*             m_FreeList     = NULL;
        u32              m_Capacity     = 0;
        u32              m_Count        = 0;
        u32              m_FreeListHead = 0;
    };

    class RAZIX_API RZAssetPoolBase
    {
    public:
        virtual ~RZAssetPoolBase() = default;
        virtual void destroy()     = 0;
    };

    template<typename T>
    class RZAssetPool : public RZAssetPoolBase
    {
    public:
        RZAssetPool() = default;
        virtual ~RZAssetPool() { destroy(); }

        /**
         * Initializes the pool with a given capacity.
         * @param capacity The maximum number of assets the pool can hold.
         */
        void init(u32 capacity)
        {
            m_Capacity     = capacity;
            m_Data         = new T[m_Capacity];
            m_FreeList     = new u32[m_Capacity];
            m_Count        = 0;
            m_FreeListHead = 0;

            for (u32 i = 0; i < m_Capacity; ++i) {
                m_FreeList[i] = i + 1;
            }
            m_FreeList[m_Capacity - 1] = UINT32_MAX;    // End of list
        }

        /**
         * Destroys the pool and frees all allocated memory.
         */
        void destroy() override
        {
            if (m_Data) {
                delete[] m_Data;
                m_Data = NULL;
            }
            if (m_FreeList) {
                delete[] m_FreeList;
                m_FreeList = NULL;
            }
            m_Capacity     = 0;
            m_Count        = 0;
            m_FreeListHead = 0;
        }

        /**
         * Allocates a new asset payload in the pool.
         * @param index The index of the allocated payload (output).
         * @return A pointer to the newly allocated payload.
         */
        T* allocate(u32& index)
        {
            if (m_FreeListHead == UINT32_MAX) {
                // Pool is full
                // TODO: Resize pool?
                return NULL;
            }

            index          = m_FreeListHead;
            m_FreeListHead = m_FreeList[index];
            m_Count++;

            // Initialize the handle in the payload header if it exists
            // We assume T has RAZIX_ASSET_PAYLOAD_HEADER macro which defines 'handle'
            // But we can't set the full handle here because we don't know the hot/cold index yet
            // The caller (RZAssetDatabase or similar) should set the handle properly.

            return &m_Data[index];
        }

        /**
         * Frees an asset payload in the pool.
         * @param index The index of the payload to free.
         */
        void release(u32 index)
        {
            if (index >= m_Capacity) {
                // Invalid index
                return;
            }

            // Call destructor manually if needed, but for POD types or simple structs it might not be necessary
            // However, for types with resources, we should probably call a destroy method or destructor
            // Since we used new T[], the default constructor was called.
            // We can re-use the memory.
            // If T has a destructor that needs to run, we should probably use placement new/delete or similar strategy
            // For now, we assume the user handles cleanup before release or T is simple enough.
            // Actually, to be safe, we might want to reset the memory or call a reset function.

            m_FreeList[index] = m_FreeListHead;
            m_FreeListHead    = index;
            m_Count--;
        }

        /**
         * Gets a pointer to an asset payload given its index.
         * @param index The index of the payload.
         * @return A pointer to the payload, or NULL if the index is invalid.
         */
        T* get(u32 index)
        {
            if (index >= m_Capacity) {
                return NULL;
            }
            return &m_Data[index];
        }

        u32 getCapacity() const { return m_Capacity; }
        u32 getCount() const { return m_Count; }

    private:
        T*   m_Data         = NULL;
        u32* m_FreeList     = NULL;
        u32  m_Capacity     = 0;
        u32  m_Count        = 0;
        u32  m_FreeListHead = 0;
    };

}    // namespace Razix
