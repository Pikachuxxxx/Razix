// HandleTests.cpp
// AI-generated unit tests for the RZHandle class
#include <Razix/Core/RZHandle.h>
#include <gtest/gtest.h>

namespace Razix {
    class RZHandleTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Setup code before each test runs (if needed)
        }

        void TearDown() override
        {
            // Cleanup code after each test runs (if needed)
        }
    };

    // Test: Default Constructor
    TEST_F(RZHandleTests, DefaultConstructor)
    {
        RZHandle<int> handle;
        EXPECT_EQ(handle.getIndex(), 0);
        EXPECT_EQ(handle.getGeneration(), 0);
        EXPECT_FALSE(handle.isValid()) << "Default handle should be invalid.";
    }

    // Test: Custom Index and Generation
    TEST_F(RZHandleTests, CustomIndexAndGeneration)
    {
        RZHandle<int> handle;
        handle.setIndex(5);
        handle.setGeneration(10);

        EXPECT_EQ(handle.getIndex(), 5);
        EXPECT_EQ(handle.getGeneration(), 10);
        EXPECT_TRUE(handle.isValid()) << "Handle with non-zero generation should be valid.";
    }

    // Test: Copy Constructor
    TEST_F(RZHandleTests, CopyConstructor)
    {
        RZHandle<int> handle1;
        handle1.setIndex(2);
        handle1.setGeneration(7);

        RZHandle<int> handle2(handle1);

        EXPECT_EQ(handle2.getIndex(), 2);
        EXPECT_EQ(handle2.getGeneration(), 7);
    }

    // Test: Copy Assignment
    TEST_F(RZHandleTests, CopyAssignment)
    {
        RZHandle<int> handle1;
        handle1.setIndex(3);
        handle1.setGeneration(8);

        RZHandle<int> handle2;
        handle2 = handle1;

        EXPECT_EQ(handle2.getIndex(), 3);
        EXPECT_EQ(handle2.getGeneration(), 8);
    }

    // Test: Move Constructor
    TEST_F(RZHandleTests, MoveConstructor)
    {
        RZHandle<int> handle1;
        handle1.setIndex(1);
        handle1.setGeneration(5);

        RZHandle<int> handle2(std::move(handle1));

        EXPECT_EQ(handle2.getIndex(), 1);
        EXPECT_EQ(handle2.getGeneration(), 5);
    }

    // Test: Move Assignment
    TEST_F(RZHandleTests, MoveAssignment)
    {
        RZHandle<int> handle1;
        handle1.setIndex(4);
        handle1.setGeneration(6);

        RZHandle<int> handle2;
        handle2 = std::move(handle1);

        EXPECT_EQ(handle2.getIndex(), 4);
        EXPECT_EQ(handle2.getGeneration(), 6);
    }

    // Test: Equality Operator
    TEST_F(RZHandleTests, EqualityOperator)
    {
        RZHandle<int> handle1;
        handle1.setIndex(2);
        handle1.setGeneration(3);

        RZHandle<int> handle2;
        handle2.setIndex(2);
        handle2.setGeneration(3);

        EXPECT_TRUE(handle1 == handle2) << "Handles with the same index should be equal.";
    }

    // Test: Inequality Operator
    TEST_F(RZHandleTests, InequalityOperator)
    {
        RZHandle<int> handle1;
        handle1.setIndex(1);

        RZHandle<int> handle2;
        handle2.setIndex(2);

        EXPECT_TRUE(handle1 != handle2) << "Handles with different indices should not be equal.";
    }

    // Test: Handle Validity
    TEST_F(RZHandleTests, HandleValidity)
    {
        RZHandle<int> handle;
        EXPECT_FALSE(handle.isValid()) << "Default handle should be invalid.";

        handle.setGeneration(1);
        EXPECT_TRUE(handle.isValid()) << "Handle with non-zero generation should be valid.";
    }
}    // namespace Razix
