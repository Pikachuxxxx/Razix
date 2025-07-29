// AssetFileSpecTests.cpp
// Unit tests for AssetSystem file specifications and constants
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/AssetSystem/RZAssetFileSpec.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace AssetSystem {

        class RZAssetFileSpecTests : public ::testing::Test
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

        // Test case for asset version constants
        TEST_F(RZAssetFileSpecTests, AssetVersionConstants)
        {
            EXPECT_EQ(RAZIX_ASSET_VERSION_V1, 0x1) << "RAZIX_ASSET_VERSION_V1 should be 0x1.";
            EXPECT_EQ(RAZIX_ASSET_VERSION_V2, 0x2) << "RAZIX_ASSET_VERSION_V2 should be 0x2.";
            EXPECT_EQ(RAZIX_ASSET_VERSION, RAZIX_ASSET_VERSION_V2) << "Current version should be V2.";
        }

        // Test case for magic number size constant
        TEST_F(RZAssetFileSpecTests, MagicNumberSize)
        {
            EXPECT_EQ(MAGIC_NUM_SZ, 18) << "MAGIC_NUM_SZ should be 18.";
            EXPECT_GT(MAGIC_NUM_SZ, 0) << "MAGIC_NUM_SZ should be positive.";
        }

        // Test case for AssetType enum values
        TEST_F(RZAssetFileSpecTests, AssetTypeEnumValues)
        {
            EXPECT_EQ(static_cast<uint8_t>(ASSET_MESH), 0) << "ASSET_MESH should be 0.";
            EXPECT_EQ(static_cast<uint8_t>(ASSET_MATERIAL), 1) << "ASSET_MATERIAL should be 1.";
        }

        // Test case for AssetType enum ordering
        TEST_F(RZAssetFileSpecTests, AssetTypeEnumOrdering)
        {
            EXPECT_LT(static_cast<uint8_t>(ASSET_MESH), 
                     static_cast<uint8_t>(ASSET_MATERIAL)) << "ASSET_MESH should come before ASSET_MATERIAL.";
        }

        // Test case for AssetType enum type safety
        TEST_F(RZAssetFileSpecTests, AssetTypeTypeSafety)
        {
            // Test that AssetType values fit in uint8_t range
            EXPECT_LE(static_cast<uint8_t>(ASSET_MESH), 255) << "ASSET_MESH should fit in uint8_t.";
            EXPECT_LE(static_cast<uint8_t>(ASSET_MATERIAL), 255) << "ASSET_MATERIAL should fit in uint8_t.";
            
            // Test that enum values are distinct
            EXPECT_NE(static_cast<uint8_t>(ASSET_MESH), 
                     static_cast<uint8_t>(ASSET_MATERIAL)) << "AssetType enum values should be distinct.";
        }

        // Test case for version comparison
        TEST_F(RZAssetFileSpecTests, VersionComparison)
        {
            EXPECT_LT(RAZIX_ASSET_VERSION_V1, RAZIX_ASSET_VERSION_V2) << "V1 should be less than V2.";
            EXPECT_EQ(RAZIX_ASSET_VERSION, RAZIX_ASSET_VERSION_V2) << "Current version should be V2.";
        }

        // Test case for constant ranges
        TEST_F(RZAssetFileSpecTests, ConstantRanges)
        {
            // Ensure constants are in reasonable ranges
            EXPECT_LT(RAZIX_ASSET_VERSION, 256) << "Asset version should be less than 256.";
            EXPECT_GT(MAGIC_NUM_SZ, 4) << "Magic number size should be reasonable (>4).";
            EXPECT_LT(MAGIC_NUM_SZ, 64) << "Magic number size should be reasonable (<64).";
        }

    }    // namespace AssetSystem
}    // namespace Razix