// AI-generated unit tests for the RZRingBuffer container class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Log/RZLog.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace Razix {
    // Test fixture for RZFixedArray tests
    template<typename T>
    class RZFixedArrayTest : public ::testing::Test
    {
    protected:
        static constexpr size_t CAPACITY = 10;

        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();
        }

        void TearDown() override
        {
            Razix::Debug::RZLog::Shutdown();
        }
    };

    // Test with different types
    using TestTypes = ::testing::Types<int, double, float, char>;
    TYPED_TEST_SUITE(RZFixedArrayTest, TestTypes);

    // ============================================================================
    // Construction and Destruction Tests
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, DefaultConstructor)
    {
        RZFixedArray<TypeParam, 10> arr;
        EXPECT_TRUE(arr.empty());
        EXPECT_EQ(arr.size(), 0);
        EXPECT_EQ(arr.capacity(), 10);
    }

    TYPED_TEST(RZFixedArrayTest, CopyConstructor)
    {
        RZFixedArray<TypeParam, 10> arr1;
        arr1.push_back(TypeParam(1));
        arr1.push_back(TypeParam(2));
        arr1.push_back(TypeParam(3));

        RZFixedArray<TypeParam, 10> arr2(arr1);

        EXPECT_EQ(arr2.size(), 3);
        EXPECT_EQ(arr2[0], TypeParam(1));
        EXPECT_EQ(arr2[1], TypeParam(2));
        EXPECT_EQ(arr2[2], TypeParam(3));
    }

    TYPED_TEST(RZFixedArrayTest, CopyAssignmentOperator)
    {
        RZFixedArray<TypeParam, 10> arr1;
        arr1.push_back(TypeParam(1));
        arr1.push_back(TypeParam(2));

        RZFixedArray<TypeParam, 10> arr2;
        arr2.push_back(TypeParam(5));

        arr2 = arr1;

        EXPECT_EQ(arr2.size(), 2);
        EXPECT_EQ(arr2[0], TypeParam(1));
        EXPECT_EQ(arr2[1], TypeParam(2));
    }

    TYPED_TEST(RZFixedArrayTest, MoveConstructor)
    {
        RZFixedArray<TypeParam, 10> arr1;
        arr1.push_back(TypeParam(1));
        arr1.push_back(TypeParam(2));

        RZFixedArray<TypeParam, 10> arr2(std::move(arr1));

        EXPECT_EQ(arr2.size(), 2);
        EXPECT_EQ(arr2[0], TypeParam(1));
        EXPECT_EQ(arr2[1], TypeParam(2));
    }

    TYPED_TEST(RZFixedArrayTest, MoveAssignmentOperator)
    {
        RZFixedArray<TypeParam, 10> arr1;
        arr1.push_back(TypeParam(1));
        arr1.push_back(TypeParam(2));

        RZFixedArray<TypeParam, 10> arr2;
        arr2 = std::move(arr1);

        EXPECT_EQ(arr2.size(), 2);
        EXPECT_EQ(arr2[0], TypeParam(1));
        EXPECT_EQ(arr2[1], TypeParam(2));
    }

    // ============================================================================
    // Element Access Tests
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, OperatorBrackets)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(5));
        arr.push_back(TypeParam(10));

        EXPECT_EQ(arr[0], TypeParam(5));
        EXPECT_EQ(arr[1], TypeParam(10));
    }

    TYPED_TEST(RZFixedArrayTest, OperatorBracketsConst)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(5));
        arr.push_back(TypeParam(10));

        const RZFixedArray<TypeParam, 10>& constArr = arr;
        EXPECT_EQ(constArr[0], TypeParam(5));
        EXPECT_EQ(constArr[1], TypeParam(10));
    }

    TYPED_TEST(RZFixedArrayTest, AtMethod)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(5));
        arr.push_back(TypeParam(10));

        EXPECT_EQ(arr.at(0), TypeParam(5));
        EXPECT_EQ(arr.at(1), TypeParam(10));
    }

    TYPED_TEST(RZFixedArrayTest, AtMethodConst)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(5));
        arr.push_back(TypeParam(10));

        const RZFixedArray<TypeParam, 10>& constArr = arr;
        EXPECT_EQ(constArr.at(0), TypeParam(5));
        EXPECT_EQ(constArr.at(1), TypeParam(10));
    }

    TYPED_TEST(RZFixedArrayTest, FrontAndBack)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        EXPECT_EQ(arr.front(), TypeParam(1));
        EXPECT_EQ(arr.back(), TypeParam(3));
    }

    TYPED_TEST(RZFixedArrayTest, FrontAndBackConst)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        const RZFixedArray<TypeParam, 10>& constArr = arr;
        EXPECT_EQ(constArr.front(), TypeParam(1));
        EXPECT_EQ(constArr.back(), TypeParam(3));
    }

    // ============================================================================
    // Iterator Tests
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, BeginAndEnd)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        auto it = arr.begin();
        EXPECT_EQ(*it, TypeParam(1));
        ++it;
        EXPECT_EQ(*it, TypeParam(2));
        ++it;
        EXPECT_EQ(*it, TypeParam(3));
        ++it;
        EXPECT_EQ(it, arr.end());
    }

    TYPED_TEST(RZFixedArrayTest, ConstBeginAndEnd)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        const RZFixedArray<TypeParam, 10>& constArr = arr;
        auto                               it       = constArr.begin();
        EXPECT_EQ(*it, TypeParam(1));
        ++it;
        EXPECT_EQ(*it, TypeParam(2));
        ++it;
        EXPECT_EQ(*it, TypeParam(3));
        ++it;
        EXPECT_EQ(it, constArr.end());
    }

    TYPED_TEST(RZFixedArrayTest, CBeginAndCEnd)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        auto it = arr.cbegin();
        EXPECT_EQ(*it, TypeParam(1));
        ++it;
        EXPECT_EQ(*it, TypeParam(2));
        ++it;
        EXPECT_EQ(*it, TypeParam(3));
        ++it;
        EXPECT_EQ(it, arr.cend());
    }

    TYPED_TEST(RZFixedArrayTest, IteratorRangeBasedLoop)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        int i = 1;
        for (auto val: arr) {
            EXPECT_EQ(val, TypeParam(i));
            i++;
        }
    }

    TYPED_TEST(RZFixedArrayTest, IteratorSTLAlgorithms)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(3));
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        std::sort(arr.begin(), arr.end());

        EXPECT_EQ(arr[0], TypeParam(1));
        EXPECT_EQ(arr[1], TypeParam(2));
        EXPECT_EQ(arr[2], TypeParam(3));
    }

    TYPED_TEST(RZFixedArrayTest, IteratorPointerArithmetic)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        auto it = arr.begin();
        EXPECT_EQ(*(it + 1), TypeParam(2));
        EXPECT_EQ(*(it + 2), TypeParam(3));
    }

    // ============================================================================
    // Capacity Tests
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, EmptyOnCreation)
    {
        RZFixedArray<TypeParam, 10> arr;
        EXPECT_TRUE(arr.empty());
    }

    TYPED_TEST(RZFixedArrayTest, NotEmptyAfterPushBack)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        EXPECT_FALSE(arr.empty());
    }

    TYPED_TEST(RZFixedArrayTest, Size)
    {
        RZFixedArray<TypeParam, 10> arr;
        EXPECT_EQ(arr.size(), 0);

        arr.push_back(TypeParam(1));
        EXPECT_EQ(arr.size(), 1);

        arr.push_back(TypeParam(2));
        EXPECT_EQ(arr.size(), 2);

        arr.pop_back();
        EXPECT_EQ(arr.size(), 1);
    }

    TYPED_TEST(RZFixedArrayTest, MaxSize)
    {
        RZFixedArray<TypeParam, 10> arr;
        EXPECT_EQ(arr.max_size(), 10);
    }

    TYPED_TEST(RZFixedArrayTest, Capacity)
    {
        RZFixedArray<TypeParam, 10> arr;
        EXPECT_EQ(arr.capacity(), 10);
    }

    // ============================================================================
    // Modification Tests - Push Back
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, PushBackLValue)
    {
        RZFixedArray<TypeParam, 10> arr;
        TypeParam                   val = TypeParam(5);
        arr.push_back(val);

        EXPECT_EQ(arr.size(), 1);
        EXPECT_EQ(arr[0], TypeParam(5));
    }

    TYPED_TEST(RZFixedArrayTest, PushBackRValue)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(5));

        EXPECT_EQ(arr.size(), 1);
        EXPECT_EQ(arr[0], TypeParam(5));
    }

    TYPED_TEST(RZFixedArrayTest, PushBackMultiple)
    {
        RZFixedArray<TypeParam, 10> arr;
        for (int i = 0; i < 5; i++) {
            arr.push_back(TypeParam(i));
        }

        EXPECT_EQ(arr.size(), 5);
        for (int i = 0; i < 5; i++) {
            EXPECT_EQ(arr[i], TypeParam(i));
        }
    }

    TYPED_TEST(RZFixedArrayTest, PushBackToCapacity)
    {
        RZFixedArray<TypeParam, 10> arr;
        for (int i = 0; i < 10; i++) {
            arr.push_back(TypeParam(i));
        }

        EXPECT_EQ(arr.size(), 10);
        EXPECT_FALSE(arr.empty());
    }

    // ============================================================================
    // Modification Tests - Pop Back
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, PopBack)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        arr.pop_back();
        EXPECT_EQ(arr.size(), 2);
        EXPECT_EQ(arr.back(), TypeParam(2));
    }

    TYPED_TEST(RZFixedArrayTest, PopBackMultiple)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        arr.pop_back();
        arr.pop_back();
        EXPECT_EQ(arr.size(), 1);
        EXPECT_EQ(arr.back(), TypeParam(1));
    }

    TYPED_TEST(RZFixedArrayTest, PopBackUntilEmpty)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.pop_back();
        EXPECT_TRUE(arr.empty());
    }

    // ============================================================================
    // Modification Tests - Emplace Back
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, EmplaceBackNoArgs)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.emplace_back();
        EXPECT_EQ(arr.size(), 1);
    }

    TYPED_TEST(RZFixedArrayTest, EmplaceBackWithValue)
    {
        RZFixedArray<TypeParam, 10> arr;
        TypeParam&                  ref = arr.emplace_back(TypeParam(5));
        EXPECT_EQ(arr.size(), 1);
        EXPECT_EQ(ref, TypeParam(5));
        EXPECT_EQ(arr[0], TypeParam(5));
    }

    TYPED_TEST(RZFixedArrayTest, EmplaceBackReturnsReference)
    {
        RZFixedArray<TypeParam, 10> arr;
        TypeParam&                  ref = arr.emplace_back(TypeParam(10));
        ref                             = TypeParam(20);
        EXPECT_EQ(arr[0], TypeParam(20));
    }

    TYPED_TEST(RZFixedArrayTest, EmplaceBackMultiple)
    {
        RZFixedArray<TypeParam, 10> arr;
        for (int i = 0; i < 5; i++) {
            arr.emplace_back(TypeParam(i));
        }
        EXPECT_EQ(arr.size(), 5);
    }

    // ============================================================================
    // Modification Tests - Resize
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, ResizeIncrease)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        arr.resize(5);
        EXPECT_EQ(arr.size(), 5);
        EXPECT_EQ(arr[0], TypeParam(1));
        EXPECT_EQ(arr[1], TypeParam(2));
        EXPECT_EQ(arr[2], TypeParam(0));
        EXPECT_EQ(arr[3], TypeParam(0));
        EXPECT_EQ(arr[4], TypeParam(0));
    }

    TYPED_TEST(RZFixedArrayTest, ResizeIncreaseWithValue)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        arr.resize(5, TypeParam(9));
        EXPECT_EQ(arr.size(), 5);
        EXPECT_EQ(arr[0], TypeParam(1));
        EXPECT_EQ(arr[1], TypeParam(2));
        EXPECT_EQ(arr[2], TypeParam(9));
        EXPECT_EQ(arr[3], TypeParam(9));
        EXPECT_EQ(arr[4], TypeParam(9));
    }

    TYPED_TEST(RZFixedArrayTest, ResizeDecrease)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        arr.resize(2);
        EXPECT_EQ(arr.size(), 2);
        EXPECT_EQ(arr[0], TypeParam(1));
        EXPECT_EQ(arr[1], TypeParam(2));
    }

    TYPED_TEST(RZFixedArrayTest, ResizeToZero)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        arr.resize(0);
        EXPECT_TRUE(arr.empty());
        EXPECT_EQ(arr.size(), 0);
    }

    TYPED_TEST(RZFixedArrayTest, ResizeNoChange)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        arr.resize(2);
        EXPECT_EQ(arr.size(), 2);
        EXPECT_EQ(arr[0], TypeParam(1));
        EXPECT_EQ(arr[1], TypeParam(2));
    }

    // ============================================================================
    // Modification Tests - Clear
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, ClearEmptyArray)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.clear();
        EXPECT_TRUE(arr.empty());
        EXPECT_EQ(arr.size(), 0);
    }

    TYPED_TEST(RZFixedArrayTest, ClearNonEmptyArray)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        arr.clear();
        EXPECT_TRUE(arr.empty());
        EXPECT_EQ(arr.size(), 0);
    }

    TYPED_TEST(RZFixedArrayTest, ClearAndReuse)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        arr.clear();
        arr.push_back(TypeParam(5));

        EXPECT_EQ(arr.size(), 1);
        EXPECT_EQ(arr[0], TypeParam(5));
    }

    // ============================================================================
    // Mutation Tests
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, MutateViaOperatorBrackets)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr[0] = TypeParam(10);
        EXPECT_EQ(arr[0], TypeParam(10));
    }

    TYPED_TEST(RZFixedArrayTest, MutateViaAt)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.at(0) = TypeParam(10);
        EXPECT_EQ(arr.at(0), TypeParam(10));
    }

    TYPED_TEST(RZFixedArrayTest, MutateViaIterator)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        auto it = arr.begin();
        *it     = TypeParam(100);
        EXPECT_EQ(arr[0], TypeParam(100));
    }

    TYPED_TEST(RZFixedArrayTest, MutateViaFrontBack)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        arr.front() = TypeParam(10);
        arr.back()  = TypeParam(20);

        EXPECT_EQ(arr[0], TypeParam(10));
        EXPECT_EQ(arr[1], TypeParam(20));
    }

    // ============================================================================
    // Edge Cases and Special Tests
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, AssignmentChaining)
    {
        RZFixedArray<TypeParam, 10> arr1, arr2, arr3;
        arr1.push_back(TypeParam(5));

        arr3 = arr2 = arr1;

        EXPECT_EQ(arr2.size(), 1);
        EXPECT_EQ(arr3.size(), 1);
        EXPECT_EQ(arr3[0], TypeParam(5));
    }

    TYPED_TEST(RZFixedArrayTest, SelfAssignment)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        arr = arr;

        EXPECT_EQ(arr.size(), 2);
        EXPECT_EQ(arr[0], TypeParam(1));
        EXPECT_EQ(arr[1], TypeParam(2));
    }

    TYPED_TEST(RZFixedArrayTest, MultipleIterations)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        // First iteration
        int count1 = 0;
        for (auto val: arr) {
            count1++;
        }

        // Second iteration
        int count2 = 0;
        for (auto val: arr) {
            count2++;
        }

        EXPECT_EQ(count1, 3);
        EXPECT_EQ(count2, 3);
    }

    TYPED_TEST(RZFixedArrayTest, TypeAliases)
    {
        RZFixedArray<TypeParam, 10> arr;

        // Verify type aliases are accessible
        static_assert(std::is_same_v<typename RZFixedArray<TypeParam, 10>::value_type, TypeParam>);
        static_assert(std::is_same_v<typename RZFixedArray<TypeParam, 10>::size_type, size_t>);
    }

    // ============================================================================
    // Complex Type Tests (if applicable)
    // ============================================================================

    class ComplexType
    {
    public:
        int        value = 0;
        static int constructCount;
        static int destructCount;
        static int copyCount;
        static int moveCount;

        ComplexType() { constructCount++; }
        ~ComplexType() { destructCount++; }

        ComplexType(int v)
            : value(v) { constructCount++; }

        ComplexType(const ComplexType& other)
            : value(other.value)
        {
            copyCount++;
        }

        ComplexType& operator=(const ComplexType& other)
        {
            value = other.value;
            copyCount++;
            return *this;
        }

        ComplexType(ComplexType&& other) noexcept
            : value(other.value)
        {
            moveCount++;
        }

        ComplexType& operator=(ComplexType&& other) noexcept
        {
            value = other.value;
            moveCount++;
            return *this;
        }

        bool operator==(const ComplexType& other) const { return value == other.value; }
        bool operator<(const ComplexType& other) const { return value < other.value; }
    };

    int ComplexType::constructCount = 0;
    int ComplexType::destructCount  = 0;
    int ComplexType::copyCount      = 0;
    int ComplexType::moveCount      = 0;

    class ComplexTypeTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            ComplexType::constructCount = 0;
            ComplexType::destructCount  = 0;
            ComplexType::copyCount      = 0;
            ComplexType::moveCount      = 0;
        }
    };

    TEST_F(ComplexTypeTest, MoveSemantics)
    {
        RZFixedArray<ComplexType, 10> arr;
        ComplexType                   ct(5);
        int                           moveBefore = ComplexType::moveCount;

        arr.push_back(std::move(ct));

        EXPECT_EQ(arr[0].value, 5);
    }

    TEST_F(ComplexTypeTest, CopySemantics)
    {
        RZFixedArray<ComplexType, 10> arr;
        ComplexType                   ct(5);

        arr.push_back(ct);

        EXPECT_EQ(arr[0].value, 5);
    }

    TEST_F(ComplexTypeTest, EmplaceBackWithComplexType)
    {
        RZFixedArray<ComplexType, 10> arr;
        arr.emplace_back(42);

        EXPECT_EQ(arr[0].value, 42);
        EXPECT_EQ(arr.size(), 1);
    }

    TEST_F(ComplexTypeTest, ResizeWithComplexType)
    {
        RZFixedArray<ComplexType, 10> arr;
        arr.push_back(ComplexType(1));
        arr.push_back(ComplexType(2));

        arr.resize(5, ComplexType(99));

        EXPECT_EQ(arr.size(), 5);
        EXPECT_EQ(arr[2].value, 99);
        EXPECT_EQ(arr[3].value, 99);
        EXPECT_EQ(arr[4].value, 99);
    }

    TEST_F(ComplexTypeTest, ClearDestructsObjects)
    {
        int destroyBefore = ComplexType::destructCount;

        {
            RZFixedArray<ComplexType, 10> arr;
            arr.push_back(ComplexType(1));
            arr.push_back(ComplexType(2));
            arr.push_back(ComplexType(3));
            arr.clear();
        }

        EXPECT_GT(ComplexType::destructCount, destroyBefore);
    }

    TEST_F(ComplexTypeTest, CopyConstructorWithComplexType)
    {
        RZFixedArray<ComplexType, 10> arr1;
        arr1.push_back(ComplexType(10));
        arr1.push_back(ComplexType(20));

        int                           copysBefore = ComplexType::copyCount;
        RZFixedArray<ComplexType, 10> arr2(arr1);
        int                           copiesAfter = ComplexType::copyCount;

        EXPECT_EQ(arr2.size(), 2);
        EXPECT_EQ(arr2[0].value, 10);
        EXPECT_EQ(arr2[1].value, 20);
        EXPECT_GT(copiesAfter, copysBefore);
    }

    TEST_F(ComplexTypeTest, MoveConstructorWithComplexType)
    {
        RZFixedArray<ComplexType, 10> arr1;
        arr1.push_back(ComplexType(10));
        arr1.push_back(ComplexType(20));

        int                           movesBefore = ComplexType::moveCount;
        RZFixedArray<ComplexType, 10> arr2(std::move(arr1));
        int                           movesAfter = ComplexType::moveCount;

        EXPECT_EQ(arr2.size(), 2);
        EXPECT_EQ(arr2[0].value, 10);
        EXPECT_EQ(arr2[1].value, 20);
    }

    // ============================================================================
    // Boundary and Stress Tests
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, AccessAllElements)
    {
        RZFixedArray<TypeParam, 10> arr;
        for (int i = 0; i < 10; i++) {
            arr.push_back(TypeParam(i));
        }

        for (int i = 0; i < 10; i++) {
            EXPECT_EQ(arr[i], TypeParam(i));
        }
    }

    TYPED_TEST(RZFixedArrayTest, PushPopPattern)
    {
        RZFixedArray<TypeParam, 10> arr;

        // Push some elements
        for (int i = 0; i < 5; i++) {
            arr.push_back(TypeParam(i));
        }

        // Pop some elements
        for (int i = 0; i < 2; i++) {
            arr.pop_back();
        }

        EXPECT_EQ(arr.size(), 3);

        // Push more
        for (int i = 5; i < 8; i++) {
            arr.push_back(TypeParam(i));
        }

        EXPECT_EQ(arr.size(), 6);
    }

    TYPED_TEST(RZFixedArrayTest, ResizePatterns)
    {
        RZFixedArray<TypeParam, 10> arr;

        arr.resize(5, TypeParam(1));
        EXPECT_EQ(arr.size(), 5);

        arr.resize(3);
        EXPECT_EQ(arr.size(), 3);

        arr.resize(8, TypeParam(2));
        EXPECT_EQ(arr.size(), 8);

        arr.resize(0);
        EXPECT_EQ(arr.size(), 0);
    }

    TYPED_TEST(RZFixedArrayTest, AlternatingPushPopClear)
    {
        RZFixedArray<TypeParam, 10> arr;

        for (int cycle = 0; cycle < 3; cycle++) {
            arr.push_back(TypeParam(1));
            arr.push_back(TypeParam(2));
            arr.push_back(TypeParam(3));
            EXPECT_EQ(arr.size(), 3);

            arr.pop_back();
            EXPECT_EQ(arr.size(), 2);

            arr.clear();
            EXPECT_TRUE(arr.empty());
        }
    }

    TYPED_TEST(RZFixedArrayTest, LargeNumberOfOperations)
    {
        RZFixedArray<TypeParam, 100> arr;

        // Fill to capacity with smaller size
        for (int i = 0; i < 50; i++) {
            arr.push_back(TypeParam(i % 10));
        }

        EXPECT_EQ(arr.size(), 50);

        // Verify all elements
        for (int i = 0; i < 50; i++) {
            EXPECT_EQ(arr[i], TypeParam(i % 10));
        }

        // Shrink and expand
        arr.resize(25);
        EXPECT_EQ(arr.size(), 25);

        arr.resize(75, TypeParam(5));
        EXPECT_EQ(arr.size(), 75);
    }

    TYPED_TEST(RZFixedArrayTest, IteratorValidityAfterOperations)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        auto it = arr.begin();
        EXPECT_EQ(*it, TypeParam(1));

        arr.push_back(TypeParam(4));
        EXPECT_EQ(*it, TypeParam(1));    // Iterator should still be valid

        arr.pop_back();
        EXPECT_EQ(*it, TypeParam(1));    // Iterator should still be valid

        ++it;
        EXPECT_EQ(*it, TypeParam(2));
    }

    TYPED_TEST(RZFixedArrayTest, FindElementWithSTLFind)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));
        arr.push_back(TypeParam(2));

        auto it = std::find(arr.begin(), arr.end(), TypeParam(2));
        EXPECT_NE(it, arr.end());
        EXPECT_EQ(*it, TypeParam(2));
    }

    TYPED_TEST(RZFixedArrayTest, CountElementsWithSTLCount)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));
        arr.push_back(TypeParam(2));

        auto count = std::count(arr.begin(), arr.end(), TypeParam(2));
        EXPECT_EQ(count, 3);
    }

    // ============================================================================
    // State Consistency Tests
    // ============================================================================

    TYPED_TEST(RZFixedArrayTest, SizeMatchesElementCount)
    {
        RZFixedArray<TypeParam, 10> arr;

        for (int i = 0; i < 7; i++) {
            arr.push_back(TypeParam(i));
            EXPECT_EQ(arr.size(), i + 1);
        }

        for (int i = 7; i > 0; i--) {
            arr.pop_back();
            EXPECT_EQ(arr.size(), i - 1);
        }
    }

    TYPED_TEST(RZFixedArrayTest, IteratorCountMatchesSize)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));
        arr.push_back(TypeParam(3));

        auto distance = std::distance(arr.begin(), arr.end());
        EXPECT_EQ(static_cast<size_t>(distance), arr.size());
    }

    TYPED_TEST(RZFixedArrayTest, EmptyAfterClear)
    {
        RZFixedArray<TypeParam, 10> arr;
        arr.push_back(TypeParam(1));
        arr.push_back(TypeParam(2));

        arr.clear();

        EXPECT_TRUE(arr.empty());
        EXPECT_EQ(arr.size(), 0);
        EXPECT_EQ(arr.begin(), arr.end());
    }
}    // namespace Razix
