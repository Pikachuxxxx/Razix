// AssetBuilderTests.cpp
// Unit tests for the RZAssetBuilder class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/AssetSystem/RZAssetBuilder.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace AssetSystem {

        class RZAssetBuilderTests : public ::testing::Test
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

        // Test case for basic instantiation
        TEST_F(RZAssetBuilderTests, BasicInstantiation)
        {
            // Test that RZAssetBuilder can be instantiated
            EXPECT_NO_THROW({
                RZAssetBuilder builder;
            }) << "RZAssetBuilder should be instantiable.";
        }

        // Test case for default constructor
        TEST_F(RZAssetBuilderTests, DefaultConstructor)
        {
            RZAssetBuilder builder;
            // Since the class is currently empty, just ensure it compiles and runs
            SUCCEED() << "RZAssetBuilder default constructor works.";
        }

        // Test case for copy constructor (if applicable)
        TEST_F(RZAssetBuilderTests, CopyConstructor)
        {
            RZAssetBuilder builder1;
            
            EXPECT_NO_THROW({
                RZAssetBuilder builder2(builder1);
            }) << "RZAssetBuilder copy constructor should work.";
        }

        // Test case for assignment operator (if applicable)
        TEST_F(RZAssetBuilderTests, AssignmentOperator)
        {
            RZAssetBuilder builder1;
            RZAssetBuilder builder2;
            
            EXPECT_NO_THROW({
                builder2 = builder1;
            }) << "RZAssetBuilder assignment operator should work.";
        }

        // Test case for destructor behavior
        TEST_F(RZAssetBuilderTests, DestructorBehavior)
        {
            // Test that destruction doesn't cause issues
            EXPECT_NO_THROW({
                auto* builder = new RZAssetBuilder();
                delete builder;
            }) << "RZAssetBuilder destructor should work without issues.";
        }

    }    // namespace AssetSystem
}    // namespace Razix