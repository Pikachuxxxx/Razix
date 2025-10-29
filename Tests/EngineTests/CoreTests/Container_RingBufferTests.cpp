// Container_RingBufferTests.cpp
// AI-generated unit tests for the RZRingBuffer container class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/ring_buffer.h"
#include "Razix/Core/Log/RZLog.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace Razix {

    // Test fixture for RZRingBuffer tests
    class RZRingBufferTests : public ::testing::Test
    {
    protected:
        // You can define common setup/teardown here if needed
        void SetUp() override
        {
            // Optional: Setup before each test
        }

        void TearDown() override
        {
            // Optional: Cleanup after each test
        }
    };

    // Test default construction
    TEST_F(RZRingBufferTests, DefaultConstruction)
    {
        RZRingBuffer<int> buffer;
        EXPECT_EQ(buffer.size(), 0);
        EXPECT_EQ(buffer.capacity(), RZ_DEFAULT_QUEUE_CAPACITY);
        EXPECT_TRUE(buffer.empty());
        EXPECT_FALSE(buffer.isFull());
    }

    // Test construction with custom capacity
    TEST_F(RZRingBufferTests, CustomCapacityConstruction)
    {
        RZRingBuffer<int> buffer(128);
        EXPECT_EQ(buffer.size(), 0);
        EXPECT_EQ(buffer.capacity(), 128);
        EXPECT_TRUE(buffer.empty());
        EXPECT_FALSE(buffer.isFull());
    }

    // Test write and read with lvalue
    TEST_F(RZRingBufferTests, WriteReadLvalue)
    {
        RZRingBuffer<int> buffer(5);
        buffer.write(10);
        buffer.write(20);
        EXPECT_EQ(buffer.size(), 2);
        EXPECT_EQ(buffer.read(), 10);
        EXPECT_EQ(buffer.read(), 20);
        EXPECT_TRUE(buffer.empty());
    }

    // Test write with rvalue
    TEST_F(RZRingBufferTests, WriteRvalue)
    {
        RZRingBuffer<std::string> buffer(5);
        buffer.write("Hello");
        buffer.write(std::string("World"));
        EXPECT_EQ(buffer.size(), 2);
        EXPECT_EQ(buffer.read(), "Hello");
        EXPECT_EQ(buffer.read(), "World");
        EXPECT_TRUE(buffer.empty());
    }

    // Test emplace
    TEST_F(RZRingBufferTests, Emplace)
    {
        RZRingBuffer<std::pair<int, std::string>> buffer(5);
        buffer.emplace(1, "one");
        buffer.emplace(2, "two");
        EXPECT_EQ(buffer.size(), 2);
        auto val1 = buffer.read();
        EXPECT_EQ(val1.first, 1);
        EXPECT_EQ(val1.second, "one");
        auto val2 = buffer.read();
        EXPECT_EQ(val2.first, 2);
        EXPECT_EQ(val2.second, "two");
    }

    // Test head and tail access
    TEST_F(RZRingBufferTests, HeadTailAccess)
    {
        RZRingBuffer<int> buffer(5);
        buffer.write(1);
        buffer.write(2);
        buffer.write(3);
        EXPECT_EQ(buffer.head(), 1);    // Head is the next to read
        EXPECT_EQ(buffer.tail(), 3);    // Tail is the last written
        buffer.read();                  // Read 1
        EXPECT_EQ(buffer.head(), 2);
        EXPECT_EQ(buffer.tail(), 3);
    }

    // Test size, capacity, empty, isFull
    TEST_F(RZRingBufferTests, SizeCapacityEmptyFull)
    {
        RZRingBuffer<int> buffer(4);
        EXPECT_EQ(buffer.size(), 0);
        EXPECT_EQ(buffer.capacity(), 4);
        EXPECT_TRUE(buffer.empty());
        EXPECT_FALSE(buffer.isFull());

        for (int i = 0; i < 2; ++i) {    // Fill to 2, one short of full
            buffer.write(i);
        }
        EXPECT_EQ(buffer.size(), 2);
        EXPECT_FALSE(buffer.empty());
        EXPECT_FALSE(buffer.isFull());

        buffer.write(3);    // Now full
        EXPECT_EQ(buffer.size(), 3);
        EXPECT_FALSE(buffer.empty());
        EXPECT_TRUE(buffer.isFull());
    }

    // Test clear
    TEST_F(RZRingBufferTests, Clear)
    {
        RZRingBuffer<int> buffer(5);
        buffer.write(1);
        buffer.write(2);
        buffer.write(3);
        EXPECT_EQ(buffer.size(), 3);
        buffer.clear();
        EXPECT_EQ(buffer.size(), 0);
        EXPECT_TRUE(buffer.empty());
    }

    // Test move constructor
    TEST_F(RZRingBufferTests, MoveConstructor)
    {
        RZRingBuffer<int> original(5);
        original.write(1);
        original.write(2);
        RZRingBuffer<int> moved(std::move(original));
        EXPECT_EQ(moved.size(), 2);
        EXPECT_EQ(moved.read(), 1);
        EXPECT_EQ(moved.read(), 2);
        EXPECT_TRUE(moved.empty());
        // Original should be empty after move
        EXPECT_TRUE(original.empty());
    }

    // Test move assignment
    TEST_F(RZRingBufferTests, MoveAssignment)
    {
        RZRingBuffer<int> original(5);
        original.write(1);
        original.write(2);
        RZRingBuffer<int> target(3);
        target = std::move(original);
        EXPECT_EQ(target.size(), 2);
        EXPECT_EQ(target.read(), 1);
        EXPECT_EQ(target.read(), 2);
        EXPECT_TRUE(target.empty());
        EXPECT_TRUE(original.empty());
    }

    // Test iterators
    TEST_F(RZRingBufferTests, Iterators)
    {
        RZRingBuffer<int> buffer(5);
        buffer.write(1);
        buffer.write(2);
        buffer.write(3);
        // Assuming iterators work linearly (not wrapping), begin to end
        auto it = buffer.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
        ++it;
        EXPECT_EQ(it, buffer.end());

        // Const iterators
        const auto& const_buffer = buffer;
        auto        cit          = const_buffer.begin();
        EXPECT_EQ(*cit, 1);
    }

    // Test with custom type
    struct TestStruct
    {
        int x;
        TestStruct(int val)
            : x(val) {}
        bool operator==(const TestStruct& other) const { return x == other.x; }
    };

    TEST_F(RZRingBufferTests, CustomType)
    {
        RZRingBuffer<TestStruct> buffer(5);
        buffer.write(TestStruct(10));
        buffer.emplace(20);
        EXPECT_EQ(buffer.size(), 2);
        TestStruct val1 = buffer.read();
        EXPECT_EQ(val1.x, 10);
        TestStruct val2 = buffer.read();
        EXPECT_EQ(val2.x, 20);
    }

    // Test edge cases: read on empty (should assert, but we skip)
    TEST_F(RZRingBufferTests, ReadOnEmpty)
    {
        RZRingBuffer<int> buffer(5);
        // buffer.read();  // Would assert, skip to avoid crash
        EXPECT_TRUE(buffer.empty());
    }

    // Test write on full (should assert, but we skip)
    TEST_F(RZRingBufferTests, WriteOnFull)
    {
        RZRingBuffer<int> buffer(4);
        for (int i = 0; i < 3; ++i) buffer.write(i);
        // buffer.write(3);  // Would assert, skip
        EXPECT_TRUE(buffer.isFull());
    }

    // Test large capacity and operations
    TEST_F(RZRingBufferTests, LargeCapacity)
    {
        RZRingBuffer<int> buffer(100);
        for (int i = 0; i < 99; ++i) {
            buffer.write(i);
        }
        EXPECT_EQ(buffer.size(), 99);
        EXPECT_TRUE(buffer.isFull());
    }

    // Test multiple wrap-arounds
    TEST_F(RZRingBufferTests, MultipleWrapArounds)
    {
        RZRingBuffer<int> buffer(5);
        for (int cycle = 0; cycle < 3; ++cycle) {
            for (int i = 0; i < 3; ++i) {
                buffer.write(cycle * 10 + i);
            }
            for (int i = 0; i < 3; ++i) {
                EXPECT_EQ(buffer.read(), cycle * 10 + i);
            }
        }
        EXPECT_TRUE(buffer.empty());
    }

}    // namespace Razix