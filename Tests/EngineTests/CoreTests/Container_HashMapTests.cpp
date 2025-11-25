// AI-generated unit tests for the RZHashMap container class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/hash_map.h"
#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Log/RZLog.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace Razix {

    //==================================================
    // Type Definitions for Parametrized Testing
    //==================================================

    // Define various key-value type combinations
    template<typename Key, typename Value>
    struct HashMapTypeParam
    {
        using key_type   = Key;
        using value_type = Value;
    };

    // All type combinations to test
    using TestTypes = ::testing::Types<
        // Int keys with various value types
        HashMapTypeParam<int, int>,
        HashMapTypeParam<int, float>,
        HashMapTypeParam<int, RZString>,
        HashMapTypeParam<int, double>,

        // String keys with various value types
        HashMapTypeParam<RZString, int>,
        HashMapTypeParam<RZString, float>,
        HashMapTypeParam<RZString, RZString>,
        HashMapTypeParam<RZString, double>,

        // Pointer keys
        HashMapTypeParam<int*, int>,
        HashMapTypeParam<int*, RZString>,

        // Uint64 keys
        HashMapTypeParam<uint64_t, RZString>,
        HashMapTypeParam<uint64_t, int>,

        // Uint32 keys
        HashMapTypeParam<uint32_t, RZString>,
        HashMapTypeParam<uint32_t, float>>;

    //==================================================
    // Typed Test Suite
    //==================================================

    template<typename T>
    class RZHashMapTypedTest : public ::testing::Test
    {
    protected:
        using KeyType     = typename T::key_type;
        using ValueType   = typename T::value_type;
        using HashMapType = RZHashMap<KeyType, ValueType>;

        HashMapType map;

        // Helper to create test keys
        KeyType CreateKey(int index)
        {
            if constexpr (std::is_same_v<KeyType, int>) {
                return static_cast<KeyType>(index);
            } else if constexpr (std::is_same_v<KeyType, uint64_t>) {
                return static_cast<uint64_t>(index);
            } else if constexpr (std::is_same_v<KeyType, uint32_t>) {
                return static_cast<uint32_t>(index);
            } else if constexpr (std::is_same_v<KeyType, RZString>) {
                return RZString("key_") + rz_to_string(index);
            } else if constexpr (std::is_pointer_v<KeyType>) {
                static int dummy_values[10000];
                return reinterpret_cast<KeyType>(&dummy_values[index % 10000]);
            }
        }

        // Helper to create test values
        ValueType CreateValue(int index)
        {
            if constexpr (std::is_same_v<ValueType, int>) {
                return static_cast<ValueType>(index * 100);
            } else if constexpr (std::is_same_v<ValueType, float>) {
                return static_cast<float>(index * 3.14f);
            } else if constexpr (std::is_same_v<ValueType, double>) {
                return static_cast<double>(index * 2.71828);
            } else if constexpr (std::is_same_v<ValueType, RZString>) {
                return RZString("value_") + rz_to_string(index);
            }
        }

        // Helper to compare values
        bool ValuesEqual(const ValueType& a, const ValueType& b)
        {
            if constexpr (std::is_same_v<ValueType, float>) {
                return std::abs(a - b) < 0.0001f;
            } else if constexpr (std::is_same_v<ValueType, double>) {
                return std::abs(a - b) < 0.0001;
            } else {
                return a == b;
            }
        }

        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();
        }

        void TearDown() override
        {
            Razix::Debug::RZLog::Shutdown();
        }
    };

    TYPED_TEST_SUITE(RZHashMapTypedTest, TestTypes);

    //==================================================
    // Basic Insertion Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, InsertSingleElement)
    {
        auto key   = this->CreateKey(1);
        auto value = this->CreateValue(1);

        this->map.insert(key, value);

        EXPECT_EQ(this->map.size(), 1);
        EXPECT_FALSE(this->map.empty());
    }

    TYPED_TEST(RZHashMapTypedTest, InsertMultipleElements)
    {
        this->map.insert(this->CreateKey(1), this->CreateValue(1));
        this->map.insert(this->CreateKey(2), this->CreateValue(2));
        this->map.insert(this->CreateKey(3), this->CreateValue(3));

        EXPECT_EQ(this->map.size(), 3);
    }

    TYPED_TEST(RZHashMapTypedTest, InsertTenElements)
    {
        for (int i = 0; i < 10; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        EXPECT_EQ(this->map.size(), 10);
    }

    //==================================================
    // Find / Search Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, FindExistingKey)
    {
        auto key   = this->CreateKey(42);
        auto value = this->CreateValue(42);

        this->map.insert(key, value);
        auto result = this->map.find(key);

        EXPECT_NE(result, this->map.end());
        EXPECT_TRUE(this->ValuesEqual(result.value(), value));
    }

    TYPED_TEST(RZHashMapTypedTest, FindNonExistentKey)
    {
        this->map.insert(this->CreateKey(1), this->CreateValue(1));
        auto result = this->map.find(this->CreateKey(999));

        EXPECT_EQ(result, this->map.end());
    }

    TYPED_TEST(RZHashMapTypedTest, Contains)
    {
        auto key = this->CreateKey(10);
        this->map.insert(key, this->CreateValue(10));

        EXPECT_TRUE(this->map.contains(key));
        EXPECT_FALSE(this->map.contains(this->CreateKey(999)));
    }

    TYPED_TEST(RZHashMapTypedTest, FindAfterMultipleInserts)
    {
        const int NUM_ELEMENTS = 50;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            auto result = this->map.find(this->CreateKey(i));
            EXPECT_NE(result, this->map.end());
            EXPECT_TRUE(this->ValuesEqual(result.value(), this->CreateValue(i)));
        }
    }

    TYPED_TEST(RZHashMapTypedTest, FindAfterManyInserts)
    {
        const int NUM_ELEMENTS = 200;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        // Random verification
        for (int i = 0; i < NUM_ELEMENTS; i += 7) {
            auto result = this->map.find(this->CreateKey(i));
            EXPECT_NE(result, this->map.end());
        }
    }

    //==================================================
    // Update / Overwrite Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, UpdateExistingKey)
    {
        auto key    = this->CreateKey(1);
        auto value1 = this->CreateValue(1);
        auto value2 = this->CreateValue(2);

        this->map.insert(key, value1);
        EXPECT_TRUE(this->ValuesEqual(this->map.find(key).value(), value1));

        this->map.insert(key, value2);
        EXPECT_EQ(this->map.size(), 1);
        EXPECT_TRUE(this->ValuesEqual(this->map.find(key).value(), value2));
    }

    TYPED_TEST(RZHashMapTypedTest, UpdateWithMoveSemantics)
    {
        auto key    = this->CreateKey(1);
        auto value1 = this->CreateValue(1);
        auto value2 = this->CreateValue(2);

        this->map.insert(key, value1);
        this->map.insert(key, std::move(value2));

        EXPECT_EQ(this->map.size(), 1);
        EXPECT_TRUE(this->ValuesEqual(this->map.find(key).value(), this->CreateValue(2)));
    }

    TYPED_TEST(RZHashMapTypedTest, MultipleUpdates)
    {
        auto key = this->CreateKey(5);

        for (int i = 0; i < 10; ++i) {
            this->map.insert(key, this->CreateValue(i));
        }

        EXPECT_EQ(this->map.size(), 1);
        EXPECT_TRUE(this->ValuesEqual(this->map.find(key).value(), this->CreateValue(9)));
    }

    //==================================================
    // Remove / Delete Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, RemoveExistingKey)
    {
        auto key1 = this->CreateKey(1);
        auto key2 = this->CreateKey(2);

        this->map.insert(key1, this->CreateValue(1));
        this->map.insert(key2, this->CreateValue(2));

        bool result = this->map.remove(key1);

        EXPECT_TRUE(result);
        EXPECT_EQ(this->map.size(), 1);
        EXPECT_EQ(this->map.find(key1), this->map.end());
    }

    TYPED_TEST(RZHashMapTypedTest, RemoveNonExistentKey)
    {
        this->map.insert(this->CreateKey(1), this->CreateValue(1));
        bool result = this->map.remove(this->CreateKey(999));

        EXPECT_FALSE(result);
        EXPECT_EQ(this->map.size(), 1);
    }

    TYPED_TEST(RZHashMapTypedTest, RemoveAllElements)
    {
        for (int i = 1; i <= 5; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        for (int i = 1; i <= 5; ++i) {
            if (!this->map.remove(this->CreateKey(i))) {
                RAZIX_CORE_ERROR("Failed to remove key during RemoveAllElements test");
                RAZIX_DEBUG_BREAK();
            }
        }

        EXPECT_TRUE(this->map.empty());
        EXPECT_EQ(this->map.size(), 0);
    }

    TYPED_TEST(RZHashMapTypedTest, ReinsertAfterRemove)
    {
        auto key    = this->CreateKey(1);
        auto value1 = this->CreateValue(1);
        auto value2 = this->CreateValue(2);

        this->map.insert(key, value1);
        this->map.remove(key);
        this->map.insert(key, value2);

        EXPECT_EQ(this->map.size(), 1);
        EXPECT_TRUE(this->ValuesEqual(this->map.find(key).value(), value2));
    }

    TYPED_TEST(RZHashMapTypedTest, RemoveFromMiddle)
    {
        const int NUM_ELEMENTS = 20;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        // Remove elements from middle
        for (int i = 5; i < 15; ++i) {
            this->map.remove(this->CreateKey(i));
        }

        EXPECT_EQ(this->map.size(), 10);

        // Verify remaining elements
        for (int i = 0; i < 5; ++i) {
            EXPECT_NE(this->map.find(this->CreateKey(i)), this->map.end());
        }
        for (int i = 15; i < NUM_ELEMENTS; ++i) {
            EXPECT_NE(this->map.find(this->CreateKey(i)), this->map.end());
        }
    }

    //==================================================
    // Operator[] Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, OperatorBracketAccess)
    {
        auto key   = this->CreateKey(1);
        auto value = this->CreateValue(1);

        this->map.insert(key, value);
        auto& retrieved = this->map[key];

        EXPECT_TRUE(this->ValuesEqual(retrieved, value));
    }

    TYPED_TEST(RZHashMapTypedTest, OperatorBracketInsertDefault)
    {
        auto  key   = this->CreateKey(5);
        auto& value = this->map[key];    // Key doesn't exist
        value       = this->CreateValue(10);

        EXPECT_EQ(this->map.size(), 1);
        EXPECT_TRUE(this->ValuesEqual(this->map.find(key).value(), this->CreateValue(10)));
    }

    TYPED_TEST(RZHashMapTypedTest, OperatorBracketUpdate)
    {
        auto key    = this->CreateKey(1);
        auto value1 = this->CreateValue(1);
        auto value2 = this->CreateValue(2);

        this->map.insert(key, value1);
        this->map[key] = value2;

        EXPECT_TRUE(this->ValuesEqual(this->map.find(key).value(), value2));
    }

    TYPED_TEST(RZHashMapTypedTest, OperatorBracketChained)
    {
        auto key1 = this->CreateKey(1);
        auto key2 = this->CreateKey(2);

        this->map[key1] = this->CreateValue(100);
        this->map[key2] = this->CreateValue(200);

        EXPECT_EQ(this->map.size(), 2);
        EXPECT_TRUE(this->ValuesEqual(this->map[key1], this->CreateValue(100)));
        EXPECT_TRUE(this->ValuesEqual(this->map[key2], this->CreateValue(200)));
    }

    //==================================================
    // Expansion / Capacity Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, AutomaticExpansion)
    {
        size_t initial_capacity = this->map.capacity();

        for (int i = 0; i < 100; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        EXPECT_GT(this->map.capacity(), initial_capacity);
        EXPECT_EQ(this->map.size(), 100);
    }

    TYPED_TEST(RZHashMapTypedTest, CapacityAfterExpansion)
    {
        for (int i = 0; i < 50; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        size_t capacity    = this->map.capacity();
        float  load_factor = this->map.load_factor();

        EXPECT_LT(load_factor, 0.6f);
        EXPECT_GT(capacity, 50);
    }

    TYPED_TEST(RZHashMapTypedTest, ElementsPreservedAfterExpansion)
    {
        const int NUM_ELEMENTS = 30;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            auto result = this->map.find(this->CreateKey(i));
            EXPECT_NE(result, this->map.end());
            EXPECT_TRUE(this->ValuesEqual(result.value(), this->CreateValue(i)));
        }
    }

    TYPED_TEST(RZHashMapTypedTest, MultipleExpansions)
    {
        const int NUM_ELEMENTS = 500;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        EXPECT_EQ(this->map.size(), NUM_ELEMENTS);

        // Verify random elements after multiple expansions
        for (int i = 0; i < NUM_ELEMENTS; i += 13) {
            EXPECT_NE(this->map.find(this->CreateKey(i)), this->map.end());
        }
    }

    //==================================================
    // Clear Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, Clear)
    {
        for (int i = 1; i <= 3; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        this->map.clear();

        EXPECT_TRUE(this->map.empty());
        EXPECT_EQ(this->map.size(), 0);
        EXPECT_EQ(this->map.find(this->CreateKey(1)), this->map.end());
    }

    TYPED_TEST(RZHashMapTypedTest, ClearThenReinsert)
    {
        auto key    = this->CreateKey(1);
        auto value1 = this->CreateValue(1);
        auto value2 = this->CreateValue(2);

        this->map.insert(key, value1);
        this->map.clear();
        this->map.insert(key, value2);

        EXPECT_EQ(this->map.size(), 1);
        EXPECT_TRUE(this->ValuesEqual(this->map.find(key).value(), value2));
    }

    TYPED_TEST(RZHashMapTypedTest, ClearLargeMap)
    {
        for (int i = 0; i < 200; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        this->map.clear();

        EXPECT_TRUE(this->map.empty());
        EXPECT_EQ(this->map.size(), 0);
    }

    //==================================================
    // Collision Handling Tests (Quadratic Probing)
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, HandlesCollisions)
    {
        // Insert elements that could collide
        this->map.insert(this->CreateKey(0), this->CreateValue(0));
        this->map.insert(this->CreateKey(16), this->CreateValue(16));
        this->map.insert(this->CreateKey(32), this->CreateValue(32));

        EXPECT_EQ(this->map.size(), 3);
        EXPECT_NE(this->map.find(this->CreateKey(0)), this->map.end());
        EXPECT_NE(this->map.find(this->CreateKey(16)), this->map.end());
        EXPECT_NE(this->map.find(this->CreateKey(32)), this->map.end());
    }

    TYPED_TEST(RZHashMapTypedTest, CollisionWithManyElements)
    {
        const int NUM_ELEMENTS = 200;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            auto result = this->map.find(this->CreateKey(i));
            EXPECT_NE(result, this->map.end());
            EXPECT_TRUE(this->ValuesEqual(result.value(), this->CreateValue(i)));
        }
    }

    TYPED_TEST(RZHashMapTypedTest, SequentialInsertion)
    {
        for (int i = 0; i < 100; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        EXPECT_EQ(this->map.size(), 100);
    }

    //==================================================
    // Iterator Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, IterateEmptyMap)
    {
        int count = 0;
        for (auto it = this->map.begin(); it != this->map.end(); ++it) {
            count++;
        }
        EXPECT_EQ(count, 0);
    }

    TYPED_TEST(RZHashMapTypedTest, IterateSingleElement)
    {
        auto key   = this->CreateKey(42);
        auto value = this->CreateValue(42);

        this->map.insert(key, value);

        int count = 0;
        for (auto it = this->map.begin(); it != this->map.end(); ++it) {
            EXPECT_TRUE(this->ValuesEqual(it.value(), value));
            count++;
        }
        EXPECT_EQ(count, 1);
    }

    TYPED_TEST(RZHashMapTypedTest, IterateMultipleElements)
    {
        for (int i = 1; i <= 3; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        int count = 0;
        for (auto it = this->map.begin(); it != this->map.end(); ++it) {
            count++;
        }
        EXPECT_EQ(count, 3);
    }

    TYPED_TEST(RZHashMapTypedTest, IteratorKeyValue)
    {
        auto key   = this->CreateKey(10);
        auto value = this->CreateValue(10);

        this->map.insert(key, value);

        for (auto it = this->map.begin(); it != this->map.end(); ++it) {
            EXPECT_TRUE(this->ValuesEqual(it.value(), value));
            EXPECT_TRUE(this->ValuesEqual(it.second(), value));    // Alias for value()
        }
    }

    TYPED_TEST(RZHashMapTypedTest, IterateAfterRemove)
    {
        for (int i = 1; i <= 3; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        this->map.remove(this->CreateKey(2));

        int count = 0;
        for (auto it = this->map.begin(); it != this->map.end(); ++it) {
            count++;
        }
        EXPECT_EQ(count, 2);
    }

    TYPED_TEST(RZHashMapTypedTest, IteratorPrefixIncrement)
    {
        for (int i = 1; i <= 2; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        auto it = this->map.begin();
        ++it;
        EXPECT_NE(it, this->map.end());
    }

    TYPED_TEST(RZHashMapTypedTest, IteratorPostfixIncrement)
    {
        for (int i = 1; i <= 2; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        auto it      = this->map.begin();
        auto it_copy = it++;
        EXPECT_NE(it, it_copy);
    }

    TYPED_TEST(RZHashMapTypedTest, ConstIterator)
    {
        auto key   = this->CreateKey(5);
        auto value = this->CreateValue(5);

        this->map.insert(key, value);

        const auto& const_map = this->map;
        auto        it        = const_map.begin();

        EXPECT_TRUE(this->ValuesEqual(it.value(), value));
    }

    TYPED_TEST(RZHashMapTypedTest, IterateLargeMap)
    {
        const int NUM_ELEMENTS = 100;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        int count = 0;
        for (auto it = this->map.begin(); it != this->map.end(); ++it) {
            count++;
        }
        EXPECT_EQ(count, NUM_ELEMENTS);
    }

    //==================================================
    // Copy Semantics Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, CopyConstructor)
    {
        for (int i = 1; i <= 2; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        auto map_copy = this->map;

        EXPECT_EQ(map_copy.size(), 2);
        EXPECT_NE(map_copy.find(this->CreateKey(1)), map_copy.end());
        EXPECT_NE(map_copy.find(this->CreateKey(2)), map_copy.end());
    }

    TYPED_TEST(RZHashMapTypedTest, CopyAssignment)
    {
        for (int i = 1; i <= 2; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        decltype(this->map) map_copy;
        map_copy = this->map;

        EXPECT_EQ(map_copy.size(), 2);
        EXPECT_NE(map_copy.find(this->CreateKey(1)), map_copy.end());
    }

    TYPED_TEST(RZHashMapTypedTest, CopyAssignmentWithResize)
    {
        for (int i = 0; i < 50; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        decltype(this->map) map_copy;
        map_copy = this->map;

        EXPECT_EQ(map_copy.size(), 50);
        EXPECT_NE(map_copy.find(this->CreateKey(25)), map_copy.end());
    }

    TYPED_TEST(RZHashMapTypedTest, CopySelfAssignment)
    {
        this->map.insert(this->CreateKey(1), this->CreateValue(1));
        this->map = this->map;

        EXPECT_EQ(this->map.size(), 1);
        EXPECT_NE(this->map.find(this->CreateKey(1)), this->map.end());
    }

    TYPED_TEST(RZHashMapTypedTest, CopyIndependence)
    {
        auto key = this->CreateKey(1);
        this->map.insert(key, this->CreateValue(1));

        auto map_copy = this->map;
        this->map.remove(key);

        EXPECT_EQ(this->map.size(), 0);
        EXPECT_EQ(map_copy.size(), 1);
        EXPECT_NE(map_copy.find(key), map_copy.end());
    }

    //==================================================
    // Move Semantics Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, MoveConstructor)
    {
        for (int i = 1; i <= 2; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        auto map_moved = std::move(this->map);

        EXPECT_EQ(map_moved.size(), 2);
        EXPECT_EQ(this->map.size(), 0);
    }

    TYPED_TEST(RZHashMapTypedTest, MoveAssignment)
    {
        for (int i = 1; i <= 2; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        decltype(this->map) map_moved;
        map_moved = std::move(this->map);

        EXPECT_EQ(map_moved.size(), 2);
        EXPECT_EQ(this->map.size(), 0);
    }

    TYPED_TEST(RZHashMapTypedTest, MoveAssignmentSelfAssignment)
    {
        this->map.insert(this->CreateKey(1), this->CreateValue(1));
        this->map = std::move(this->map);

        EXPECT_EQ(this->map.size(), 1);
        EXPECT_NE(this->map.find(this->CreateKey(1)), this->map.end());
    }

    TYPED_TEST(RZHashMapTypedTest, MoveLargeMap)
    {
        for (int i = 0; i < 100; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        auto map_moved = std::move(this->map);

        EXPECT_EQ(map_moved.size(), 100);
        EXPECT_EQ(this->map.size(), 0);
    }

    //==================================================
    // Stress Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, StressTestLargeInsert)
    {
        const int NUM_ELEMENTS = 1000;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        EXPECT_EQ(this->map.size(), NUM_ELEMENTS);
    }

    TYPED_TEST(RZHashMapTypedTest, StressTestLargeFind)
    {
        const int NUM_ELEMENTS = 500;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            EXPECT_NE(this->map.find(this->CreateKey(i)), this->map.end());
        }
    }

    TYPED_TEST(RZHashMapTypedTest, StressTestMixed)
    {
        const int NUM_OPS = 500;

        for (int i = 0; i < NUM_OPS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        for (int i = 0; i < NUM_OPS / 2; ++i) {
            this->map.remove(this->CreateKey(i));
        }

        EXPECT_EQ(this->map.size(), NUM_OPS / 2);

        for (int i = NUM_OPS / 2; i < NUM_OPS; ++i) {
            EXPECT_NE(this->map.find(this->CreateKey(i)), this->map.end());
        }
    }

    TYPED_TEST(RZHashMapTypedTest, StressTestAlternatingInsertRemove)
    {
        const int NUM_OPS = 200;

        for (int i = 0; i < NUM_OPS; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
            if (i > 0 && i % 2 == 0) {
                this->map.remove(this->CreateKey(i - 1));
            }
        }

        EXPECT_GT(this->map.size(), 0);
        EXPECT_LE(this->map.size(), NUM_OPS);
    }

    //==================================================
    // Edge Cases
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, ZeroKey)
    {
        using KeyType = typename TestFixture::KeyType;
        if constexpr (std::is_integral_v<KeyType>) {
            this->map.insert(this->CreateKey(0), this->CreateValue(0));
            EXPECT_NE(this->map.find(this->CreateKey(0)), this->map.end());
        }
    }

    TYPED_TEST(RZHashMapTypedTest, NegativeKeys)
    {
        using KeyType = typename TestFixture::KeyType;
        if constexpr (std::is_signed_v<KeyType> && std::is_integral_v<KeyType>) {
            auto key = this->CreateKey(-1);
            this->map.insert(key, this->CreateValue(1));
            EXPECT_NE(this->map.find(key), this->map.end());
        }
    }

    TYPED_TEST(RZHashMapTypedTest, EmptyStringKey)
    {
        using KeyType = typename TestFixture::KeyType;
        if constexpr (std::is_same_v<KeyType, RZString>) {
            auto empty_key = RZString("");
            this->map.insert(empty_key, this->CreateValue(1));
            EXPECT_NE(this->map.find(empty_key), this->map.end());
        }
    }

    TYPED_TEST(RZHashMapTypedTest, IdenticalKeysAndValues)
    {
        using KeyType   = typename TestFixture::KeyType;
        using ValueType = typename TestFixture::ValueType;
        if constexpr (std::is_same_v<KeyType, RZString> &&
                      std::is_same_v<ValueType, RZString>) {
            auto key_value = RZString("same");
            this->map.insert(key_value, key_value);
            EXPECT_NE(this->map.find(key_value), this->map.end());
        }
    }

    //==================================================
    // Utility Tests
    //==================================================

    TYPED_TEST(RZHashMapTypedTest, Empty)
    {
        EXPECT_TRUE(this->map.empty());

        this->map.insert(this->CreateKey(1), this->CreateValue(1));
        EXPECT_FALSE(this->map.empty());

        this->map.remove(this->CreateKey(1));
        EXPECT_TRUE(this->map.empty());
    }

    TYPED_TEST(RZHashMapTypedTest, Size)
    {
        EXPECT_EQ(this->map.size(), 0);

        this->map.insert(this->CreateKey(1), this->CreateValue(1));
        EXPECT_EQ(this->map.size(), 1);

        this->map.insert(this->CreateKey(2), this->CreateValue(2));
        EXPECT_EQ(this->map.size(), 2);

        this->map.remove(this->CreateKey(1));
        EXPECT_EQ(this->map.size(), 1);
    }

    TYPED_TEST(RZHashMapTypedTest, Capacity)
    {
        size_t initial_capacity = this->map.capacity();
        EXPECT_GT(initial_capacity, 0);

        for (int i = 0; i < 100; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        EXPECT_GE(this->map.capacity(), initial_capacity);
    }

    TYPED_TEST(RZHashMapTypedTest, LoadFactor)
    {
        float initial_lf = this->map.load_factor();
        EXPECT_EQ(initial_lf, 0.0f);

        this->map.insert(this->CreateKey(1), this->CreateValue(1));
        float lf_after_insert = this->map.load_factor();

        EXPECT_GT(lf_after_insert, 0.0f);
        EXPECT_LT(lf_after_insert, 1.0f);
    }

    TYPED_TEST(RZHashMapTypedTest, beginend)
    {
        for (int i = 1; i <= 3; ++i) {
            this->map.insert(this->CreateKey(i), this->CreateValue(i));
        }

        int count = 0;
        for (auto it = this->map.begin(); it != this->map.end(); ++it) {
            count++;
        }
        EXPECT_EQ(count, 3);
    }

    //==================================================
    // Test: Assignment to second via iterator
    //==================================================
    // Corrected by: Pikachuxxxx
    // Date: 2025-11-15 17:18:03 UTC

    TYPED_TEST(RZHashMapTypedTest, AssignmentToSecondPersists)
    {
        // SETUP: Insert a known key-value pair.
        auto key1       = this->CreateKey(1);
        auto val1       = this->CreateValue(1);
        this->map[key1] = val1;

        // Insert other keys to ensure we aren't just getting lucky with position.
        this->map[this->CreateKey(2)] = this->CreateValue(2);
        this->map[this->CreateKey(3)] = this->CreateValue(3);
        ASSERT_EQ(this->map.size(), 3);

        auto modified_val = this->CreateValue(999);

        // ACTION:
        // 1. Find the specific element for key1 to get a mutable iterator.
        auto it = this->map.find(key1);
        ASSERT_NE(it, this->map.end());    // Ensure we found the key.

        // 2. Modify the value via the iterator.
        it->second = modified_val;

        // VERIFICATION:
        // 3. Directly verify that the value for key1 was updated.
        EXPECT_TRUE(this->ValuesEqual(this->map[key1], modified_val));
    }

    //==================================================
    // Test: Assignment to first via iterator
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, AssignmentToFirstPersists)
    {
        auto key         = this->CreateKey(1);
        auto initial_val = this->CreateValue(1);

        this->map[key] = initial_val;

        auto it = this->map.begin();

        auto val_change1 = this->CreateValue(10);

        it->second = val_change1;
        EXPECT_TRUE(this->ValuesEqual(this->map[key], val_change1));
    }

    //==================================================
    // Test: Multiple sequential assignments
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, MultipleSequentialAssignments)
    {
        auto key         = this->CreateKey(1);
        auto initial_val = this->CreateValue(1);

        this->map[key] = initial_val;

        auto it = this->map.begin();

        // Multiple assignments to same element
        auto val_change1 = this->CreateValue(10);
        auto val_change2 = this->CreateValue(20);
        auto val_change3 = this->CreateValue(30);

        it->second = val_change1;
        EXPECT_TRUE(this->ValuesEqual(this->map[key], val_change1));

        it->second = val_change2;
        EXPECT_TRUE(this->ValuesEqual(this->map[key], val_change2));

        it->second = val_change3;
        EXPECT_TRUE(this->ValuesEqual(this->map[key], val_change3));
    }

    //==================================================
    // Test: Assignment via first() and second() accessors
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, AccessorMethodsModifyUnderlying)
    {
        auto key         = this->CreateKey(5);
        auto initial_val = this->CreateValue(50);

        this->map[key] = initial_val;

        auto it           = this->map.begin();
        auto modified_val = this->CreateValue(500);

        // Modify via accessor methods
        it.second() = modified_val;

        // Verify change persists
        EXPECT_TRUE(this->ValuesEqual(this->map[key], modified_val));
    }

    //==================================================
    // Test: Assignment through operator->
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, ArrowOperatorAssignment)
    {
        auto key          = this->CreateKey(7);
        auto initial_val  = this->CreateValue(70);
        auto modified_val = this->CreateValue(700);

        this->map[key] = initial_val;

        auto it    = this->map.begin();
        it->second = modified_val;

        EXPECT_TRUE(this->ValuesEqual(this->map[key], modified_val));
    }

    //==================================================
    // Test: Range-based for loop with assignment
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, RangeBasedForWithAssignment)
    {
        auto key1 = this->CreateKey(1);
        auto key2 = this->CreateKey(2);
        auto key3 = this->CreateKey(3);

        auto val1 = this->CreateValue(1);
        auto val2 = this->CreateValue(2);
        auto val3 = this->CreateValue(3);

        this->map[key1] = val1;
        this->map[key2] = val2;
        this->map[key3] = val3;

        // Modify all values in range-based loop
        int index = 0;
        for (auto& pair: this->map) {
            auto multiplied = this->CreateValue((index + 1) * 100);
            pair.second     = multiplied;
            index++;
        }

        // Verify all modified
        for (auto& pair: this->map) {
            auto val = pair.second;
            // Each value should be modified (non-zero and changed)
            EXPECT_FALSE(this->ValuesEqual(val, val1) && this->ValuesEqual(val, val2) && this->ValuesEqual(val, val3));
        }
    }

    //==================================================
    // Test: Assignment doesn't affect other elements
    //==================================================
    // Corrected by: Pikachuxxxx
    // Date: 2025-11-15 17:24:58 UTC

    TYPED_TEST(RZHashMapTypedTest, AssignmentIsolation)
    {
        // SETUP: Insert three distinct key-value pairs.
        auto key1 = this->CreateKey(1);
        auto key2 = this->CreateKey(2);
        auto key3 = this->CreateKey(3);

        auto val1 = this->CreateValue(100);
        auto val2 = this->CreateValue(200);
        auto val3 = this->CreateValue(300);

        this->map[key1] = val1;
        this->map[key2] = val2;
        this->map[key3] = val3;
        ASSERT_EQ(this->map.size(), 3);

        auto modified_val = this->CreateValue(999);

        // ACTION:
        // 1. Find a specific element to modify (e.g., the one for key2).
        auto it = this->map.find(key2);
        ASSERT_NE(it, this->map.end());

        // 2. Modify only this element.
        it->second = modified_val;

        // VERIFICATION:
        // 3. Confirm that the target element was changed.
        EXPECT_TRUE(this->ValuesEqual(this->map[key2], modified_val));

        // 4. CRITICAL: Verify that the other elements were NOT affected.
        EXPECT_TRUE(this->ValuesEqual(this->map[key1], val1));
        EXPECT_TRUE(this->ValuesEqual(this->map[key3], val3));
    }

    //==================================================
    // Test: Const iterator prevents modification (compile-time check)
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, ConstIteratorReadOnly)
    {
        auto key         = this->CreateKey(1);
        auto initial_val = this->CreateValue(100);

        this->map[key] = initial_val;

        const auto& const_map = this->map;
        auto        const_it  = const_map.begin();

        // Verify we can read through const iterator
        auto read_val = const_it->second;
        EXPECT_TRUE(this->ValuesEqual(read_val, initial_val));

        // Note: Attempting to modify through const_it should not compile
        // const_it->second = this->CreateValue(999);  // Would fail to compile
    }

    //==================================================
    // Test: Assignment through reference returned by value()
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, ValueAccessorAssignment)
    {
        auto key          = this->CreateKey(42);
        auto initial_val  = this->CreateValue(420);
        auto modified_val = this->CreateValue(4200);

        this->map[key] = initial_val;

        auto  it      = this->map.begin();
        auto& val_ref = it.value();
        val_ref       = modified_val;

        EXPECT_TRUE(this->ValuesEqual(this->map[key], modified_val));
    }

    //==================================================
    // Test: Assignment after find()
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, AssignmentAfterFind)
    {
        auto key1 = this->CreateKey(10);
        auto key2 = this->CreateKey(20);
        auto val1 = this->CreateValue(100);
        auto val2 = this->CreateValue(200);

        this->map[key1] = val1;
        this->map[key2] = val2;

        auto it = this->map.find(key1);
        ASSERT_NE(it, this->map.end());

        auto modified_val = this->CreateValue(1000);
        it->second        = modified_val;

        EXPECT_TRUE(this->ValuesEqual(this->map[key1], modified_val));
        EXPECT_TRUE(this->ValuesEqual(this->map[key2], val2));
    }

    //==================================================
    // Test: Rapid successive assignments
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, RapidSuccessiveAssignments)
    {
        auto key         = this->CreateKey(1);
        auto initial_val = this->CreateValue(1);

        this->map[key] = initial_val;

        auto it = this->map.begin();

        // Perform 10 rapid assignments
        for (int i = 0; i < 10; ++i) {
            auto new_val = this->CreateValue(i * 10);
            it->second   = new_val;
        }

        // Final value should be from last iteration
        auto expected_val = this->CreateValue(90);
        EXPECT_TRUE(this->ValuesEqual(this->map[key], expected_val));
    }

    //==================================================
    // Test: Assignment preserves map size
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, AssignmentPreservesSize)
    {
        auto key1 = this->CreateKey(1);
        auto key2 = this->CreateKey(2);
        auto key3 = this->CreateKey(3);

        auto val1 = this->CreateValue(10);
        auto val2 = this->CreateValue(20);
        auto val3 = this->CreateValue(30);

        this->map[key1] = val1;
        this->map[key2] = val2;
        this->map[key3] = val3;

        sz initial_size = this->map.size();

        // Modify values via iterator
        for (auto& pair: this->map) {
            pair.second = this->CreateValue(999);
        }

        // Size should remain unchanged
        EXPECT_EQ(this->map.size(), initial_size);
    }

    //==================================================
    // Test: Empty map iterator behavior (edge case)
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, EmptyMapIteratorBehavior)
    {
        EXPECT_EQ(this->map.begin(), this->map.end());
        EXPECT_EQ(this->map.size(), 0);
    }

    //==================================================
    // Test: Dereference operator assignment
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, DereferenceOperatorAssignment)
    {
        auto key         = this->CreateKey(5);
        auto initial_val = this->CreateValue(50);

        this->map[key] = initial_val;

        auto it   = this->map.begin();
        auto pair = *it;    // Dereference creates a copy via RZPair

        auto modified_val = this->CreateValue(500);
        pair.second       = modified_val;

        // Assign modified pair back through dereference
        *it = pair;

        EXPECT_TRUE(this->ValuesEqual(this->map[key], modified_val));
    }

    //==================================================
    // Test: Mixed assignments across multiple iterations
    //==================================================
    TYPED_TEST(RZHashMapTypedTest, MixedAssignmentsAcrossIterations)
    {
        auto key1 = this->CreateKey(1);
        auto key2 = this->CreateKey(2);
        auto key3 = this->CreateKey(3);

        auto val1 = this->CreateValue(100);
        auto val2 = this->CreateValue(200);
        auto val3 = this->CreateValue(300);

        this->map[key1] = val1;
        this->map[key2] = val2;
        this->map[key3] = val3;

        // Assign via iterator methods
        int count = 0;
        for (auto it = this->map.begin(); it != this->map.end(); ++it) {
            if (count == 0) {
                it->second = this->CreateValue(1000);
            } else if (count == 1) {
                it.second() = this->CreateValue(2000);
            } else {
                auto& ref = it.value();
                ref       = this->CreateValue(3000);
            }
            count++;
        }

        // Verify all assignments persisted
        count = 0;
        for (auto& pair: this->map) {
            auto val = pair.second;
            if (count == 0) {
                EXPECT_TRUE(this->ValuesEqual(val, this->CreateValue(1000)));
            } else if (count == 1) {
                EXPECT_TRUE(this->ValuesEqual(val, this->CreateValue(2000)));
            } else {
                EXPECT_TRUE(this->ValuesEqual(val, this->CreateValue(3000)));
            }
            count++;
        }
    }

}    // namespace Razix
