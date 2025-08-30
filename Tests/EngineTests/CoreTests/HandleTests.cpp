// HandleTests.cpp
// AI-generated unit tests for the rz_handle C API
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

    // Test: Default Invalid Handle Creation
    TEST_F(RZHandleTests, DefaultInvalidHandle)
    {
        rz_handle handle = rz_handle_make_invalid();
        EXPECT_EQ(rz_handle_get_index(&handle), 0);
        EXPECT_EQ(rz_handle_get_generation(&handle), 0);
        EXPECT_FALSE(rz_handle_is_valid(&handle)) << "Default handle should be invalid.";
    }

    // Test: Custom Index and Generation Creation
    TEST_F(RZHandleTests, CustomIndexAndGeneration)
    {
        rz_handle handle = rz_handle_create(5, 10);

        EXPECT_EQ(rz_handle_get_index(&handle), 5);
        EXPECT_EQ(rz_handle_get_generation(&handle), 10);
        EXPECT_TRUE(rz_handle_is_valid(&handle)) << "Handle with non-zero generation should be valid.";
    }

    // Test: Setting Index and Generation
    TEST_F(RZHandleTests, SetIndexAndGeneration)
    {
        rz_handle handle = rz_handle_make_invalid();
        rz_handle_set_index(&handle, 7);
        rz_handle_set_generation(&handle, 15);

        EXPECT_EQ(rz_handle_get_index(&handle), 7);
        EXPECT_EQ(rz_handle_get_generation(&handle), 15);
        EXPECT_TRUE(rz_handle_is_valid(&handle)) << "Handle with non-zero generation should be valid.";
    }

    // Test: Copy Semantics (struct copy)
    TEST_F(RZHandleTests, CopySemantics)
    {
        rz_handle handle1 = rz_handle_create(2, 7);
        rz_handle handle2 = handle1; // struct copy

        EXPECT_EQ(rz_handle_get_index(&handle2), 2);
        EXPECT_EQ(rz_handle_get_generation(&handle2), 7);
        EXPECT_TRUE(rz_handle_is_valid(&handle2));
    }

    // Test: Assignment
    TEST_F(RZHandleTests, Assignment)
    {
        rz_handle handle1 = rz_handle_create(3, 8);
        rz_handle handle2 = rz_handle_make_invalid();
        
        handle2 = handle1; // struct assignment

        EXPECT_EQ(rz_handle_get_index(&handle2), 3);
        EXPECT_EQ(rz_handle_get_generation(&handle2), 8);
        EXPECT_TRUE(rz_handle_is_valid(&handle2));
    }

    // Test: Equality Function
    TEST_F(RZHandleTests, EqualityFunction)
    {
        rz_handle handle1 = rz_handle_create(2, 3);
        rz_handle handle2 = rz_handle_create(2, 5); // Same index, different generation

        EXPECT_TRUE(rz_handle_equals(&handle1, &handle2)) << "Handles with the same index should be equal (C API compares only index).";
    }

    // Test: Inequality Function
    TEST_F(RZHandleTests, InequalityFunction)
    {
        rz_handle handle1 = rz_handle_create(1, 3);
        rz_handle handle2 = rz_handle_create(2, 3);

        EXPECT_TRUE(rz_handle_not_equals(&handle1, &handle2)) << "Handles with different indices should not be equal.";
    }

    // Test: C++ Operator Equality (compares both index and generation)
    TEST_F(RZHandleTests, CppOperatorEquality)
    {
        rz_handle handle1 = rz_handle_create(2, 3);
        rz_handle handle2 = rz_handle_create(2, 3);
        rz_handle handle3 = rz_handle_create(2, 4); // Same index, different generation

        EXPECT_TRUE(handle1 == handle2) << "Handles with same index and generation should be equal.";
        EXPECT_FALSE(handle1 == handle3) << "Handles with same index but different generation should not be equal.";
    }

    // Test: Handle Validity
    TEST_F(RZHandleTests, HandleValidity)
    {
        rz_handle handle = rz_handle_make_invalid();
        EXPECT_FALSE(rz_handle_is_valid(&handle)) << "Default handle should be invalid.";

        rz_handle_set_generation(&handle, 1);
        EXPECT_TRUE(rz_handle_is_valid(&handle)) << "Handle with non-zero generation should be valid.";
    }

    // Test: Handle Destruction
    TEST_F(RZHandleTests, HandleDestruction)
    {
        rz_handle handle = rz_handle_create(5, 10);
        EXPECT_TRUE(rz_handle_is_valid(&handle));

        rz_handle_destroy(&handle);
        EXPECT_EQ(rz_handle_get_index(&handle), 0);
        EXPECT_EQ(rz_handle_get_generation(&handle), 0);
        EXPECT_FALSE(rz_handle_is_valid(&handle)) << "Destroyed handle should be invalid.";
    }

    // Test: Multiple Handle Operations
    TEST_F(RZHandleTests, MultipleHandleOperations)
    {
        rz_handle handle1 = rz_handle_create(1, 1);
        rz_handle handle2 = rz_handle_create(2, 2);
        rz_handle handle3 = rz_handle_create(3, 3);

        EXPECT_TRUE(rz_handle_not_equals(&handle1, &handle2));
        EXPECT_TRUE(rz_handle_not_equals(&handle2, &handle3));
        EXPECT_TRUE(rz_handle_not_equals(&handle1, &handle3));

        // Test modification
        rz_handle_set_index(&handle1, 2);
        EXPECT_TRUE(rz_handle_equals(&handle1, &handle2)) << "After setting handle1 index to 2, it should equal handle2 (C API compares only index).";
    }
}    // namespace Razix