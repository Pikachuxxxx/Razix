#pragma once

#include "Razix/Asset/RZTransformAsset.h"
#include "Razix/AssetSystem/RZAssetBase.h"
#include "Razix/Core/Memory/RZMemoryBudgets.h"
#include "Razix/Core/RZDepartments.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include "Razix/Core/Profiling/RZProfiling.h"

#define RAZIX_ASSETPOOL_DEFAULT_CAPACITY 1024_Kib    // 1 MB

namespace Razix {

    // [Optional] TODO: pagefault/memory safety hooks --> allocated by pages etc.?

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

    struct RZAssetPoolHooks
    {
        void (*onAllocate)(rz_asset_handle handle, const char* label) = NULL;
        void (*onRelease)(rz_asset_handle handle, const char* label)  = NULL;
    };

    struct RAZIX_API RZAssetPoolConfig
    {
        Memory::RZMemoryPoolType poolType   = Memory::RZ_MEM_POOL_TYPE_ASSET_POOL;
        RZString                 debugLabel = "<RZAssetPool>";
        RZAssetPoolHooks         hooks      = {};
    };

    u32 GetAssetPoolCapacityFromMemoryBudget(Memory::RZMemoryPoolType poolType, u32 slotSize);
    u32 ClampAssetPoolCapacity(Memory::RZMemoryPoolType poolType, u32 desiredCapacity, u32 slotSize);

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

        void init(void* where, u32 capacity);
        void init(u32 capacity);
        void destroy();

        u32                    allocate(RZAssetType type);
        void                   release(u32 index);
        const RZAsset*         get(rz_asset_handle handle) const;
        const RZAssetColdData* getColdData(rz_asset_handle handle) const;
        RZAsset*               getMutablePtr(rz_asset_handle handle) const;
        RZAssetColdData*       getColdDataMutablePtr(rz_asset_handle handle) const;

        inline u32 getCapacity() const { return m_Capacity; }
        inline u32 getCount() const { return m_Count; }

        inline void                     setHooks(const RZAssetPoolHooks& hooks) { m_Config.hooks = hooks; }
        inline const RZAssetPoolConfig& getConfig() const { return m_Config; }
        inline void*                    getBackingMemoryMutablePtr() { return static_cast<void*>(m_Assets); }

    private:
        RZAsset*          m_Assets         = NULL;
        RZAssetColdData*  m_ColdData       = NULL;
        u32*              m_FreeList       = NULL;
        u32               m_Capacity       = 0;
        u32               m_Count          = 0;
        u32               m_FreeListHead   = 0;
        RZAssetPoolConfig m_Config         = {};
        bool              m_ExternalMemory = false;
    };

    //class RAZIX_API RZAssetPoolBase
    //{
    //public:
    //    virtual ~RZAssetPoolBase() = default;
    //    virtual void destroy()     = 0;
    //};

    template<typename T>
    class RZAssetPool final
    {
    public:
        using value_type = T;

        RZAssetPool()          = default;
        virtual ~RZAssetPool() = default;

        void init(void* where, u32 capacity, const RZAssetPoolConfig& config)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            u32 slotSize     = sizeof(T);
            u32 poolCapacity = ClampAssetPoolCapacity(config.poolType, capacity, slotSize);
            init(where, poolCapacity);
            m_Config = config;
        }

        void init(u32 capacity, const RZAssetPoolConfig& config)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            u32 slotSize     = sizeof(T);
            u32 poolCapacity = ClampAssetPoolCapacity(config.poolType, capacity, slotSize);
            init(poolCapacity);
            m_Config = config;
        }

        void init(void* where, u32 capacity)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            m_Capacity       = capacity;
            m_Data           = reinterpret_cast<T*>(where);
            m_FreeList       = reinterpret_cast<u32*>((u8*) where + sizeof(T) * capacity);
            m_Count          = 0;
            m_FreeListHead   = 0;
            m_ExternalMemory = true;
            for (u32 i = 0; i < m_Capacity; ++i)
                m_FreeList[i] = i + 1;
            m_FreeList[m_Capacity - 1] = UINT32_MAX;    // End of list
        }

        void init(u32 capacity)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            m_Capacity     = capacity;
            m_Data         = (T*) rz_malloc(sizeof(T) * m_Capacity, RAZIX_16B_ALIGN);
            m_FreeList     = (u32*) rz_malloc_aligned(sizeof(u32) * m_Capacity);
            m_Count        = 0;
            m_FreeListHead = 0;

            m_ExternalMemory = false;

            for (u32 i = 0; i < m_Capacity; ++i)
                m_FreeList[i] = i + 1;
            m_FreeList[m_Capacity - 1] = UINT32_MAX;    // End of list
        }

        void destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (m_Data && !m_ExternalMemory) {
                rz_free(m_Data);
                m_Data = NULL;
            }
            if (m_FreeList && !m_ExternalMemory) {
                rz_free(m_FreeList);
                m_FreeList = NULL;
            }
            m_Capacity       = 0;
            m_Count          = 0;
            m_FreeListHead   = 0;
            m_ExternalMemory = false;
        }

        u32 allocate(RZAssetType /*assetType*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (m_FreeListHead == UINT32_MAX) {
                // Pool is full
                // TODO: Resize pool?
                // Actually no dynamic resizing, account for asset memory budget before hand
                RAZIX_CORE_ERROR("[AssetSystem] Asset Pool is full! Cannot allocate more assets. Please edit the budget files to adjust meomry budget at engine ingition time. Game budget should always be pre-compute and fixed.");
                return RAZIX_ASSET_INVALID_HANDLE;
            }
            u32 index      = m_FreeListHead;
            m_FreeListHead = m_FreeList[index];
            m_Count++;

            if (m_Config.hooks.onAllocate) {
                rz_asset_handle handle = (rz_asset_handle) index;
                m_Config.hooks.onAllocate(handle, m_Config.debugLabel.c_str());
            }

            return index;
        }

        void release(u32 index)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (index >= m_Capacity)
                return;
            m_FreeList[index] = m_FreeListHead;
            m_FreeListHead    = index;
            m_Count--;

            if (m_Config.hooks.onRelease) {
                rz_asset_handle handle = (rz_asset_handle) index;
                m_Config.hooks.onRelease(handle, m_Config.debugLabel.c_str());
            }

            rz_poison_memory(&m_Data[index], m_SlotSize);
        }

        T* get(u32 index)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (index >= m_Capacity) {
                return NULL;
            }
            return &m_Data[index];
        }

        void set(u32 index, const T& value)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (index >= m_Capacity) {
                return;
            }
            m_Data[index] = value;
        }

        inline u32 getCapacity() const { return m_Capacity; }
        inline u32 getCount() const { return m_Count; }

        inline void* getBackingMemoryMutablePtr() { return static_cast<void*>(m_Data); }

    private:
        T*                m_Data           = NULL;
        u32*              m_FreeList       = NULL;
        u32               m_Capacity       = 0;
        u32               m_Count          = 0;
        u32               m_FreeListHead   = 0;
        u32               m_SlotSize       = sizeof(T);
        RZAssetPoolConfig m_Config         = {};
        bool              m_ExternalMemory = false;
    };

    //---------------------------------------------------------------
    // RZAssetPool specialization for RZAssetPool<RZTransformAsset> due to SOA design
    //---------------------------------------------------------------

    struct RZTransformAsset;

    template<>
    class RZAssetPool<RZTransformAsset>
    {
    public:
        using value_type = RZTransformAsset;

        RZAssetPool()          = default;
        virtual ~RZAssetPool() = default;

        void init(void* where, u32 capacity, const RZAssetPoolConfig& config)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            u32 slotSize     = sizeof(RZTransformAsset) + sizeof(float4) * 3 + sizeof(float4x4) * 2;
            u32 poolCapacity = ClampAssetPoolCapacity(config.poolType, capacity, slotSize);
            init(where, poolCapacity);
            m_Config = config;
        }

        void init(u32 capacity, const RZAssetPoolConfig& config)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            u32 slotSize     = sizeof(RZTransformAsset) + sizeof(float4) * 3 + sizeof(float4x4) * 2;
            u32 poolCapacity = ClampAssetPoolCapacity(config.poolType, capacity, slotSize);
            init(poolCapacity);
            m_Config = config;
        }

        void init(void* where, u32 capacity)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            m_Capacity       = capacity;
            m_Assets         = reinterpret_cast<RZTransformAsset*>(where);
            m_Positions      = reinterpret_cast<float4*>((u8*) where + sizeof(RZTransformAsset) * capacity);
            m_Rotations      = reinterpret_cast<float4*>((u8*) where + sizeof(RZTransformAsset) * capacity + sizeof(float4) * capacity);
            m_Scales         = reinterpret_cast<float4*>((u8*) where + sizeof(RZTransformAsset) * capacity + sizeof(float4) * capacity * 2);
            m_LocalMatrices  = reinterpret_cast<float4x4*>((u8*) where + sizeof(RZTransformAsset) * capacity + sizeof(float4) * capacity * 3);
            m_WorldMatrices  = reinterpret_cast<float4x4*>((u8*) where + sizeof(RZTransformAsset) * capacity + sizeof(float4) * capacity * 3 + sizeof(float4x4) * capacity);
            m_FreeList       = reinterpret_cast<u32*>((u8*) where + sizeof(RZTransformAsset) * capacity + sizeof(float4) * capacity * 3 + sizeof(float4x4) * capacity * 2);
            m_Count          = 0;
            m_FreeListHead   = 0;
            m_ExternalMemory = true;

            for (u32 i = 0; i < m_Capacity; ++i)
                m_FreeList[i] = i + 1;
        }

        void init(u32 capacity)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            m_Capacity      = capacity;
            m_Assets        = (RZTransformAsset*) rz_malloc(sizeof(RZTransformAsset) * m_Capacity, RAZIX_16B_ALIGN);
            m_Positions     = (float4*) rz_malloc(sizeof(float4) * m_Capacity, RAZIX_16B_ALIGN);
            m_Rotations     = (float4*) rz_malloc(sizeof(float4) * m_Capacity, RAZIX_16B_ALIGN);
            m_Scales        = (float4*) rz_malloc(sizeof(float4) * m_Capacity, RAZIX_16B_ALIGN);
            m_LocalMatrices = (float4x4*) rz_malloc(sizeof(float4x4) * m_Capacity, RAZIX_16B_ALIGN);
            m_WorldMatrices = (float4x4*) rz_malloc(sizeof(float4x4) * m_Capacity, RAZIX_16B_ALIGN);
            m_FreeList      = (u32*) rz_malloc_aligned(sizeof(u32) * m_Capacity);
            m_Count         = 0;
            m_FreeListHead  = 0;

            m_ExternalMemory = false;

            for (u32 i = 0; i < m_Capacity; ++i)
                m_FreeList[i] = i + 1;
            m_FreeList[m_Capacity - 1] = UINT32_MAX;    // End of list
        }

        void destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (m_Assets && !m_ExternalMemory) {
                rz_free(m_Assets);
                m_Assets = NULL;
            }
            if (m_Positions && !m_ExternalMemory) {
                rz_free(m_Positions);
                m_Positions = NULL;
            }
            if (m_Rotations && !m_ExternalMemory) {
                rz_free(m_Rotations);
                m_Rotations = NULL;
            }
            if (m_Scales && !m_ExternalMemory) {
                rz_free(m_Scales);
                m_Scales = NULL;
            }
            if (m_LocalMatrices && !m_ExternalMemory) {
                rz_free(m_LocalMatrices);
                m_LocalMatrices = NULL;
            }
            if (m_WorldMatrices && !m_ExternalMemory) {
                rz_free(m_WorldMatrices);
                m_WorldMatrices = NULL;
            }
            if (m_FreeList && !m_ExternalMemory) {
                rz_free(m_FreeList);
                m_FreeList = NULL;
            }
            m_Capacity       = 0;
            m_Count          = 0;
            m_FreeListHead   = 0;
            m_ExternalMemory = false;
        }

        u32 allocate(RZAssetType /*assetType*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (m_FreeListHead == UINT32_MAX) {
                // Pool is full
                RAZIX_CORE_ERROR("[AssetSystem] Asset Pool is full! Cannot allocate more assets. Please edit the budget files to adjust meomry budget at engine ingition time. Game budget should always be pre-compute and fixed.");
                return RAZIX_ASSET_INVALID_HANDLE;
            }
            u32 index      = m_FreeListHead;
            m_FreeListHead = m_FreeList[index];
            m_Count++;

            // Reset slot to sensible defaults for transforms
            m_Assets[index].handle = RAZIX_ASSET_INVALID_HANDLE;
            m_Positions[index]     = float4(0.0f);
            m_Rotations[index]     = float4(0.0f, 0.0f, 0.0f, 1.0f);
            m_Scales[index]        = float4(1.0f);
            m_LocalMatrices[index] = float4x4(1.0f);
            m_WorldMatrices[index] = float4x4(1.0f);

            if (m_Config.hooks.onAllocate) {
                rz_asset_handle handle = (rz_asset_handle) index;
                m_Config.hooks.onAllocate(handle, m_Config.debugLabel.c_str());
            }

            return index;
        }

        void release(u32 index)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (index >= m_Capacity)
                return;
            m_Assets[index].handle = RAZIX_ASSET_INVALID_HANDLE;
            m_Positions[index]     = float4(0.0f);
            m_Rotations[index]     = float4(0.0f, 0.0f, 0.0f, 1.0f);
            m_Scales[index]        = float4(1.0f);
            m_LocalMatrices[index] = float4x4(1.0f);
            m_WorldMatrices[index] = float4x4(1.0f);
            m_FreeList[index]      = m_FreeListHead;
            m_FreeListHead         = index;
            m_Count--;

            if (m_Config.hooks.onRelease) {
                rz_asset_handle handle = (rz_asset_handle) index;
                m_Config.hooks.onRelease(handle, m_Config.debugLabel.c_str());
            }

            rz_poison_memory(&m_Assets[index], m_SlotSize);
            rz_poison_memory(&m_Positions[index], m_SlotSize);
            rz_poison_memory(&m_Rotations[index], m_SlotSize);
            rz_poison_memory(&m_Scales[index], m_SlotSize);
            rz_poison_memory(&m_LocalMatrices[index], m_SlotSize);
            rz_poison_memory(&m_WorldMatrices[index], m_SlotSize);
        }

        RZTransformAsset* get(u32 index)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

            if (index >= m_Capacity) {
                return NULL;
            }
            return &m_Assets[index];
        }

        inline u32       getCapacity() const { return m_Capacity; }
        inline u32       getCount() const { return m_Count; }
        inline float4*   getPositionPtr(u32 index) { return &m_Positions[index]; }
        inline float4*   getRotationPtr(u32 index) { return &m_Rotations[index]; }
        inline float4*   getScalePtr(u32 index) { return &m_Scales[index]; }
        inline float4x4* getLocalMatrixPtr(u32 index) { return &m_LocalMatrices[index]; }
        inline float4x4* getWorldMatrixPtr(u32 index) { return &m_WorldMatrices[index]; }

        inline void setPosition(u32 index, const float4& position) { m_Positions[index] = position; }
        inline void setRotation(u32 index, const float4& rotation) { m_Rotations[index] = rotation; }
        inline void setScale(u32 index, const float4& scale) { m_Scales[index] = scale; }
        inline void setLocalMatrix(u32 index, const float4x4& localMatrix) { m_LocalMatrices[index] = localMatrix; }
        inline void setWorldMatrix(u32 index, const float4x4& worldMatrix) { m_WorldMatrices[index] = worldMatrix; }

        inline float4*   getPositions() { return m_Positions; }
        inline float4*   getRotations() { return m_Rotations; }
        inline float4*   getScales() { return m_Scales; }
        inline float4x4* getLocalMatrices() { return m_LocalMatrices; }
        inline float4x4* getWorldMatrices() { return m_WorldMatrices; }

        inline void* getBackingMemoryMutablePtr() { return static_cast<void*>(m_Assets); }

    private:
        RZTransformAsset* m_Assets         = NULL;
        float4*           m_Positions      = NULL;
        float4*           m_Rotations      = NULL;
        float4*           m_Scales         = NULL;
        float4x4*         m_LocalMatrices  = NULL;
        float4x4*         m_WorldMatrices  = NULL;
        u32               m_Capacity       = 0;
        u32               m_Count          = 0;
        u32*              m_FreeList       = NULL;
        u32               m_FreeListHead   = 0;
        u32               m_SlotSize       = sizeof(RZTransformAsset) + sizeof(float4) * 3 + sizeof(float4x4) * 2;
        RZAssetPoolConfig m_Config         = {};
        bool              m_ExternalMemory = false;
    };
}    // namespace Razix
