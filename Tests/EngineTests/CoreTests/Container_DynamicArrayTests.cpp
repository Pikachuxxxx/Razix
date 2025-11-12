// AI-generated unit tests for the RZDynamicArray container class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Log/RZLog.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace Razix {

    // Define test types
    using TestTypes = ::testing::Types<int, double, float, char>;

    // Generic fixture for typed tests
    template<typename T>
    class RZDynamicArrayTypedTest : public ::testing::Test
    {
    protected:
        RZDynamicArray<T> arr;

        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();
        }

        void TearDown() override
        {
            Razix::Debug::RZLog::Shutdown();
        }
    };

    TYPED_TEST_SUITE(RZDynamicArrayTypedTest, TestTypes);

    // ============================================================================
    // CONSTRUCTOR TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, DefaultConstructor)
    {
        EXPECT_EQ(this->arr.size(), 0);
        EXPECT_EQ(this->arr.capacity(), 0);
        EXPECT_TRUE(this->arr.empty());
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ConstructorWithCapacity)
    {
        RZDynamicArray<TypeParam> arr(10);
        EXPECT_EQ(arr.size(), 0);
        EXPECT_GE(arr.capacity(), 10);
        EXPECT_TRUE(arr.empty());
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ConstructorWithCapacityAndValue)
    {
        TypeParam                 value = static_cast<TypeParam>(42);
        RZDynamicArray<TypeParam> arr(5, value);
        EXPECT_EQ(arr.size(), 5);
        EXPECT_GE(arr.capacity(), 5);
        EXPECT_FALSE(arr.empty());
        for (int i = 0; i < 5; ++i) {
            EXPECT_EQ(arr[i], value);
        }
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ConstructorZeroCapacity)
    {
        RZDynamicArray<TypeParam> arr(0);
        EXPECT_EQ(arr.size(), 0);
        EXPECT_EQ(arr.capacity(), 0);
        EXPECT_TRUE(arr.empty());
    }

    // ============================================================================
    // ACCESS OPERATORS TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, OperatorBracketAccess)
    {
        TypeParam                 value = static_cast<TypeParam>(100);
        RZDynamicArray<TypeParam> arr(5, value);
        EXPECT_EQ(arr[0], value);
        EXPECT_EQ(arr[2], value);
        EXPECT_EQ(arr[4], value);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, OperatorBracketModify)
    {
        RZDynamicArray<TypeParam> arr(3, static_cast<TypeParam>(0));
        arr[0] = static_cast<TypeParam>(10);
        arr[1] = static_cast<TypeParam>(20);
        arr[2] = static_cast<TypeParam>(30);
        EXPECT_EQ(arr[0], static_cast<TypeParam>(10));
        EXPECT_EQ(arr[1], static_cast<TypeParam>(20));
        EXPECT_EQ(arr[2], static_cast<TypeParam>(30));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, AtMethodValid)
    {
        TypeParam                 value = static_cast<TypeParam>(77);
        RZDynamicArray<TypeParam> arr(5, value);
        EXPECT_EQ(arr.at(0), value);
        EXPECT_EQ(arr.at(2), value);
        EXPECT_EQ(arr.at(4), value);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, AtMethodBoundary)
    {
        TypeParam                 value = static_cast<TypeParam>(42);
        RZDynamicArray<TypeParam> arr(5, value);
        EXPECT_EQ(arr.at(0), value);    // First element
        EXPECT_EQ(arr.at(4), value);    // Last element
    }

    TYPED_TEST(RZDynamicArrayTypedTest, FrontAndBackOnSingleElement)
    {
        TypeParam                 value = static_cast<TypeParam>(42);
        RZDynamicArray<TypeParam> arr(1, value);
        EXPECT_EQ(arr.front(), value);
        EXPECT_EQ(arr.back(), value);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, FrontAndBackMultipleElements)
    {
        RZDynamicArray<TypeParam> arr(5, static_cast<TypeParam>(0));
        arr[0] = static_cast<TypeParam>(10);
        arr[4] = static_cast<TypeParam>(99);
        EXPECT_EQ(arr.front(), static_cast<TypeParam>(10));
        EXPECT_EQ(arr.back(), static_cast<TypeParam>(99));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ConstFrontAndBack)
    {
        TypeParam                       value = static_cast<TypeParam>(55);
        const RZDynamicArray<TypeParam> arr(3, value);
        EXPECT_EQ(arr.front(), value);
        EXPECT_EQ(arr.back(), value);
    }

    // ============================================================================
    // ITERATOR TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, BeginEndIterators)
    {
        RZDynamicArray<TypeParam> arr(5, static_cast<TypeParam>(0));
        for (int i = 0; i < 5; ++i) {
            arr[i] = static_cast<TypeParam>(i * 10);
        }

        TypeParam expected = static_cast<TypeParam>(0);
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            EXPECT_EQ(*it, expected);
            expected += static_cast<TypeParam>(10);
        }
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ConstBeginEndIterators)
    {
        RZDynamicArray<TypeParam> arr(4, static_cast<TypeParam>(0));
        arr[0] = static_cast<TypeParam>(1);
        arr[1] = static_cast<TypeParam>(2);
        arr[2] = static_cast<TypeParam>(3);
        arr[3] = static_cast<TypeParam>(4);

        const RZDynamicArray<TypeParam>& constArr = arr;
        TypeParam                        expected = static_cast<TypeParam>(1);
        for (auto it = constArr.begin(); it != constArr.end(); ++it) {
            EXPECT_EQ(*it, expected);
            expected += static_cast<TypeParam>(1);
        }
    }

    TYPED_TEST(RZDynamicArrayTypedTest, CBeginCEndIterators)
    {
        RZDynamicArray<TypeParam> arr(3, static_cast<TypeParam>(0));
        arr[0] = static_cast<TypeParam>(5);
        arr[1] = static_cast<TypeParam>(10);
        arr[2] = static_cast<TypeParam>(15);

        TypeParam expected = static_cast<TypeParam>(5);
        for (auto it = arr.cbegin(); it != arr.cend(); ++it) {
            EXPECT_EQ(*it, expected);
            expected += static_cast<TypeParam>(5);
        }
    }

    TYPED_TEST(RZDynamicArrayTypedTest, IteratorModification)
    {
        RZDynamicArray<TypeParam> arr(3, static_cast<TypeParam>(0));
        TypeParam                 newValue = static_cast<TypeParam>(42);
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            *it = newValue;
        }
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            EXPECT_EQ(*it, newValue);
        }
    }

    TYPED_TEST(RZDynamicArrayTypedTest, RangeBasedForLoop)
    {
        RZDynamicArray<TypeParam> arr(4, static_cast<TypeParam>(0));
        arr[0] = static_cast<TypeParam>(1);
        arr[1] = static_cast<TypeParam>(2);
        arr[2] = static_cast<TypeParam>(3);
        arr[3] = static_cast<TypeParam>(4);

        int index = 0;
        for (TypeParam val: arr) {
            EXPECT_EQ(val, static_cast<TypeParam>(index + 1));
            index++;
        }
    }

    TYPED_TEST(RZDynamicArrayTypedTest, EmptyIterators)
    {
        RZDynamicArray<TypeParam> arr(10);
        EXPECT_EQ(arr.begin(), arr.end());
        EXPECT_EQ(arr.cbegin(), arr.cend());
    }

    // ============================================================================
    // CAPACITY TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, EmptyOnNewArray)
    {
        RZDynamicArray<TypeParam> arr;
        EXPECT_TRUE(arr.empty());
    }

    TYPED_TEST(RZDynamicArrayTypedTest, EmptyOnNonEmptyArray)
    {
        RZDynamicArray<TypeParam> arr(5, static_cast<TypeParam>(1));
        EXPECT_FALSE(arr.empty());
    }

    TYPED_TEST(RZDynamicArrayTypedTest, SizeAfterConstruction)
    {
        RZDynamicArray<TypeParam> arr(0);
        EXPECT_EQ(arr.size(), 0);

        RZDynamicArray<TypeParam> arr2(10);
        EXPECT_EQ(arr2.size(), 0);

        RZDynamicArray<TypeParam> arr3(5, static_cast<TypeParam>(10));
        EXPECT_EQ(arr3.size(), 5);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, CapacityAfterConstruction)
    {
        RZDynamicArray<TypeParam> arr(10);
        EXPECT_GE(arr.capacity(), 10);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, CapacityGreaterOrEqualSize)
    {
        RZDynamicArray<TypeParam> arr(5, static_cast<TypeParam>(1));
        EXPECT_GE(arr.capacity(), arr.size());
    }

    // ============================================================================
    // PUSH_BACK TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, PushBackSingleElement)
    {
        TypeParam value = static_cast<TypeParam>(42);
        this->arr.push_back(value);
        EXPECT_EQ(this->arr.size(), 1);
        EXPECT_EQ(this->arr[0], value);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, PushBackMultipleElements)
    {
        for (int i = 0; i < 10; ++i) {
            this->arr.push_back(static_cast<TypeParam>(i));
        }
        EXPECT_EQ(this->arr.size(), 10);
        for (int i = 0; i < 10; ++i) {
            EXPECT_EQ(this->arr[i], static_cast<TypeParam>(i));
        }
    }

    TYPED_TEST(RZDynamicArrayTypedTest, PushBackCausesResize)
    {
        RZDynamicArray<TypeParam> arr(2);
        size_t                    initialCapacity = arr.capacity();
        arr.push_back(static_cast<TypeParam>(1));
        arr.push_back(static_cast<TypeParam>(2));
        arr.push_back(static_cast<TypeParam>(3));    // This should trigger resize
        EXPECT_GE(arr.capacity(), initialCapacity);
        EXPECT_EQ(arr.size(), 3);
        EXPECT_EQ(arr[2], static_cast<TypeParam>(3));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, PushBackManyElements)
    {
        for (int i = 0; i < 1000; ++i) {
            this->arr.push_back(static_cast<TypeParam>(i % 100));
            EXPECT_GE(this->arr.capacity(), this->arr.size());
        }
        EXPECT_EQ(this->arr.size(), 1000);
    }

    // ============================================================================
    // EMPLACE_BACK TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, EmplaceBackSingleElement)
    {
        auto& ref = this->arr.emplace_back(static_cast<TypeParam>(99));
        EXPECT_EQ(this->arr.size(), 1);
        EXPECT_EQ(this->arr[0], static_cast<TypeParam>(99));
        EXPECT_EQ(ref, static_cast<TypeParam>(99));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, EmplaceBackReturnsReference)
    {
        auto& ref1 = this->arr.emplace_back(static_cast<TypeParam>(10));
        ref1       = static_cast<TypeParam>(20);
        EXPECT_EQ(this->arr[0], static_cast<TypeParam>(20));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, EmplaceBackMultipleElements)
    {
        this->arr.emplace_back(static_cast<TypeParam>(1));
        this->arr.emplace_back(static_cast<TypeParam>(2));
        this->arr.emplace_back(static_cast<TypeParam>(3));
        EXPECT_EQ(this->arr.size(), 3);
        EXPECT_EQ(this->arr[0], static_cast<TypeParam>(1));
        EXPECT_EQ(this->arr[1], static_cast<TypeParam>(2));
        EXPECT_EQ(this->arr[2], static_cast<TypeParam>(3));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, EmplaceBackCausesResize)
    {
        RZDynamicArray<TypeParam> arr(2);
        arr.emplace_back(static_cast<TypeParam>(1));
        arr.emplace_back(static_cast<TypeParam>(2));
        auto& ref = arr.emplace_back(static_cast<TypeParam>(3));    // Should trigger resize
        EXPECT_EQ(arr.size(), 3);
        EXPECT_EQ(ref, static_cast<TypeParam>(3));
        EXPECT_GE(arr.capacity(), 3);
    }

    // ============================================================================
    // POP_BACK TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, PopBackSingleElement)
    {
        this->arr.push_back(static_cast<TypeParam>(42));
        this->arr.pop_back();
        EXPECT_EQ(this->arr.size(), 0);
        EXPECT_TRUE(this->arr.empty());
    }

    TYPED_TEST(RZDynamicArrayTypedTest, PopBackMultipleElements)
    {
        for (int i = 0; i < 5; ++i) {
            this->arr.push_back(static_cast<TypeParam>(i));
        }
        this->arr.pop_back();
        EXPECT_EQ(this->arr.size(), 4);
        EXPECT_EQ(this->arr.back(), static_cast<TypeParam>(3));
        this->arr.pop_back();
        EXPECT_EQ(this->arr.size(), 3);
        EXPECT_EQ(this->arr.back(), static_cast<TypeParam>(2));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, PopBackDoesNotChangeSizeNegatively)
    {
        this->arr.push_back(static_cast<TypeParam>(1));
        this->arr.push_back(static_cast<TypeParam>(2));
        this->arr.pop_back();
        this->arr.pop_back();
        EXPECT_EQ(this->arr.size(), 0);
    }

    // ============================================================================
    // RESIZE TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, ResizeExpand)
    {
        RZDynamicArray<TypeParam> arr(2, static_cast<TypeParam>(5));
        arr.resize(5);
        EXPECT_EQ(arr.size(), 5);
        EXPECT_EQ(arr[0], static_cast<TypeParam>(5));
        EXPECT_EQ(arr[1], static_cast<TypeParam>(5));
        EXPECT_EQ(arr[2], static_cast<TypeParam>(0));    // Default-constructed
        EXPECT_EQ(arr[3], static_cast<TypeParam>(0));
        EXPECT_EQ(arr[4], static_cast<TypeParam>(0));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ResizeExpandWithValue)
    {
        RZDynamicArray<TypeParam> arr(2, static_cast<TypeParam>(5));
        arr.resize(5, static_cast<TypeParam>(99));
        EXPECT_EQ(arr.size(), 5);
        EXPECT_EQ(arr[0], static_cast<TypeParam>(5));
        EXPECT_EQ(arr[1], static_cast<TypeParam>(5));
        EXPECT_EQ(arr[2], static_cast<TypeParam>(99));
        EXPECT_EQ(arr[3], static_cast<TypeParam>(99));
        EXPECT_EQ(arr[4], static_cast<TypeParam>(99));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ResizeShrink)
    {
        RZDynamicArray<TypeParam> arr(5, static_cast<TypeParam>(1));
        arr.resize(2);
        EXPECT_EQ(arr.size(), 2);
        EXPECT_EQ(arr[0], static_cast<TypeParam>(1));
        EXPECT_EQ(arr[1], static_cast<TypeParam>(1));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ResizeToZero)
    {
        RZDynamicArray<TypeParam> arr(5, static_cast<TypeParam>(42));
        arr.resize(0);
        EXPECT_EQ(arr.size(), 0);
        EXPECT_TRUE(arr.empty());
    }

    // ============================================================================
    // RESERVE TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, ReserveIncreasesCapacity)
    {
        this->arr.reserve(50);
        EXPECT_GE(this->arr.capacity(), 50);
        EXPECT_EQ(this->arr.size(), 0);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ReserveSmallCapacity)
    {
        RZDynamicArray<TypeParam> arr(10);
        size_t                    oldCapacity = arr.capacity();
        arr.reserve(5);    // Smaller than current capacity
        EXPECT_GE(arr.capacity(), oldCapacity);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ReservePreventResize)
    {
        this->arr.reserve(100);
        for (int i = 0; i < 100; ++i) {
            this->arr.push_back(static_cast<TypeParam>(i % 50));
        }
        EXPECT_EQ(this->arr.size(), 100);
        EXPECT_GE(this->arr.capacity(), 100);
    }

    // ============================================================================
    // CLEAR TESTS (All Types)
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, ClearEmptiesArray)
    {
        this->arr.push_back(static_cast<TypeParam>(1));
        this->arr.push_back(static_cast<TypeParam>(2));
        this->arr.push_back(static_cast<TypeParam>(3));
        this->arr.clear();
        EXPECT_EQ(this->arr.size(), 0);
        EXPECT_TRUE(this->arr.empty());
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ClearDoesNotChangeCapacity)
    {
        this->arr.push_back(static_cast<TypeParam>(1));
        this->arr.push_back(static_cast<TypeParam>(2));
        size_t capacity = this->arr.capacity();
        this->arr.clear();
        EXPECT_EQ(this->arr.capacity(), capacity);
    }

    // ============================================================================
    // EDGE CASES AND SPECIAL SCENARIOS
    // ============================================================================

    TYPED_TEST(RZDynamicArrayTypedTest, AlternatingPushAndPop)
    {
        this->arr.push_back(static_cast<TypeParam>(1));
        this->arr.push_back(static_cast<TypeParam>(2));
        this->arr.pop_back();
        this->arr.push_back(static_cast<TypeParam>(3));
        this->arr.pop_back();
        this->arr.push_back(static_cast<TypeParam>(4));
        EXPECT_EQ(this->arr.size(), 2);
        EXPECT_EQ(this->arr[0], static_cast<TypeParam>(1));
        EXPECT_EQ(this->arr[1], static_cast<TypeParam>(4));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, LargeScaleTest)
    {
        const int LARGE_SIZE = 10000;
        for (int i = 0; i < LARGE_SIZE; ++i) {
            this->arr.push_back(static_cast<TypeParam>(i % 100));
        }
        EXPECT_EQ(this->arr.size(), LARGE_SIZE);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ResizeWithExistingData)
    {
        this->arr.push_back(static_cast<TypeParam>(10));
        this->arr.push_back(static_cast<TypeParam>(20));
        this->arr.push_back(static_cast<TypeParam>(30));
        this->arr.resize(5, static_cast<TypeParam>(0));
        EXPECT_EQ(this->arr[0], static_cast<TypeParam>(10));
        EXPECT_EQ(this->arr[1], static_cast<TypeParam>(20));
        EXPECT_EQ(this->arr[2], static_cast<TypeParam>(30));
        EXPECT_EQ(this->arr[3], static_cast<TypeParam>(0));
        EXPECT_EQ(this->arr[4], static_cast<TypeParam>(0));
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ConstCorrectness)
    {
        TypeParam value = static_cast<TypeParam>(42);
        this->arr.push_back(value);
        const RZDynamicArray<TypeParam>& constRef = this->arr;
        EXPECT_EQ(constRef[0], value);
        EXPECT_EQ(constRef.at(0), value);
        EXPECT_EQ(constRef.front(), value);
        EXPECT_EQ(constRef.back(), value);
        EXPECT_EQ(constRef.size(), 1);
    }

    TYPED_TEST(RZDynamicArrayTypedTest, ModifyThroughIterator)
    {
        this->arr.push_back(static_cast<TypeParam>(1));
        this->arr.push_back(static_cast<TypeParam>(2));
        this->arr.push_back(static_cast<TypeParam>(3));
        *(this->arr.begin() + 1) = static_cast<TypeParam>(99);
        EXPECT_EQ(this->arr[1], static_cast<TypeParam>(99));
    }

    // ============================================================================
    // STRING-SPECIFIC TESTS (Non-typed)
    // ============================================================================

    class RZDynamicArrayStringTest : public ::testing::Test
    {
    protected:
        RZDynamicArray<std::string> arr;

        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();
        }

        void TearDown() override
        {
            Razix::Debug::RZLog::Shutdown();
        }
    };

    TEST_F(RZDynamicArrayStringTest, ConstructorWithCapacityAndStringValue)
    {
        RZDynamicArray<std::string> arr(3, "hello");
        EXPECT_EQ(arr.size(), 3);
        EXPECT_GE(arr.capacity(), 3);
        for (size_t i = 0; i < 3; ++i) {
            EXPECT_EQ(arr[i], "hello");
        }
    }

    TEST_F(RZDynamicArrayStringTest, PushBackStrings)
    {
        arr.push_back("first");
        arr.push_back("second");
        arr.push_back("third");
        EXPECT_EQ(arr.size(), 3);
        EXPECT_EQ(arr[0], "first");
        EXPECT_EQ(arr[1], "second");
        EXPECT_EQ(arr[2], "third");
    }

    //TEST_F(RZDynamicArrayStringTest, EmplaceBackString)
    //{
    //    auto& ref = arr.emplace_back("hello", 3);    // Construct std::string with "hel"
    //    EXPECT_EQ(arr.size(), 1);
    //    EXPECT_EQ(ref, "hel");
    //}

    TEST_F(RZDynamicArrayStringTest, ResizeWithStrings)
    {
        RZDynamicArray<std::string> arr(2, "a");
        arr.resize(4, "b");
        EXPECT_EQ(arr.size(), 4);
        EXPECT_EQ(arr[0], "a");
        EXPECT_EQ(arr[1], "a");
        EXPECT_EQ(arr[2], "b");
        EXPECT_EQ(arr[3], "b");
    }

    TEST_F(RZDynamicArrayStringTest, ClearWithStrings)
    {
        arr.push_back("hello");
        arr.push_back("world");
        arr.clear();
        EXPECT_EQ(arr.size(), 0);
        EXPECT_TRUE(arr.empty());
    }

    // ============================================================================
    // CUSTOM OBJECT TESTS
    // ============================================================================

    class CustomObject
    {
    public:
        int  value;
        bool constructed = false;

        CustomObject()
            : value(0), constructed(true) {}
        explicit CustomObject(int v)
            : value(v), constructed(true) {}
        CustomObject(const CustomObject& other)
            : value(other.value), constructed(true) {}
        CustomObject(CustomObject&& other) noexcept
            : value(other.value), constructed(true) {}
        ~CustomObject() { constructed = false; }
    };

    TEST(RZDynamicArrayCustomObject, ConstructorCalled)
    {
        RZDynamicArray<CustomObject> arr(3);
        for (size_t i = 0; i < 3; ++i) {
            arr.push_back(CustomObject(i));
            EXPECT_TRUE(arr[i].constructed);
        }
    }

    TEST(RZDynamicArrayCustomObject, EmplaceBackCustomObject)
    {
        RZDynamicArray<CustomObject> arr;
        auto&                        ref = arr.emplace_back(42);
        EXPECT_EQ(arr.size(), 1);
        EXPECT_EQ(arr[0].value, 42);
        EXPECT_TRUE(ref.constructed);
    }

}    // namespace Razix
