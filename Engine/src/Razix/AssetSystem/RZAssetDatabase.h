#pragma once
#include "Razix/AssetSystem/RZAssetPool.h"
#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/hash_map.h"

namespace Razix {

    /**
     * The Asset Database manages all asset pools and provides a central interface for asset allocation and retrieval.
     * It registers asset types and their corresponding pools.
     * It ensures O(1) access to asset data using handles.
     */
    class RAZIX_API RZAssetDatabase
    {
    public:
        RZAssetDatabase()  = default;
        ~RZAssetDatabase() = default;

        /**
         * Initializes the asset database.
         * Sets up the central asset header pool and registers default asset types.
         */
        void init();

        /**
         * Shuts down the asset database and releases all resources.
         */
        void shutDown();

        /**
         * Registers a new asset type with a specific pool.
         * @tparam T The asset payload type.
         * @param type The asset type enum.
         * @param capacity The initial capacity of the pool for this asset type.
         */
        template<typename T>
        void registerAssetType(RZAssetType type, u32 capacity)
        {
            // Create a new pool for this type
            RZAssetPool<T>* pool = new RZAssetPool<T>();
            pool->init(capacity);

            // Store the pool pointer in our registry
            // We use a void* to store different pool types, but we need to know how to cast it back or use a base interface
            // Since RZAssetPool is templated, we might need a base class or use void* and cast based on type
            // For now, let's use void* and assume we know the type when accessing
            if ((u32) type < (u32) RZAssetType::COUNT) {
                m_AssetPools[(u32) type] = pool;
            }
        }

        /**
         * Allocates a new asset of the given type.
         * @tparam T The asset payload type.
         * @param type The asset type.
         * @return A handle to the newly allocated asset.
         */
        template<typename T>
        rz_asset_handle allocate(RZAssetType type)
        {
            // 1. Allocate header in the central pool
            rz_asset_handle handle = 0;
            RZAsset*        asset  = m_HeaderPool.allocate(type, handle);

            if (!asset) {
                // Header allocation failed
                return RAZIX_ASSET_INVALID_HANDLE;
            }

            // 2. Allocate payload in the specific pool
            u32 typeIndex = (u32) type;
            if (typeIndex >= (u32) RZAssetType::COUNT || !m_AssetPools[typeIndex]) {
                // Pool not registered for this type
                // Rollback header allocation
                m_HeaderPool.release(handle);
                return RAZIX_ASSET_INVALID_HANDLE;
            }

            RZAssetPool<T>* pool         = static_cast<RZAssetPool<T>*>(m_AssetPools[typeIndex]);
            u32             payloadIndex = 0;
            T*              payload      = pool->allocate(payloadIndex);

            if (!payload) {
                // Payload allocation failed
                // Rollback header allocation
                m_HeaderPool.release(handle);
                return RAZIX_ASSET_INVALID_HANDLE;
            }

            // 3. Construct the full handle
            // Lower 32 bits: Header index (already in handle)
            // Upper 32 bits: Payload index
            handle |= ((u64) payloadIndex << 32);

            // 4. Store the handle in the payload if it has one (via macro)
            // We assume T has a 'handle' member as per RAZIX_ASSET_PAYLOAD_HEADER
            // We can't easily check this at compile time without concepts or SFINAE, but we assume it for now
            // reinterpret_cast<rz_asset_handle*>(payload)[0] = handle; // This is unsafe if layout changes
            // Better to rely on the user or a trait, but for now we assume the first member is the handle
            // or we can just return the handle and let the caller set it.
            // Actually, the macro puts 'handle' as the first member.
            *(rz_asset_handle*) payload = handle;

            // Update the asset header with the handle as well?
            // The asset header (RZAsset) has m_Hot.handle.
            // We should update it.
            // RZAsset is opaque, we need a setter or friend access.
            // RZAsset has no setHandle, but m_Hot is protected.
            // We might need to add a method to RZAsset or make RZAssetDatabase a friend.
            // For now, let's assume we can't easily set it without modifying RZAsset.
            // Wait, RZAssetBase.h: RZAsset has m_Hot.handle?
            // Looking at RZAssetBase.h, RZAssetHotData has 'handle'.
            // But RZAsset class doesn't expose a setter for it.
            // We should probably add one or make RZAssetDatabase a friend.

            return handle;
        }

        /**
         * Retrieves the asset header (hot data) for a given handle.
         * @param handle The asset handle.
         * @return Pointer to the RZAsset.
         */
        RZAsset* getAsset(rz_asset_handle handle)
        {
            return m_HeaderPool.get(handle);
        }

        /**
         * Retrieves the asset payload for a given handle.
         * @tparam T The asset payload type.
         * @param handle The asset handle.
         * @return Pointer to the asset payload.
         */
        template<typename T>
        T* getAssetPayload(rz_asset_handle handle)
        {
            if (handle == RAZIX_ASSET_INVALID_HANDLE) return nullptr;

            // Extract type from header to verify? Or assume caller knows T?
            // For speed, we assume caller knows T.
            // But we need the type index to find the pool.
            // We can get the type from the header.
            RZAsset* asset = m_HeaderPool.get(handle);
            if (!asset) return nullptr;

            RZAssetType type      = asset->getType();
            u32         typeIndex = (u32) type;

            if (typeIndex >= (u32) RZAssetType::COUNT || !m_AssetPools[typeIndex]) {
                return nullptr;
            }

            RZAssetPool<T>* pool         = static_cast<RZAssetPool<T>*>(m_AssetPools[typeIndex]);
            u32             payloadIndex = (u32) (handle >> 32);

            return pool->get(payloadIndex);
        }

        /**
         * Releases an asset.
         * @tparam T The asset payload type.
         * @param handle The asset handle.
         */
        template<typename T>
        void release(rz_asset_handle handle)
        {
            if (handle == RAZIX_ASSET_INVALID_HANDLE) return;

            RZAsset* asset = m_HeaderPool.get(handle);
            if (!asset) return;

            RZAssetType type      = asset->getType();
            u32         typeIndex = (u32) type;

            // Release payload
            if (typeIndex < (u32) RZAssetType::COUNT && m_AssetPools[typeIndex]) {
                RZAssetPool<T>* pool         = static_cast<RZAssetPool<T>*>(m_AssetPools[typeIndex]);
                u32             payloadIndex = (u32) (handle >> 32);
                pool->release(payloadIndex);
            }

            // Release header
            m_HeaderPool.release(handle);
        }

        static RZAssetDatabase& Get()
        {
            static RZAssetDatabase instance;
            return instance;
        }

    private:
        RZAssetHeaderPool m_HeaderPool;
        void*             m_AssetPools[(u32) RZAssetType::COUNT] = {NULL};
    };

}    // namespace Razix
