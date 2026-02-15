// clang-format off
#include "rzxpch.h"
#include <AssetSystem/RZAssetBase.h>
// clang-format on

#include "RZAssetDB.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Core/Job/RZJobSystem.h"
#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"
#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/Profiling/RZProfiling.h"
#include "Razix/Core/RZThreadCore.h"
#include "Razix/Core/Serialization/RZSerializable.h"
#include "Razix/Core/Utils/RZBuildUtils.h"
#include "Razix/Core/std/utility.h"

#include <type_traits>

namespace Razix {

    constexpr bool bCapCapacityToMaxAssets = true;
    constexpr u32  kRZAssetHeaderSlotBytes = sizeof(RZAsset) + sizeof(RZAssetColdData) + sizeof(u32);

    using AssetSaveFn = bool (*)(rz_asset_handle handle);
    using AssetLoadFn = void (*)(const RZAssetAsyncLoadJobData& jobData);

    struct RZAssetIOHandlers
    {
        AssetSaveFn save = nullptr;
        AssetLoadFn load = nullptr;
    };

    static RZAssetIOHandlers s_AssetIORegistry[(size_t) RZAssetType::COUNT];

    template<typename T>
    static void RegisterAssetIOHandlers(RZAssetType type)
    {
        s_AssetIORegistry[(size_t) type].save = [](rz_asset_handle handle) -> bool {
            // Use engine heap allocator or passed allocator
            Memory::RZHeapAllocator& heapAllocator = RZEngine::Get().getSystemAllocator();

            const RZAsset* hdr = RZAssetDB::Get().getAssetHeader(handle);
            if (!hdr) {
                RAZIX_CORE_ERROR("[AssetSystem] Failed to retrieve asset header for handle: {}", handle);
                return false;
            }

            auto& pool       = RZAssetDB::Get().GetAssetPoolRef<T>();
            T*    payloadPtr = pool.get(handle);
            if (!payloadPtr) {
                RAZIX_CORE_ERROR("[AssetSystem] Failed to retrieve asset payload for handle: {}", handle);
                return false;
            }

            auto headerBinary  = RZSerializable<RZAsset>::serializeToBinary(*hdr, heapAllocator);
            auto payloadBinary = RZSerializable<T>::serializeToBinary(*payloadPtr, heapAllocator);

            RZFileHandle fileHandle = RZFileSystem::OpenFile(hdr->getName(), RZFileMode::Write);
            if (fileHandle.handle == -1) {
                RAZIX_CORE_ERROR("[AssetSystem] Failed to open file for writing: {}", hdr->getName());
                return false;
            }

            u32 magic = RAZIX_ASSSET_FILE_MAGIC;
            u32 size  = RZFileSystem::WriteToFile(fileHandle, &magic, sizeof(u32));
            RAZIX_CORE_ASSERT(size == sizeof(u32), "[AssetSystem] Failed to write file magic for asset: {}", hdr->getName());

            size = RZFileSystem::WriteToFile(fileHandle, headerBinary.data(), headerBinary.size());
            RAZIX_CORE_ASSERT(size == headerBinary.size(), "[AssetSystem] Failed to write asset header for asset: {}", hdr->getName());

            size = RZFileSystem::WriteToFile(fileHandle, payloadBinary.data(), payloadBinary.size());
            RAZIX_CORE_ASSERT(size == payloadBinary.size(), "[AssetSystem] Failed to write asset payload for asset: {}", hdr->getName());

            RZFileSystem::CloseFile(fileHandle);
            return true;
        };

        s_AssetIORegistry[(size_t) type].load = [](const RZAssetAsyncLoadJobData& jobData) {
            RAZIX_UNIMPLEMENTED_METHOD;
        };
    }

    //-------------------------------------------------------------------------

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
        u64 raw_size   = rz_mem_align(maxAssets * kRZAssetHeaderSlotBytes, RAZIX_CACHE_LINE_ALIGN);
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

    static void AsyncRZAssetLoadJob(rz_job* pJob)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        auto* jobData = reinterpret_cast<RZAssetAsyncLoadJobData*>(pJob->hot.pUserData);
        RAZIX_CORE_ASSERT(jobData != NULL, "[AssetSystem] Invalid job data for async asset load.");

        RAZIX_CORE_INFO("[AssetSystem] Asynchronously loading asset: {} of type: {} from path: {}", jobData->AssetUUID.prettyString(), jobData->AssetType, jobData->FilePath);
    }

    struct RZAssetAsyncLoadJob
    {
        rz_job* job;

        RZAssetAsyncLoadJob(RZString jobName, const RZAssetAsyncLoadJobData& jobData)
        {
            Memory::RZBumpAllocator& frameAllocator = RZEngine::Get().getFrameAllocator();

            job                   = static_cast<rz_job*>(frameAllocator.allocate(sizeof(rz_job)));
            rz_job_cold* coldData = static_cast<rz_job_cold*>(frameAllocator.allocate(sizeof(rz_job_cold), RAZIX_CACHE_LINE_ALIGN));
            job->pCold            = coldData;
            // TODO: Use a razix utility function to set const char* names safely
            memcpy(coldData->pName, jobName.c_str(), std::min(jobName.size(), static_cast<size_t>(RAZIX_JOB_NAME_MAX_CHARS - 1)));
            job->hot.pFunc = AsyncRZAssetLoadJob;

            RZAssetAsyncLoadJobData* jobDataPtr = static_cast<RZAssetAsyncLoadJobData*>(frameAllocator.allocate(sizeof(RZAssetAsyncLoadJobData)));
            *jobDataPtr                         = jobData;
            job->hot.pUserData                  = jobDataPtr;
        }
    };

    //-------------------------------------------------------------------------

    void RZAssetDB::Startup(Memory::RZHeapAllocator& assetAllocator, Memory::RZHeapAllocator& assetHeaderAllocator)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        RAZIX_CORE_INFO("[AssetSystem] Starting up asset system...");

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

        RegisterAssetIOHandlers<RZTransformAsset>(RZAssetType::kTransform);
        RegisterAssetIOHandlers<RZCameraAsset>(RZAssetType::kCamera);
        RegisterAssetIOHandlers<RZLightAsset>(RZAssetType::kLight);
        RegisterAssetIOHandlers<RZMaterialAsset>(RZAssetType::kMaterial);
        RegisterAssetIOHandlers<RZPhysicsMaterialAsset>(RZAssetType::kPhysicsMaterial);
        RegisterAssetIOHandlers<RZMeshAsset>(RZAssetType::kMesh);
        RegisterAssetIOHandlers<RZTextureAsset>(RZAssetType::kTexture);
        RegisterAssetIOHandlers<RZAnimationAsset>(RZAssetType::kAnimation);
        RegisterAssetIOHandlers<RZAudioAsset>(RZAssetType::kAudio);
        RegisterAssetIOHandlers<RZLuaScriptAsset>(RZAssetType::kLuaScript);
        RegisterAssetIOHandlers<RZAssetRefAsset>(RZAssetType::kAssetRef);
        RegisterAssetIOHandlers<RZVignerePuzzleAsset>(RZAssetType::kVignerePuzzle);
        RegisterAssetIOHandlers<RZClothAsset>(RZAssetType::kCloth);
        RegisterAssetIOHandlers<RZGameDataAsset>(RZAssetType::kGameData);

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

        //-----------------------------------------------------------------------------
        // load registry in development builds
#if RAZIX_IS_DEVELOPMENT_BUILD
        if (!loadAssetDBRegistry()) {
            RAZIX_CORE_ERROR("[AssetSystem] Failed to load registry: {0}", RAZIX_ASSET_DB_REGISTRY_FILE);
            RAZIX_CORE_ASSERT(false, "AssetDB registry missing or corrupted.");
        }
#endif
    }

    void RZAssetDB::Shutdown()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        RAZIX_CORE_INFO("[AssetSystem] Shutting down asset system...");

#if RAZIX_IS_DEVELOPMENT_BUILD
        exportAssetDBRegistry();
#endif

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

#if RAZIX_IS_DEVELOPMENT_BUILD
    bool RZAssetDB::saveAssetToDisk(rz_asset_handle handle) const
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        // TODO: Serialize RZAsset and it's payload to disk using RSSerializable
        // For now we can just write the contents to a buffer via the frameAllocator scratch space
        // and then flush the contents to disk using RZFileSystem.
        // What to serialize and how:
        // - RZAsset is serialized along with it's cold data, just call serializeToBinary
        const RZAsset* hdr = getAssetHeader(handle);
        if (!hdr)
            return false;

        auto type = hdr->getType();

        auto fn = s_AssetIORegistry[(size_t) type].save;
        if (!fn)
            return false;
        fn(handle);

        return true;
    }

    RZString RZAssetDB::requestAssetLoadFromDiskInternal(RZUUID assetUUID, std::type_index typeIdx)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        auto it = m_AssetDBDevRegistry.find(assetUUID);
        if (it == m_AssetDBDevRegistry.end()) {
            RAZIX_CORE_WARN("[AssetSystem] Asset UUID {} not found in registry.", assetUUID.prettyString());
            return "<Invalid Asset>";
        }

        const RZString& assetPath = it->second;
        RAZIX_CORE_INFO("[AssetSystem] [ASYNC] Loading asset from disk: UUID={}, Path={}", assetUUID.prettyString(), assetPath);
        // async load the asset from disk using the path, and once loaded, update the handle in the registry with the actual rz_asset_handle
        // For now, just return an invalid handle as a placeholder
        // using the default asset immediately and then switch to the actual asset once it's loaded
        // Asset status is set to loading on the dummy asset, we create a default one in the correct slot and return it's handle with default data
        // As for creating, we can call the createAsset here with the correct type, which will give us a handle and also create the default asset in the pool,
        // then we can update the registry with the handle and kick off the async load, once the async load is done, we can update the asset data in the pool
        // with the actual data loaded from disk, and set the asset status to loaded, so that the game can start using it
        auto assetloadAsyncJob = RZAssetAsyncLoadJob("Async Asset Load Job", RZAssetAsyncLoadJobData{assetUUID, GetAssetTypeFromTypeIndex(typeIdx), assetPath});
        rz_job_system_submit_job(assetloadAsyncJob.job);

        return GetFileName(assetPath);
    }

    bool RZAssetDB::loadAssetDBRegistry()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        RZString physicalPath;
        if (!RZVirtualFileSystem::Get().resolvePhysicalPath(RAZIX_ASSET_DB_REGISTRY_FILE, physicalPath)) {
            RAZIX_CORE_WARN("[AssetSystem] Registry file not found: {0}. Starting with empty registry.", RAZIX_ASSET_DB_REGISTRY_FILE);
            m_AssetDBDevRegistry.clear();
            return true;
        }

        u64  fileSize        = 0;
        u32  assetDBFileSize = RZFileSystem::GetFileSize(physicalPath);
        u8*  buffer          = (u8*) rz_malloc_aligned(assetDBFileSize);
        bool bReadResult     = RZFileSystem::ReadFile(physicalPath, buffer, assetDBFileSize);
        if (bReadResult || fileSize < sizeof(u32)) {
            RAZIX_CORE_ERROR("[AssetSystem] Failed reading registry file or file corrupted.");
            rz_free(buffer);
            return false;
        }

        m_AssetDBDevRegistry.clear();

        const u8* cursor = buffer;
        const u8* end    = buffer + fileSize;

        auto readU32 = [&](u32& out) {
            RAZIX_CORE_ASSERT(cursor + sizeof(u32) <= end, "[AssetSystem] Registry read overflow (u32).");
            memcpy(&out, cursor, sizeof(u32));
            cursor += sizeof(u32);
        };

        auto readUUID = [&](RZUUID& uuid) {
            RAZIX_CORE_ASSERT(cursor + sizeof(RZUUID) <= end, "[AssetSystem] Registry read overflow (UUID).");
            memcpy(uuid.mutable_data(), cursor, sizeof(RZUUID));
            cursor += sizeof(RZUUID);
        };

        u32 entryCount = 0;
        readU32(entryCount);
        RAZIX_CORE_TRACE("[AssetSystem] Loading assetdb.bin registry with {0} entries.", entryCount);

        for (u32 i = 0; i < entryCount; ++i) {
            RZUUID uuid;
            readUUID(uuid);

            u32 stringSize = 0;
            readU32(stringSize);

            RAZIX_CORE_ASSERT(cursor + stringSize <= end, "[AssetSystem] Registry read overflow (string).");

            RZString path;
            path.append(reinterpret_cast<const char*>(cursor), stringSize);
            cursor += stringSize;

            RAZIX_CORE_TRACE("[AssetSystem] Loaded registry entry: UUID={0}, Path={1}", uuid.prettyString(), path);

            m_AssetDBDevRegistry[uuid] = path;
        }

        rz_free(buffer);

        RAZIX_CORE_INFO("[AssetSystem] Loaded {0} registry entries.", entryCount);
        return true;
    }

    void RZAssetDB::exportAssetDBRegistry() const
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_ASSET_SYSTEM);

        // In development builds, we can save/load the entire registry to a single file for faster iteration
        // called when AssetDB startup/shutsdown happens
        // No streaming or zone --> pak loading support for this, just a single
        // file with all assets in it, used for faster iteration during development

        u32    entryCount       = static_cast<u32>(m_AssetDBDevRegistry.size());
        size_t registryFileSize = sizeof(u32) + entryCount * (sizeof(RZUUID) + sizeof(u32) + RAZIX_ASSET_MAX_FILE_PATH_LENGTH);
        u8*    buffer           = (u8*) rz_malloc_aligned(registryFileSize);
        u64    offset           = 0;

        // write count first
        buffer[offset] = entryCount;
        offset += sizeof(u32);

        // Compute total size needed: 4 bytes for entry count + (16 bytes for UUID + 4 bytes for string length + string bytes) per entryCount

        for (const auto& [uuid, path]: m_AssetDBDevRegistry) {
            RAZIX_CORE_TRACE("[AssetSystem] Registry entry to export: UUID={0}, Path={1}", uuid.prettyString(), path);

            // Write uuid
            memcpy(buffer + offset, uuid.data(), sizeof(RZUUID));
            offset += sizeof(RZUUID);

            // Write string length
            u32 stringLength = static_cast<u32>(path.size());
            memcpy(buffer + offset, &stringLength, sizeof(u32));
            offset += sizeof(u32);

            // Write string bytes
            // cap the path to RAZIX_ASSET_MAX_FILE_PATH_LENGTH to prevent overflow during read/writes
            RAZIX_CORE_ASSERT(path.size() <= RAZIX_ASSET_MAX_FILE_PATH_LENGTH, "[AssetSystem] Asset path exceeds max length: {0}", path);
            u32 stringBytesToWrite = rz_min<u32>(static_cast<u32>(path.size()), RAZIX_ASSET_MAX_FILE_PATH_LENGTH);
            memcpy(buffer + offset, path.c_str(), stringBytesToWrite);
            offset += stringBytesToWrite;
        }

        RAZIX_CORE_TRACE("[AssetSystem] Finished writing registry entries to buffer. Total size: {} bytes. Expected size: {} bytes.", offset, registryFileSize);
        RAZIX_CORE_ASSERT(offset <= registryFileSize, "[AssetSystem] Buffer overflow during registry export: offset {} exceeds allocated size {}", offset, registryFileSize);
        // Write buffer to file
        RZString physicalPath;
        if (!RZVirtualFileSystem::Get().resolvePhysicalPath("//Assets/", physicalPath, true)) {
            RAZIX_CORE_ERROR("[AssetSystem] Failed to resolve physical path for registry export: {0}", RAZIX_ASSET_DB_REGISTRY_FILE);
            rz_free(buffer);
            return;
        }

        physicalPath += RAZIX_ASSET_DB_REGISTRY_FILE_NAME;

        bool bWriteResult = RZFileSystem::WriteFile(physicalPath, buffer, offset);
        if (!bWriteResult) {
            RAZIX_CORE_ERROR("[AssetSystem] Failed to write registry file: {0}", physicalPath);
        } else {
            RAZIX_CORE_INFO("[AssetSystem] Successfully exported registry with {0} entries to {1} ({2} bytes).", entryCount, physicalPath, offset);
        }

        rz_free(buffer);
    }
#endif    // RAZIX_IS_DEVELOPMENT_BUILD

}    // namespace Razix
