// Container_QueueTests.cpp
// AI-generated unit tests for the RZQueue container class

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/queue.h"
#include "Razix/Core/Log/RZLog.h"

#include <gtest/gtest.h>
#include <string>
#include <utility>

namespace Razix {

    class RZQueueTests : public ::testing::Test
    {
    };

    // ------------------------------------------------------------
    // Construction Tests
    // ------------------------------------------------------------
    TEST_F(RZQueueTests, DefaultConstruction)
    {
        RZQueue<int> queue;
        EXPECT_EQ(queue.size(), 0);
        EXPECT_EQ(queue.capacity(), RZ_DEFAULT_QUEUE_CAPACITY);
        EXPECT_TRUE(queue.empty());
    }

    TEST_F(RZQueueTests, CustomCapacityConstruction)
    {
        RZQueue<int> queue(128);
        EXPECT_EQ(queue.size(), 0);
        EXPECT_EQ(queue.capacity(), 128);
        EXPECT_TRUE(queue.empty());
    }

    // ------------------------------------------------------------
    // Push / Pop / Front / Back Tests
    // ------------------------------------------------------------
    TEST_F(RZQueueTests, PushPopLvalue)
    {
        RZQueue<int> queue;
        int          a = 10;
        int          b = 20;
        queue.push(a);
        queue.push(b);
        EXPECT_EQ(queue.size(), 2);
        EXPECT_EQ(queue.front(), 10);
        EXPECT_EQ(queue.back(), 20);

        EXPECT_EQ(queue.pop(), 10);
        EXPECT_EQ(queue.pop(), 20);
        EXPECT_TRUE(queue.empty());
    }

    TEST_F(RZQueueTests, PushPopRvalue)
    {
        RZQueue<std::string> queue;
        queue.push("Hello");
        queue.push(std::string("World"));
        EXPECT_EQ(queue.size(), 2);
        EXPECT_EQ(queue.front(), "Hello");
        EXPECT_EQ(queue.back(), "World");

        EXPECT_EQ(queue.pop(), "Hello");
        EXPECT_EQ(queue.pop(), "World");
        EXPECT_TRUE(queue.empty());
    }

    TEST_F(RZQueueTests, Emplace)
    {
        RZQueue<std::pair<int, std::string>> queue;
        queue.emplace(1, "one");
        queue.emplace(2, "two");

        EXPECT_EQ(queue.size(), 2);
        EXPECT_EQ(queue.front().first, 1);
        EXPECT_EQ(queue.back().second, "two");

        auto first = queue.pop();
        EXPECT_EQ(first.first, 1);
        EXPECT_EQ(first.second, "one");
    }

    // ------------------------------------------------------------
    // Access Tests
    // ------------------------------------------------------------
    TEST_F(RZQueueTests, FrontBackAccess)
    {
        RZQueue<int> queue;
        queue.push(5);
        queue.push(6);
        EXPECT_EQ(queue.front(), 5);
        EXPECT_EQ(queue.back(), 6);

        queue.front() = 50;
        queue.back()  = 60;
        EXPECT_EQ(queue.front(), 50);
        EXPECT_EQ(queue.back(), 60);

        const auto& const_queue = queue;
        EXPECT_EQ(const_queue.front(), 50);
        EXPECT_EQ(const_queue.back(), 60);
    }

    // ------------------------------------------------------------
    // Capacity and Clear Tests
    // ------------------------------------------------------------
    TEST_F(RZQueueTests, SizeCapacity)
    {
        RZQueue<int> queue(4);
        for (int i = 0; i < 4; ++i)
            queue.push(i);
        EXPECT_EQ(queue.size(), 4);
        EXPECT_GE(queue.capacity(), 4);
    }

    TEST_F(RZQueueTests, Empty)
    {
        RZQueue<int> queue;
        EXPECT_TRUE(queue.empty());
        queue.push(42);
        EXPECT_FALSE(queue.empty());
        queue.pop();
        EXPECT_TRUE(queue.empty());
    }

    TEST_F(RZQueueTests, Clear)
    {
        RZQueue<int> queue;
        queue.push(1);
        queue.push(2);
        queue.push(3);
        EXPECT_EQ(queue.size(), 3);
        queue.clear();
        EXPECT_EQ(queue.size(), 0);
        EXPECT_TRUE(queue.empty());
    }

    TEST_F(RZQueueTests, Reserve)
    {
        RZQueue<int> queue;
        queue.reserve(100);
        EXPECT_GE(queue.capacity(), 100);
        EXPECT_EQ(queue.size(), 0);
        queue.push(1);
        queue.reserve(50);    // Should not shrink
        EXPECT_GE(queue.capacity(), 100);
    }

    // ------------------------------------------------------------
    // Iteration Tests
    // ------------------------------------------------------------
    TEST_F(RZQueueTests, Iterators)
    {
        RZQueue<int> queue;
        queue.push(1);
        queue.push(2);
        queue.push(3);

        auto it = queue.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
        ++it;
        EXPECT_EQ(it, queue.end());

        const auto& const_queue = queue;
        auto        cit         = const_queue.begin();
        EXPECT_EQ(*cit, 1);
        ++cit;
        EXPECT_EQ(*cit, 2);
    }

    // ------------------------------------------------------------
    // Move Semantics Tests
    // ------------------------------------------------------------
    TEST_F(RZQueueTests, MoveConstructor)
    {
        RZQueue<int> original;
        original.push(10);
        original.push(20);

        RZQueue<int> moved(std::move(original));
        EXPECT_EQ(moved.size(), 2);
        EXPECT_EQ(moved.front(), 10);
        EXPECT_EQ(moved.back(), 20);
        EXPECT_TRUE(original.empty());
    }

    TEST_F(RZQueueTests, MoveAssignment)
    {
        RZQueue<int> original;
        original.push(5);
        original.push(6);

        RZQueue<int> target;
        target = std::move(original);
        EXPECT_EQ(target.size(), 2);
        EXPECT_EQ(target.front(), 5);
        EXPECT_EQ(target.back(), 6);
        EXPECT_TRUE(original.empty());
    }

    // ------------------------------------------------------------
    // Destroy & Edge Case Tests
    // ------------------------------------------------------------
    TEST_F(RZQueueTests, Destroy)
    {
        RZQueue<int> queue;
        queue.push(1);
        queue.push(2);
        EXPECT_EQ(queue.size(), 2);
        queue.destroy();
        EXPECT_EQ(queue.size(), 0);
        EXPECT_EQ(queue.capacity(), 0);
        EXPECT_TRUE(queue.empty());
    }

    TEST_F(RZQueueTests, PopEmptyQueue)
    {
        RZQueue<int> queue;
        EXPECT_TRUE(queue.empty());
        // Uncomment if you decide to add runtime check:
        // EXPECT_DEATH(queue.pop(), ".*");
    }

    // ------------------------------------------------------------
    // Custom Type Tests
    // ------------------------------------------------------------
    struct TestStruct
    {
        int         x;
        std::string name;
        TestStruct(int val, std::string n)
            : x(val), name(std::move(n)) {}
    };

    TEST_F(RZQueueTests, CustomType)
    {
        RZQueue<TestStruct> queue;
        queue.emplace(10, "ten");
        queue.push(TestStruct(20, "twenty"));
        EXPECT_EQ(queue.size(), 2);
        EXPECT_EQ(queue.front().x, 10);
        EXPECT_EQ(queue.back().name, "twenty");

        auto f = queue.pop();
        EXPECT_EQ(f.x, 10);
        EXPECT_EQ(queue.front().x, 20);
    }

}    // namespace Razix
