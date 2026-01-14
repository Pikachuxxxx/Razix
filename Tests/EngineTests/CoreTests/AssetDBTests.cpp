// AssetDBTests.cpp
// Unit tests for RZAssetDB using RZHeapAllocator-backed pools

#include "Razix/AssetSystem/RZAssetDB.h"
#include "Razix/AssetSystem/RZAssetPool.h"
#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"
#include "Razix/Core/Memory/RZMemoryBudgets.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace Razix {
    namespace {

        static std::filesystem::path GetRepoRootFromThisFile()
        {
            std::filesystem::path p(__FILE__);
            return p.parent_path().parent_path().parent_path().parent_path();
        }

        static std::filesystem::path WriteBudgetFile(u32 assetPoolMB)
        {
            const std::string content = R"INI([GlobalFrameBudget]
TotalFrameBudgetMs = 16.67

[DeptTimingBudgets]
Core.FrameBudgetMs = 1.0
Environment.FrameBudgetMs = 1.0
Character.FrameBudgetMs = 1.0
Lighting.FrameBudgetMs = 1.0
RayTracing.FrameBudgetMs = 1.0
VFX.FrameBudgetMs = 1.0
UI.FrameBudgetMs = 1.0
Physics.FrameBudgetMs = 1.0
Scripting.FrameBudgetMs = 1.0
Audio.FrameBudgetMs = 1.0
Network.FrameBudgetMs = 1.0
Rendering.FrameBudgetMs = 1.0

[MemoryPools]
CoreSystems.HeapSizeMB = 16
AssetPool.HeapSizeMB = )INI" + std::to_string(assetPoolMB) +
                                        R"INI(
GfxResources.HeapSizeMB = 32
GfxResources.GPUMemoryMB = 64
RenderingPool.HeapSizeMB = 16

[ThreadFrameBudgets]
TotalFrameAllocatorBudget = 64
RenderThread.PerFrameAllocatorMB = 16
GameThread.PerFrameAllocatorMB = 32
WorkerThread.PerFrameAllocatorMB = 16
)INI";

            const std::filesystem::path tempPath = std::filesystem::temp_directory_path() / "AssetDBTestBudget.ini";
            std::ofstream               out(tempPath);
            out << content;
            out.close();
            return tempPath;
        }

    }    // namespace

    class AssetDBFixture : public ::testing::Test
    {
    protected:
        Memory::RZHeapAllocator allocator;
        RZAssetDB               db;
        std::filesystem::path   tempBudgetPath;
        std::filesystem::path   defaultBudgetPath;

        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();

            const u32 assetBudgetMB = 16;    // small but sufficient for 1024 camera slots
            tempBudgetPath          = WriteBudgetFile(assetBudgetMB);
            ASSERT_TRUE(Memory::ParseBudgetFile(RZString(tempBudgetPath.string().c_str())));

            defaultBudgetPath = GetRepoRootFromThisFile() / "Engine/content/config/RazixDepartmentBudgets.ini";

            const auto   assetBudget = Memory::GetMemoryPoolBudget(Memory::RZ_MEM_POOL_TYPE_ASSET_POOL);
            const size_t chunkSize   = static_cast<size_t>(assetBudget.HeapSizeMB) * 1024 * 1024;
            allocator.init(chunkSize);

            db.Startup(allocator);
        }

        void TearDown() override
        {
            db.Shutdown();
            allocator.shutdown();

            // Restore the default budgets so other tests see expected values
            Memory::ParseBudgetFile(RZString(defaultBudgetPath.string().c_str()));

            Razix::Debug::RZLog::Shutdown();
        }
    };

    TEST_F(AssetDBFixture, StartupAndAllocateCameraAsset)
    {
        const rz_asset_handle handle = db.allocateAsset<RZCameraAsset>();
        ASSERT_NE(handle, RAZIX_ASSET_INVALID_HANDLE);

        RZCameraAsset* camera = db.getAssetResourceMutablePtr<RZCameraAsset>(handle);
        ASSERT_NE(camera, nullptr);

        camera->MovementSpeed = 5.0f;
        EXPECT_FLOAT_EQ(camera->MovementSpeed, 5.0f);

        db.releaseAsset<RZCameraAsset>(handle);
    }

    TEST_F(AssetDBFixture, HandlesEncodeHeaderAndPayloadIndices)
    {
        const rz_asset_handle handleA = db.allocateAsset<RZCameraAsset>();
        const rz_asset_handle handleB = db.allocateAsset<RZCameraAsset>();

        ASSERT_NE(handleA, RAZIX_ASSET_INVALID_HANDLE);
        ASSERT_NE(handleB, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_NE(handleA, handleB);

        const u32 headerA  = static_cast<u32>(handleA & RAZIX_ASSET_HOTDATA_MASK);
        const u32 payloadA = static_cast<u32>((handleA & RAZIX_ASSET_PAYLOLAD_INDEX_MASK) >> RAZIX_ASSET_PAYLOAD_SHIFT_INDEX);
        const u32 headerB  = static_cast<u32>(handleB & RAZIX_ASSET_HOTDATA_MASK);
        const u32 payloadB = static_cast<u32>((handleB & RAZIX_ASSET_PAYLOLAD_INDEX_MASK) >> RAZIX_ASSET_PAYLOAD_SHIFT_INDEX);

        EXPECT_LT(headerA, RAZIX_MAX_ASSETS);
        EXPECT_LT(payloadA, RAZIX_MAX_ASSETS);
        EXPECT_LT(headerB, RAZIX_MAX_ASSETS);
        EXPECT_LT(payloadB, RAZIX_MAX_ASSETS);

        EXPECT_NE(headerA, headerB);
        EXPECT_NE(payloadA, payloadB);

        db.releaseAsset<RZCameraAsset>(handleA);
        db.releaseAsset<RZCameraAsset>(handleB);
    }

    TEST_F(AssetDBFixture, ReleaseRecyclesSlots)
    {
        const rz_asset_handle handle1 = db.allocateAsset<RZCameraAsset>();
        ASSERT_NE(handle1, RAZIX_ASSET_INVALID_HANDLE);

        db.releaseAsset<RZCameraAsset>(handle1);

        const rz_asset_handle handle2 = db.allocateAsset<RZCameraAsset>();
        ASSERT_NE(handle2, RAZIX_ASSET_INVALID_HANDLE);

        // At least one of header/payload indices should be reused after release
        const u32 header1  = static_cast<u32>(handle1 & RAZIX_ASSET_HOTDATA_MASK);
        const u32 payload1 = static_cast<u32>((handle1 & RAZIX_ASSET_PAYLOLAD_INDEX_MASK) >> RAZIX_ASSET_PAYLOAD_SHIFT_INDEX);
        const u32 header2  = static_cast<u32>(handle2 & RAZIX_ASSET_HOTDATA_MASK);
        const u32 payload2 = static_cast<u32>((handle2 & RAZIX_ASSET_PAYLOLAD_INDEX_MASK) >> RAZIX_ASSET_PAYLOAD_SHIFT_INDEX);

        EXPECT_TRUE(header1 == header2 || payload1 == payload2);

        db.releaseAsset<RZCameraAsset>(handle2);
    }

}    // namespace Razix
