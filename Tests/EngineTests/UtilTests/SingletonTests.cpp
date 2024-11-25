// CommandLineParserTests.cpp
// AI-generated unit tests for the RZCommandLineParser class

#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include <Razix/Core/Log/RZLog.h>

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Utilities/TRZSingleton.h"

#include <gtest/gtest.h>

namespace Razix {

    // Test fixture for RZSingleton
    class RZSingletonTests : public ::testing::Test
    {
    protected:
        void SetUp() override {}
        void TearDown() override {}
    };

    // Test class to derive from RZSingleton
    class DerivedSingleton : public RZSingleton<DerivedSingleton>
    {
    public:
        void setNumber(int value) { number = value; }
        int  getNumber() const { return number; }

    private:
        int number = 0;    // Default number
    };

    TEST_F(RZSingletonTests, SingletonReferenceStability)
    {
        auto& instance1 = DerivedSingleton::Get();
        auto& instance2 = DerivedSingleton::Get();

        // Check that the references are the same
        EXPECT_EQ(&instance1, &instance2);
    }

    TEST_F(RZSingletonTests, SingletonStatePersistence)
    {
        auto& instance = DerivedSingleton::Get();

        // Set a value and ensure it persists
        instance.setNumber(42);
        EXPECT_EQ(instance.getNumber(), 42);

        // Retrieve the singleton again and check the state
        auto& retrievedInstance = DerivedSingleton::Get();
        EXPECT_EQ(retrievedInstance.getNumber(), 42);
    }

    TEST_F(RZSingletonTests, MultipleStateChanges)
    {
        auto& instance = DerivedSingleton::Get();

        // Change state multiple times and verify
        instance.setNumber(10);
        EXPECT_EQ(instance.getNumber(), 10);

        instance.setNumber(20);
        EXPECT_EQ(instance.getNumber(), 20);

        // Verify the singleton state after changes
        auto& retrievedInstance = DerivedSingleton::Get();
        EXPECT_EQ(retrievedInstance.getNumber(), 20);
    }
}    // namespace Razix