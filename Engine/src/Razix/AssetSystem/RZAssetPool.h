#pragma once

#include "Razix/AssetSystem/RZAssetBase.h"

namespace Razix {

    // TODO: debugging hooks
    // TODO: profiling hooks
    // TODO: Fixed budget enforcement (skip resizing delibretaly) use RZDepartments properly here
    // TODO: pagefault/memory safety hooks --> allocated by pages etc.?
    // TODO: provide memory in a consumed way

    // Why not re-use RZResourceFreeListmemPool?
    // Because we need to pack 2 32-bit handles and rz_handle actually tracks generation index
    // We don't need that here, unlike GPU resource handles asset cannot be sale or idk doesn't make sense to me here
    // so we use a internal simple freelist as-is in place with consumed bool tracking/ofc size/8 to reduce memory.
    //
    // Why not reuse RZResourceFreeListmemPool with rz_handle (u64: 32-bit index + 32-bit generation)?
    // GPU resources (e.g., textures) need generations to prevent stale handles in async Gfx paths.
    // Ex: Alloc TexA (handle=0x00000000_00000000), release (slot 0 free), reuse for TexB (0x00000000_00000001).
    // If renderer queue has stale A, gen mismatch (0 !=1) fails safely → no UAF/corruption.
    // Assets are sync/long-lived (load once, hold till unload), low churn—stales are rare bugs, not races.
    // Gens would bloat (u64 vs u32) + add checks w/o perf gain here. Simple u32 freelist wins.
    // ASCII ex (Gfx pool, 4 slots; arrows= freelist links):
    // Time 0: Empty
    // +----------+     +----------+     +----------+     +----------+
    // | Slot 0   | --> | Slot 1   | --> | Slot 2   | --> | Slot 3   | --> NULL
    // | Gen=0    |     | Gen=0    |     | Gen=0    |     | Gen=0    |
    // | Data: -- |     | Data: -- |     | Data: -- |     | Data: -- |
    // +----------+     +----------+     +----------+     +----------+
    // Head -->0; No handles.
    // Time1: Alloc A (0x...00) [Async upload queued]
    // +----------+     +----------+     +----------+     +----------+
    // | Slot 0   | --> | Slot 1   | --> | Slot 2   | --> | Slot 3   | --> NULL
    // | Gen=0    |     | Gen=0    |     | Gen=0    |     | Gen=0    |
    // | Data: A  |     | Data: -- |     | Data: -- |     | Data: -- | <-- Renderer uses A
    // +----------+     +----------+     +----------+     +----------+
    // Head-->1; A valid.
    // Time3: Release A [Cmd pending!]
    // +----------+     +----------+     +----------+     +----------+
    // | Slot 0   | --> | Slot 1   | --> | Slot 2   | --> | Slot 3   | --> NULL
    // | Gen=0    |     | Gen=0    |     | Gen=0    |     | Gen=0    |
    // | Data: -- |     | Data: -- |     | Data: -- |     | Data: -- |
    // +----------+     +----------+     +----------+     +----------+
    // Head-->0; Stale A (0x...00) dangles.
    // Time4: Alloc C (0x...01)
    // +----------+     +----------+     +----------+     +----------+
    // | Slot 0   | --> | Slot 1   | --> | Slot 2   | --> | Slot 3   | --> NULL
    // | Gen=1    |     | Gen=0    |     | Gen=0    |     | Gen=0    |
    // | Data: C  |     | Data: -- |     | Data: -- |     | Data: -- |
    // +----------+     +----------+     +----------+     +----------+
    // Head-->1; C valid. Stale A: idx=0,gen=0!=1 → Blocked! (No gen: Binds C to A call → glitch/crash)

    /**
     * A pool for storing RZAsset headers (hot data) and cold data.
     * This pool manages the memory for RZAsset instances and their associated cold data.
     * It uses a free list to manage available slots. Same as RZResourceManager class design.
     */
    class RAZIX_API RZAssetHeaderPool
    {
    public:
        RZAssetHeaderPool()  = default;
        ~RZAssetHeaderPool() = default;

        void init(u32 capacity);
        void destroy();

        rz_asset_handle        allocate(RZAssetType type);
        void                   release(rz_asset_handle handle);
        const RZAsset*         get(rz_asset_handle handle) const;
        const RZAssetColdData* getColdData(rz_asset_handle handle) const;
        RZAsset*               getMutablePtr(rz_asset_handle handle) const;
        RZAssetColdData*       getColdDataMutablePtr(rz_asset_handle handle) const;

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
    class RZAssetPool final : public RZAssetPoolBase
    {
    public:
        RZAssetPool()          = default;
        virtual ~RZAssetPool() = default;

        void init(u32 capacity)
        {
            m_Capacity     = capacity;
            m_Data         = (T*) rz_malloc(sizeof(T) * m_Capacity, RAZIX_16B_ALIGN);
            m_FreeList     = (u32*) rz_malloc_aligned(sizeof(u32) * m_Capacity);
            m_Count        = 0;
            m_FreeListHead = 0;

            for (u32 i = 0; i < m_Capacity; ++i) {
                m_FreeList[i] = i + 1;
            }
            m_FreeList[m_Capacity - 1] = UINT32_MAX;    // End of list
        }

        void destroy() override
        {
            if (m_Data) {
                rz_free(m_Data);
                m_Data = NULL;
            }
            if (m_FreeList) {
                rz_free(m_FreeList);
                m_FreeList = NULL;
            }
            m_Capacity     = 0;
            m_Count        = 0;
            m_FreeListHead = 0;
        }

        T* allocate(u32& index)
        {
            if (m_FreeListHead == UINT32_MAX) {
                // Pool is full
                // TODO: Resize pool?
                // Actually no dynamic resizing, account for asset memory budget before hand
                return NULL;
            }

            index          = m_FreeListHead;
            m_FreeListHead = m_FreeList[index];
            m_Count++;
            return &m_Data[index];
        }

        void release(u32 index)
        {
            if (index >= m_Capacity)
                return;
            m_FreeList[index] = m_FreeListHead;
            m_FreeListHead    = index;
            m_Count--;
        }

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

    //---------------------------------------------------------------
    // RZAssetPool specialization for RZAssetPool<RZTransformAsset> due to SOA design
    //---------------------------------------------------------------

    struct RZTransformAsset;

    template<>
    class RZAssetPool<RZTransformAsset> : public RZAssetPoolBase
    {
    public:
        RZAssetPool()          = default;
        virtual ~RZAssetPool() = default;

        // TODO: Implement the SOA design specific methods for RZTransformAsset
        void init(u32 capacity)
        {
            m_Capacity      = capacity;
            m_Positions     = (float4*) rz_malloc(sizeof(float4) * m_Capacity, RAZIX_16B_ALIGN);
            m_Rotations     = (float4*) rz_malloc(sizeof(float4) * m_Capacity, RAZIX_16B_ALIGN);
            m_Scales        = (float4*) rz_malloc(sizeof(float4) * m_Capacity, RAZIX_16B_ALIGN);
            m_LocalMatrices = (float4x4*) rz_malloc(sizeof(float4x4) * m_Capacity, RAZIX_16B_ALIGN);
            m_WorldMatrices = (float4x4*) rz_malloc(sizeof(float4x4) * m_Capacity, RAZIX_16B_ALIGN);
            m_FreeList      = (u32*) rz_malloc_aligned(sizeof(u32) * m_Capacity);
            m_Count         = 0;
            m_FreeListHead  = 0;

            for (u32 i = 0; i < m_Capacity; ++i) {
                m_FreeList[i] = i + 1;
            }
            m_FreeList[m_Capacity - 1] = UINT32_MAX;    // End of list
        }

        void destroy() override
        {
            if (m_Positions) {
                rz_free(m_Positions);
                m_Positions = NULL;
            }
            if (m_Rotations) {
                rz_free(m_Rotations);
                m_Rotations = NULL;
            }
            if (m_Scales) {
                rz_free(m_Scales);
                m_Scales = NULL;
            }
            if (m_LocalMatrices) {
                rz_free(m_LocalMatrices);
                m_LocalMatrices = NULL;
            }
            if (m_WorldMatrices) {
                rz_free(m_WorldMatrices);
                m_WorldMatrices = NULL;
            }
            if (m_FreeList) {
                rz_free(m_FreeList);
                m_FreeList = NULL;
            }
            m_Capacity     = 0;
            m_Count        = 0;
            m_FreeListHead = 0;
        }

        RZTransformAsset* allocate(u32& index)
        {
            if (m_FreeListHead == UINT32_MAX) {
                // Pool is full
                return NULL;
            }
            index          = m_FreeListHead;
            m_FreeListHead = m_FreeList[index];
            m_Count++;

            return NULL;
        }

        void release(u32 index)
        {
            if (index >= m_Capacity)
                return;
            m_FreeList[index] = m_FreeListHead;
            m_FreeListHead    = index;
            m_Count--;
        }

        RZTransformAsset* get(u32 index)
        {
            if (index >= m_Capacity) {
                return NULL;
            }
            return NULL;
        }

        u32       getCapacity() const { return m_Capacity; }
        u32       getCount() const { return m_Count; }
        float4*   getPositionPtr(u32 index) { return &m_Positions[index]; }
        float4*   getRotationPtr(u32 index) { return &m_Rotations[index]; }
        float4*   getScalePtr(u32 index) { return &m_Scales[index]; }
        float4x4* getLocalMatrixPtr(u32 index) { return &m_LocalMatrices[index]; }
        float4x4* getWorldMatrixPtr(u32 index) { return &m_WorldMatrices[index]; }

        void setPosition(u32 index, const float4& position) { m_Positions[index] = position; }
        void setRotation(u32 index, const float4& rotation) { m_Rotations[index] = rotation; }
        void setScale(u32 index, const float4& scale) { m_Scales[index] = scale; }
        void setLocalMatrix(u32 index, const float4x4& localMatrix) { m_LocalMatrices[index] = localMatrix; }
        void setWorldMatrix(u32 index, const float4x4& worldMatrix) { m_WorldMatrices[index] = worldMatrix; }

        float4*   getPositions() { return m_Positions; }
        float4*   getRotations() { return m_Rotations; }
        float4*   getScales() { return m_Scales; }
        float4x4* getLocalMatrices() { return m_LocalMatrices; }
        float4x4* getWorldMatrices() { return m_WorldMatrices; }

    private:
        float4*   m_Positions     = NULL;
        float4*   m_Rotations     = NULL;
        float4*   m_Scales        = NULL;
        float4x4* m_LocalMatrices = NULL;
        float4x4* m_WorldMatrices = NULL;
        u32       m_Capacity      = 0;
        u32       m_Count         = 0;
        u32*      m_FreeList      = NULL;
        u32       m_FreeListHead  = 0;
    };
}    // namespace Razix
