// AssetPoolTests.cpp
// Unit tests for asset pools and asset budget helpers

#include "Razix/AssetSystem/RZAssetDB.h"
#include "Razix/AssetSystem/RZAssetPool.h"
#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace {

        struct DummyAsset
        {
            u32 payload = 0;
        };

        static size_t align_up(size_t value, size_t alignment)
        {
            const size_t mask = alignment - 1;
            return (value + mask) & ~mask;
        }

    }    // namespace

    class AssetPoolFixture : public ::testing::Test
    {
    protected:
        Memory::RZHeapAllocator allocator;
        RZAssetPool<DummyAsset> pool;
        void*                   backing  = nullptr;
        const u32               capacity = 4;

        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();

            allocator.init(64 * 1024 * 1024);
            const size_t slotBytes    = sizeof(DummyAsset);
            const size_t freelistSize = sizeof(u32) * capacity;
            backing                   = allocator.allocate(slotBytes * capacity + freelistSize);
            pool.init(backing, capacity);
        }

        void TearDown() override
        {
            pool.destroy();
            allocator.deallocate(backing);
            allocator.shutdown();

            Razix::Debug::RZLog::Shutdown();
        }
    };

    class AssetHeaderPoolFixture : public ::testing::Test
    {
    protected:
        Memory::RZHeapAllocator allocator;
        RZAssetHeaderPool       pool;
        void*                   backing  = nullptr;
        const u32               capacity = 4;

        void SetUp() override
        {
            Debug::RZLog::StartUp();    

            allocator.init(128 * 1024 * 1024);

            size_t bufferSize = sizeof(RZAsset) * capacity;
            // bufferSize        = align_up(bufferSize, alignof(RZAssetColdData));
            bufferSize += sizeof(RZAssetColdData) * capacity;
            // bufferSize = align_up(bufferSize, alignof(u32));
            bufferSize += sizeof(u32) * capacity;

            backing = allocator.allocate(bufferSize);
            pool.init(backing, capacity);
        }

        void TearDown() override
        {
            pool.destroy();
            allocator.deallocate(backing);
            allocator.shutdown();

            Debug::RZLog::Shutdown();
        }
    };

    TEST_F(AssetPoolFixture, AllocateReleaseRespectsCapacity)
    {
        EXPECT_EQ(pool.getCapacity(), capacity);

        const u32 i0 = pool.allocate(RZAssetType::kMesh);
        const u32 i1 = pool.allocate(RZAssetType::kMesh);
        const u32 i2 = pool.allocate(RZAssetType::kMesh);
        const u32 i3 = pool.allocate(RZAssetType::kMesh);

        EXPECT_NE(i0, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_NE(i1, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_NE(i2, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_NE(i3, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_EQ(pool.getCount(), capacity);

        const u32 overflow = pool.allocate(RZAssetType::kMesh);
        EXPECT_EQ(overflow, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_EQ(pool.getCount(), capacity);

        pool.release(i1);
        EXPECT_EQ(pool.getCount(), capacity - 1);

        const u32 reused = pool.allocate(RZAssetType::kMesh);
        EXPECT_NE(reused, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_LT(reused, capacity);
    }

    TEST_F(AssetPoolFixture, GetReturnsNullWhenOutOfBounds)
    {
        const u32 idx = pool.allocate(RZAssetType::kMesh);
        ASSERT_NE(idx, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_NE(pool.get(idx), nullptr);
        EXPECT_EQ(pool.get(capacity), nullptr);
    }

    TEST_F(AssetHeaderPoolFixture, AllocateReleaseReusesSlots)
    {
        const u32 h0 = pool.allocate(RZAssetType::kTexture);
        const u32 h1 = pool.allocate(RZAssetType::kTexture);

        EXPECT_NE(h0, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_NE(h1, RAZIX_ASSET_INVALID_HANDLE);
        EXPECT_EQ(pool.getCount(), 2u);

        const rz_asset_handle handle0 = static_cast<rz_asset_handle>(h0);
        EXPECT_NE(pool.get(handle0), nullptr);
        EXPECT_NE(pool.getColdData(handle0), nullptr);

        pool.release(h0);
        EXPECT_EQ(pool.getCount(), 1u);

        const u32 h2 = pool.allocate(RZAssetType::kTexture);
        EXPECT_EQ(h2, h0);    // freelist should reuse the released slot
        EXPECT_EQ(pool.getCount(), 2u);
    }

    TEST(AssetDBTests, ComputeMinBudgetIsPositiveAndMonotonic)
    {
        const u64 budget1 = RZAssetDB::ComputeMinPoolBudgetBytesForMaxAssets(1);
        const u64 budget2 = RZAssetDB::ComputeMinPoolBudgetBytesForMaxAssets(2);

        EXPECT_GT(budget1, 0u);
        EXPECT_GT(budget2, budget1);

        const u64 headerBudget1 = RZAssetDB::ComputeMinHeaderBudgetBytesForMaxAssets(1);
        const u64 headerBudget2 = RZAssetDB::ComputeMinHeaderBudgetBytesForMaxAssets(2);

        EXPECT_GT(headerBudget1, 0u);
        EXPECT_GT(headerBudget2, headerBudget1);
    }
}    // namespace Razix
