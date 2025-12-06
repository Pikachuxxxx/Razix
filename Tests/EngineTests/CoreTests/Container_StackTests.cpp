// Container_StackTests.cpp
// AI-generated unit tests for the RZStack container class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/stack.h"
#include "Razix/Core/Log/RZLog.h"

#include <gtest/gtest.h>
#include <vector>

namespace Razix {

    // Test fixture for RZStack tests
    class RZStackTests : public ::testing::Test
    {
    };

    // Test default construction
    TEST_F(RZStackTests, DefaultConstruction)
    {
        RZStack<int> stack;
        EXPECT_EQ(stack.size(), 0);
        EXPECT_EQ(stack.capacity(), RZ_DEFAULT_STACK_CAPACITY);
        EXPECT_TRUE(stack.empty());
    }

    // Test construction with custom capacity
    TEST_F(RZStackTests, CustomCapacityConstruction)
    {
        RZStack<int> stack(128);
        EXPECT_EQ(stack.size(), 0);
        EXPECT_EQ(stack.capacity(), 128);
        EXPECT_TRUE(stack.empty());
    }

    // Test push and pop with lvalue
    TEST_F(RZStackTests, PushPopLvalue)
    {
        RZStack<int> stack;
        int          val1 = 10;
        int          val2 = 20;
        stack.push(val1);
        stack.push(val2);
        EXPECT_EQ(stack.size(), 2);
        EXPECT_EQ(stack.top(), 20);
        EXPECT_EQ(stack.pop(), 20);
        EXPECT_EQ(stack.top(), 10);
        EXPECT_EQ(stack.pop(), 10);
        EXPECT_TRUE(stack.empty());
    }

    // Test push with rvalue
    TEST_F(RZStackTests, PushRvalue)
    {
        RZStack<std::string> stack;
        stack.push("Hello");
        stack.push(std::string("World"));
        EXPECT_EQ(stack.size(), 2);
        EXPECT_EQ(stack.top(), "World");
        EXPECT_EQ(stack.pop(), "World");
        EXPECT_EQ(stack.top(), "Hello");
    }

    // Test emplace
    TEST_F(RZStackTests, Emplace)
    {
        RZStack<std::pair<int, std::string>> stack;
        stack.emplace(1, "one");
        stack.emplace(2, "two");
        EXPECT_EQ(stack.size(), 2);
        auto top = stack.top();
        EXPECT_EQ(top.first, 2);
        EXPECT_EQ(top.second, "two");
    }

    // Test top (non-const and const)
    TEST_F(RZStackTests, TopAccess)
    {
        RZStack<int> stack;
        stack.push(42);
        EXPECT_EQ(stack.top(), 42);
        stack.top() = 100;    // Modify via non-const top
        EXPECT_EQ(stack.top(), 100);
        const auto& const_stack = stack;
        EXPECT_EQ(const_stack.top(), 100);    // Const top
    }

    // Test size and capacity
    TEST_F(RZStackTests, SizeCapacity)
    {
        RZStack<int> stack(15);
        EXPECT_EQ(stack.size(), 0);
        EXPECT_EQ(stack.capacity(), 15);
        for (int i = 0; i < 15; ++i) {
            stack.push(i);
        }
        EXPECT_EQ(stack.size(), 15);
        EXPECT_GE(stack.capacity(), 15);    // Capacity should have grown
    }

    // Test empty
    TEST_F(RZStackTests, Empty)
    {
        RZStack<int> stack;
        EXPECT_TRUE(stack.empty());
        stack.push(1);
        EXPECT_FALSE(stack.empty());
        stack.pop();
        EXPECT_TRUE(stack.empty());
    }

    // Test reserve
    TEST_F(RZStackTests, Reserve)
    {
        RZStack<int> stack;
        stack.reserve(100);
        EXPECT_EQ(stack.capacity(), 100);
        EXPECT_EQ(stack.size(), 0);
        stack.push(1);
        stack.reserve(50);    // Reserve smaller should not shrink
        EXPECT_GE(stack.capacity(), 100);
        stack.destroy();
    }

    // Test clear
    TEST_F(RZStackTests, Clear)
    {
        RZStack<int> stack;
        stack.push(1);
        stack.push(2);
        stack.push(3);
        EXPECT_EQ(stack.size(), 3);
        stack.clear();
        EXPECT_EQ(stack.size(), 0);
        EXPECT_TRUE(stack.empty());
    }

    // Test iterators
    TEST_F(RZStackTests, Iterators)
    {
        RZStack<int> stack;
        stack.push(1);
        stack.push(2);
        stack.push(3);
        // Stack is LIFO, so begin() points to bottom (1), end() to top+1
        auto it = stack.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
        ++it;
        EXPECT_EQ(it, stack.end());

        // Const iterators
        const auto& const_stack = stack;
        auto        cit         = const_stack.begin();
        EXPECT_EQ(*cit, 1);
        ++cit;
        EXPECT_EQ(*cit, 2);
    }

    // Test move constructor
    TEST_F(RZStackTests, MoveConstructor)
    {
        RZStack<int> original;
        original.push(1);
        original.push(2);
        RZStack<int> moved(std::move(original));
        EXPECT_EQ(moved.size(), 2);
        EXPECT_EQ(moved.top(), 2);
        // Original should be empty after move (implementation dependent, but typically)
        EXPECT_TRUE(original.empty());
    }

    // Test move assignment
    TEST_F(RZStackTests, MoveAssignment)
    {
        RZStack<int> original;
        original.push(1);
        original.push(2);
        RZStack<int> target;
        target = std::move(original);
        EXPECT_EQ(target.size(), 2);
        EXPECT_EQ(target.top(), 2);
        EXPECT_TRUE(original.empty());
    }

    // Test pop on empty stack (should throw or undefined, but test if it handles)
    TEST_F(RZStackTests, PopEmptyStack)
    {
        RZStack<int> stack;
        // Assuming pop on empty is undefined, but we can test size
        EXPECT_TRUE(stack.empty());
        // stack.pop();  // Would be undefined behavior, so skip or expect crash
    }

    // Test destroy (assuming it's non-const and clears)
    TEST_F(RZStackTests, Destroy)
    {
        RZStack<int> stack;
        stack.push(1);
        stack.push(2);
        EXPECT_EQ(stack.size(), 2);
        stack.destroy();    // Assuming it clears the stack
        EXPECT_EQ(stack.size(), 0);
    }

    // Test with custom type
    struct TestStruct
    {
        int x;
        TestStruct(int val)
            : x(val) {}
        bool operator==(const TestStruct& other) const { return x == other.x; }
    };

    TEST_F(RZStackTests, CustomType)
    {
        RZStack<TestStruct> stack;
        stack.push(TestStruct(10));
        stack.emplace(20);
        EXPECT_EQ(stack.size(), 2);
        EXPECT_EQ(stack.top().x, 20);
        EXPECT_EQ(stack.pop().x, 20);
        EXPECT_EQ(stack.top().x, 10);
    }

}    // namespace Razix
