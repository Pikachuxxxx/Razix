// AssetBaseTests.cpp
// Unit tests for the AssetSystem base classes and enums
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/AssetSystem/RZAssetBase.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace AssetSystem {

        class RZAssetBaseTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Initialize any required setup
            }

            void TearDown() override
            {
                // Clean up any allocated resources
            }
        };

        // Test case for AssetType enum values
        TEST_F(RZAssetBaseTests, AssetTypeEnumValues)
        {
            EXPECT_EQ(static_cast<int>(AssetType::kUndefined), 0) << "kUndefined should be the first enum value.";
            EXPECT_NE(static_cast<int>(AssetType::COUNT), 0) << "COUNT should not be zero.";
            EXPECT_GT(static_cast<int>(AssetType::COUNT), static_cast<int>(AssetType::kUndefined)) << "COUNT should be greater than first enum value.";
        }

        // Test case for AssetType enum completeness
        TEST_F(RZAssetBaseTests, AssetTypeEnumCompleteness)
        {
            // Test that all expected asset types are present
            EXPECT_NO_THROW({
                auto type1 = AssetType::kUndefined;
                auto type2 = AssetType::kTransform;
                auto type3 = AssetType::kCamera;
                auto type4 = AssetType::kLight;
                auto type5 = AssetType::kMaterial;
                auto type6 = AssetType::kPhysicsMaterial;
                auto type7 = AssetType::kMesh;
                auto type8 = AssetType::kTexture;
                auto type9 = AssetType::kAnimation;
                auto type10 = AssetType::kAudio;
                auto type11 = AssetType::kLuaScript;
                auto type12 = AssetType::kAssetRef;
                auto type13 = AssetType::kAssetBuilt;
                auto type14 = AssetType::kUserDataContainer;
                auto count = AssetType::COUNT;
            }) << "All AssetType enum values should be accessible.";
        }

        // Test case for AssetStorageType enum values
        TEST_F(RZAssetBaseTests, AssetStorageTypeEnumValues)
        {
            EXPECT_EQ(static_cast<int>(AssetStorageType::kMemoryBacked), 0) << "kMemoryBacked should be the first enum value.";
            
            EXPECT_NO_THROW({
                auto storage1 = AssetStorageType::kMemoryBacked;
                auto storage2 = AssetStorageType::kGPUBacked;
                auto storage3 = AssetStorageType::kNonMemoryBacked;
            }) << "All AssetStorageType enum values should be accessible.";
        }

        // Test case for AssetStorageType enum ordering
        TEST_F(RZAssetBaseTests, AssetStorageTypeEnumOrdering)
        {
            EXPECT_LT(static_cast<int>(AssetStorageType::kMemoryBacked), 
                     static_cast<int>(AssetStorageType::kGPUBacked)) << "kMemoryBacked should come before kGPUBacked.";
            
            EXPECT_LT(static_cast<int>(AssetStorageType::kGPUBacked), 
                     static_cast<int>(AssetStorageType::kNonMemoryBacked)) << "kGPUBacked should come before kNonMemoryBacked.";
        }

        // Test case for enum size validation
        TEST_F(RZAssetBaseTests, EnumSizeValidation)
        {
            // Ensure AssetType count is reasonable (not corrupted)
            EXPECT_GT(static_cast<int>(AssetType::COUNT), 10) << "AssetType COUNT should be greater than 10.";
            EXPECT_LT(static_cast<int>(AssetType::COUNT), 100) << "AssetType COUNT should be less than 100 (sanity check).";
        }

    }    // namespace AssetSystem
}    // namespace Razix