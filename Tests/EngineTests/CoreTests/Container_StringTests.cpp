// Container_StringTests.cpp
// AI-generated unit tests for the RZStack container class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Log/RZLog.h"

#include <gtest/gtest.h>
#include <vector>

namespace Razix {

    // ============ Constructor Tests ============
    class RZStringConstructorTests : public ::testing::Test
    {
    };

    TEST_F(RZStringConstructorTests, DefaultConstructor)
    {
        RZString str;
        EXPECT_TRUE(str.empty());
        EXPECT_EQ(str.length(), 0);
        EXPECT_EQ(str.size(), 0);
    }

    TEST_F(RZStringConstructorTests, CStringConstructor)
    {
        RZString str("Hello");
        EXPECT_EQ(str.length(), 5);
        EXPECT_STREQ(str.c_str(), "Hello");
    }

    TEST_F(RZStringConstructorTests, CStringWithCountConstructor)
    {
        RZString str("Hello World", 5);
        EXPECT_EQ(str.length(), 5);
        EXPECT_STREQ(str.c_str(), "Hello");
    }

    TEST_F(RZStringConstructorTests, CharCountConstructor)
    {
        RZString str(5, 'a');
        EXPECT_EQ(str.length(), 5);
        EXPECT_STREQ(str.c_str(), "aaaaa");
    }

    TEST_F(RZStringConstructorTests, CopyConstructor)
    {
        RZString str1("Hello");
        RZString str2(str1);
        EXPECT_EQ(str2.length(), str1.length());
        EXPECT_STREQ(str2.c_str(), str1.c_str());
    }

    TEST_F(RZStringConstructorTests, MoveConstructor)
    {
        RZString str1("Hello");
        RZString str2(std::move(str1));
        EXPECT_EQ(str2.length(), 5);
        EXPECT_STREQ(str2.c_str(), "Hello");
        EXPECT_TRUE(str1.empty());
    }

    TEST_F(RZStringConstructorTests, SubstringConstructor)
    {
        RZString str1("Hello World");
        RZString str2(str1, 0, 5);
        EXPECT_EQ(str2.length(), 5);
        EXPECT_STREQ(str2.c_str(), "Hello");
    }

    TEST_F(RZStringConstructorTests, SubstringConstructorWithNpos)
    {
        RZString str1("Hello World");
        RZString str2(str1, 6, RZString::npos);
        EXPECT_STREQ(str2.c_str(), "World");
    }

    // ============ Assignment Tests ============
    class RZStringAssignmentTests : public ::testing::Test
    {
    };

    TEST_F(RZStringAssignmentTests, CopyAssignment)
    {
        RZString str1("Hello");
        RZString str2;
        str2 = str1;
        EXPECT_STREQ(str2.c_str(), "Hello");
        EXPECT_EQ(str2.length(), 5);
    }

    TEST_F(RZStringAssignmentTests, MoveAssignment)
    {
        RZString str1("Hello");
        RZString str2;
        str2 = std::move(str1);
        EXPECT_STREQ(str2.c_str(), "Hello");
        EXPECT_TRUE(str1.empty());
    }

    TEST_F(RZStringAssignmentTests, CStringAssignment)
    {
        RZString str;
        str = "Hello World";
        EXPECT_STREQ(str.c_str(), "Hello World");
        EXPECT_EQ(str.length(), 11);
    }

    TEST_F(RZStringAssignmentTests, CharAssignment)
    {
        RZString str;
        str = 'A';
        EXPECT_STREQ(str.c_str(), "A");
        EXPECT_EQ(str.length(), 1);
    }

    // ============ Element Access Tests ============
    class RZStringElementAccessTests : public ::testing::Test
    {
    protected:
        RZString str{"Hello"};
    };

    TEST_F(RZStringElementAccessTests, OperatorBracket)
    {
        EXPECT_EQ(str[0], 'H');
        EXPECT_EQ(str[4], 'o');
    }

    TEST_F(RZStringElementAccessTests, At)
    {
        EXPECT_EQ(str.at(0), 'H');
        EXPECT_EQ(str.at(4), 'o');
    }

    TEST_F(RZStringElementAccessTests, Front)
    {
        EXPECT_EQ(str.front(), 'H');
    }

    TEST_F(RZStringElementAccessTests, Back)
    {
        EXPECT_EQ(str.back(), 'o');
    }

    TEST_F(RZStringElementAccessTests, Data)
    {
        EXPECT_STREQ(str.data(), "Hello");
    }

    TEST_F(RZStringElementAccessTests, CStr)
    {
        EXPECT_STREQ(str.c_str(), "Hello");
    }

    // ============ Capacity Tests ============
    class RZStringCapacityTests : public ::testing::Test
    {
    };

    TEST_F(RZStringCapacityTests, Empty)
    {
        RZString str1;
        EXPECT_TRUE(str1.empty());

        RZString str2("Hello");
        EXPECT_FALSE(str2.empty());
    }

    TEST_F(RZStringCapacityTests, Size)
    {
        RZString str("Hello");
        EXPECT_EQ(str.size(), 5);
    }

    TEST_F(RZStringCapacityTests, Length)
    {
        RZString str("Hello");
        EXPECT_EQ(str.length(), 5);
    }

    TEST_F(RZStringCapacityTests, Capacity)
    {
        RZString str("Hi");
        EXPECT_GE(str.capacity(), str.length());
    }

    TEST_F(RZStringCapacityTests, Reserve)
    {
        RZString str("Hello");
        sz       old_capacity = str.capacity();
        str.reserve(100);
        EXPECT_GE(str.capacity(), 100);
    }

    TEST_F(RZStringCapacityTests, Clear)
    {
        RZString str("Hello");
        str.clear();
        EXPECT_TRUE(str.empty());
        EXPECT_EQ(str.length(), 0);
    }

    class RZStringResizeReserveTests : public ::testing::Test
    {
    protected:
        // Helper to verify null termination
        void VerifyNullTermination(const RZString& str)
        {
            const char* data = str.c_str();
            ASSERT_NE(data, nullptr);
            EXPECT_EQ(data[str.length()], '\0');
        }
    };

    // ============ Resize/Reserve Tests ============
    TEST_F(RZStringResizeReserveTests, ResizeEmptyStringAndFill)
    {
        RZString str;
        str.resize(10);

        EXPECT_EQ(str.length(), 10);
        EXPECT_GE(str.capacity(), 10);
        VerifyNullTermination(str);
    }

    TEST_F(RZStringResizeReserveTests, ResizeAndCopyWithMemcpy)
    {
        RZString str;
        str.resize(15);

        const char* test_data = "hello";
        memcpy((char*) str.data(), test_data, 5);
        str.setLength(5);

        EXPECT_EQ(str.length(), 5);
        EXPECT_STREQ(str.c_str(), "hello");
        VerifyNullTermination(str);
    }

    TEST_F(RZStringResizeReserveTests, ResizeDecreaseAndVerify)
    {
        RZString str("hello world");
        str.resize(5);

        EXPECT_EQ(str.length(), 5);
        EXPECT_STREQ(str.c_str(), "hello");
        VerifyNullTermination(str);
    }

    TEST_F(RZStringResizeReserveTests, ResizeToZero)
    {
        RZString str("test");
        str.resize(0);

        EXPECT_EQ(str.length(), 0);
        EXPECT_EQ(str.c_str()[0], '\0');
        VerifyNullTermination(str);
    }

    TEST_F(RZStringResizeReserveTests, ResizePastSSOBoundary)
    {
        RZString str;
        sz       large_size = RAZIX_SSO_STRING_SIZE + 20;

        str.resize(large_size);

        EXPECT_EQ(str.length(), large_size);
        EXPECT_GE(str.capacity(), large_size);
        VerifyNullTermination(str);
    }

    TEST_F(RZStringResizeReserveTests, ReserveAndResizeWithMemcpy)
    {
        RZString str;
        str.reserve(50);
        str.resize(20);

        const char* data = "copy_test_data";
        memcpy((char*) str.data(), data, 14);
        str.setLength(14);

        EXPECT_EQ(str.length(), 14);
        EXPECT_STREQ(str.c_str(), "copy_test_data");
        VerifyNullTermination(str);
    }

    TEST_F(RZStringResizeReserveTests, ReserveHeapAndResizeWithSnprintf)
    {
        RZString str;
        str.reserve(RAZIX_SSO_STRING_SIZE + 30);
        str.resize(RAZIX_SSO_STRING_SIZE + 20);

        int written = snprintf((char*) str.data(), str.capacity() + 1, "%d-%s", 42, "test");
        str.setLength(written);

        EXPECT_GT(written, 0);
        EXPECT_STREQ(str.c_str(), "42-test");
        VerifyNullTermination(str);
    }

    // ============ Append Tests ============
    class RZStringAppendTests : public ::testing::Test
    {
    };

    TEST_F(RZStringAppendTests, AppendRZString)
    {
        RZString str1("Hello");
        RZString str2(" World");
        str1.append(str2);
        EXPECT_STREQ(str1.c_str(), "Hello World");
    }

    TEST_F(RZStringAppendTests, AppendCString)
    {
        RZString str("Hello");
        str.append(" World");
        EXPECT_STREQ(str.c_str(), "Hello World");
    }

    TEST_F(RZStringAppendTests, AppendCStringWithCount)
    {
        RZString str("Hello");
        str.append(" World", 6);
        EXPECT_STREQ(str.c_str(), "Hello World");
    }

    TEST_F(RZStringAppendTests, AppendCharCount)
    {
        RZString str("Hello");
        str.append(3, '!');
        EXPECT_STREQ(str.c_str(), "Hello!!!");
    }

    TEST_F(RZStringAppendTests, AppendOperator)
    {
        RZString str("Hello");
        str += " World";
        EXPECT_STREQ(str.c_str(), "Hello World");
    }

    TEST_F(RZStringAppendTests, AppendCharOperator)
    {
        RZString str("Hello");
        str += '!';
        EXPECT_STREQ(str.c_str(), "Hello!");
    }

    // ============ Insert Tests ============
    class RZStringInsertTests : public ::testing::Test
    {
    };

    TEST_F(RZStringInsertTests, InsertRZString)
    {
        RZString str("Hello World");
        RZString insert("Beautiful ");
        str.insert(6, insert);
        EXPECT_STREQ(str.c_str(), "Hello Beautiful World");
    }

    TEST_F(RZStringInsertTests, InsertCString)
    {
        RZString str("Hello World");
        str.insert(6, "Beautiful ");
        EXPECT_STREQ(str.c_str(), "Hello Beautiful World");
    }

    TEST_F(RZStringInsertTests, InsertCharCount)
    {
        RZString str("Hello World");
        str.insert(5, 3, '!');
        EXPECT_STREQ(str.c_str(), "Hello!!! World");
    }

    TEST_F(RZStringInsertTests, InsertCStringWithCount)
    {
        RZString str("Hello World");
        str.insert(5, "Beautiful ", 5);
        EXPECT_STREQ(str.c_str(), "HelloBeaut World");
    }

    // ============ Erase Tests ============
    class RZStringEraseTests : public ::testing::Test
    {
    };

    TEST_F(RZStringEraseTests, EraseDefault)
    {
        RZString str("Hello World");
        str.erase();
        EXPECT_TRUE(str.empty());
    }

    TEST_F(RZStringEraseTests, EraseWithPos)
    {
        RZString str("Hello World");
        str.erase(5);
        EXPECT_STREQ(str.c_str(), "Hello");
    }

    TEST_F(RZStringEraseTests, EraseWithPosAndCount)
    {
        RZString str("Hello World");
        str.erase(5, 1);
        EXPECT_STREQ(str.c_str(), "HelloWorld");
    }

    // ============ Replace Tests ============
    class RZStringReplaceTests : public ::testing::Test
    {
    };

    TEST_F(RZStringReplaceTests, ReplaceWithRZString)
    {
        RZString str("Hello World");
        RZString replacement("Beautiful");
        str.replace(6, 5, replacement);
        EXPECT_STREQ(str.c_str(), "Hello Beaut");
    }

    TEST_F(RZStringReplaceTests, ReplaceWithCString)
    {
        RZString str("Hello World");
        str.replace(6, 5, "Beautiful");
        EXPECT_STREQ(str.c_str(), "Hello Beaut");
    }

    TEST_F(RZStringReplaceTests, ReplaceWithChar)
    {
        RZString str("Hello World");
        str.replace(5, 6, '*');
        EXPECT_STREQ(str.c_str(), "Hello******");
    }

    // ============ Push/Pop Tests ============
    class RZStringPushPopTests : public ::testing::Test
    {
    };

    TEST_F(RZStringPushPopTests, PushBack)
    {
        RZString str("Hello");
        str.push_back('!');
        EXPECT_STREQ(str.c_str(), "Hello!");
    }

    TEST_F(RZStringPushPopTests, PopBack)
    {
        RZString str("Hello!");
        str.pop_back();
        EXPECT_STREQ(str.c_str(), "Hello");
    }

    // ============ Find Tests ============
    class RZStringFindTests : public ::testing::Test
    {
    protected:
        RZString str{"Hello World"};
    };

    TEST_F(RZStringFindTests, FindCString)
    {
        sz pos = str.find("World");
        EXPECT_EQ(pos, 6);
    }

    TEST_F(RZStringFindTests, FindCStringNotFound)
    {
        sz pos = str.find("xyz");
        EXPECT_EQ(pos, RZString::npos);
    }

    TEST_F(RZStringFindTests, FindChar)
    {
        sz pos = str.find('o');
        EXPECT_EQ(pos, 4);
    }

    TEST_F(RZStringFindTests, FindCharNotFound)
    {
        sz pos = str.find('z');
        EXPECT_EQ(pos, RZString::npos);
    }

    TEST_F(RZStringFindTests, FindWithPos)
    {
        sz pos = str.find("o", 5);
        EXPECT_EQ(pos, 7);
    }

    TEST_F(RZStringFindTests, FindRZString)
    {
        RZString search("World");
        sz       pos = str.find(search);
        EXPECT_EQ(pos, 6);
    }

    // ============ RFind Tests ============
    class RZStringRFindTests : public ::testing::Test
    {
    protected:
        RZString str{"Hello World Hello"};
    };

    TEST_F(RZStringRFindTests, RFindCString)
    {
        sz pos = str.rfind("Hello");
        EXPECT_EQ(pos, 12);
    }

    TEST_F(RZStringRFindTests, RFindChar)
    {
        sz pos = str.rfind('o');
        EXPECT_EQ(pos, 16);
    }

    TEST_F(RZStringRFindTests, RFindNotFound)
    {
        sz pos = str.rfind("xyz");
        EXPECT_EQ(pos, RZString::npos);
    }

    // ============ Find First Of Tests ============
    class RZStringFindFirstOfTests : public ::testing::Test
    {
    protected:
        RZString str{"Hello World"};
    };

    TEST_F(RZStringFindFirstOfTests, FindFirstOfCString)
    {
        sz pos = str.find_first_of("aeiou");
        EXPECT_EQ(pos, 1);    // 'e' at position 1
    }

    TEST_F(RZStringFindFirstOfTests, FindFirstOfChar)
    {
        sz pos = str.find_first_of('o');
        EXPECT_EQ(pos, 4);
    }

    TEST_F(RZStringFindFirstOfTests, FindFirstOfNotFound)
    {
        sz pos = str.find_first_of("xyz");
        EXPECT_EQ(pos, RZString::npos);
    }

    // ============ Find Last Of Tests ============
    class RZStringFindLastOfTests : public ::testing::Test
    {
    protected:
        RZString str{"Hello World"};
    };

    TEST_F(RZStringFindLastOfTests, FindLastOfCString)
    {
        sz pos = str.find_last_of("aeiou");
        EXPECT_EQ(pos, 7);    // 'o' at position 7
    }

    TEST_F(RZStringFindLastOfTests, FindLastOfChar)
    {
        sz pos = str.find_last_of('o');
        EXPECT_EQ(pos, 7);
    }

    // ============ Find First Not Of Tests ============
    class RZStringFindFirstNotOfTests : public ::testing::Test
    {
    protected:
        RZString str{"Hello World"};
    };

    TEST_F(RZStringFindFirstNotOfTests, FindFirstNotOfCString)
    {
        sz pos = str.find_first_not_of("He");
        EXPECT_EQ(pos, 2);    // 'l' at position 2
    }

    TEST_F(RZStringFindFirstNotOfTests, FindFirstNotOfChar)
    {
        sz pos = str.find_first_not_of('H');
        EXPECT_EQ(pos, 1);
    }

    // ============ Find Last Not Of Tests ============
    class RZStringFindLastNotOfTests : public ::testing::Test
    {
    protected:
        RZString str{"Hello World"};
    };

    TEST_F(RZStringFindLastNotOfTests, FindLastNotOfCString)
    {
        sz pos = str.find_last_not_of("d");
        EXPECT_EQ(pos, 10);
    }

    TEST_F(RZStringFindLastNotOfTests, FindLastNotOfChar)
    {
        sz pos = str.find_last_not_of('d');
        EXPECT_EQ(pos, 10);
    }

    // ============ Substr Tests ============
    class RZStringSubstrTests : public ::testing::Test
    {
    protected:
        RZString str{"Hello World"};
    };

    TEST_F(RZStringSubstrTests, SubstrDefault)
    {
        RZString sub = str.substr();
        EXPECT_STREQ(sub.c_str(), "Hello World");
    }

    TEST_F(RZStringSubstrTests, SubstrWithPos)
    {
        RZString sub = str.substr(6);
        EXPECT_STREQ(sub.c_str(), "World");
    }

    TEST_F(RZStringSubstrTests, SubstrWithPosAndCount)
    {
        RZString sub = str.substr(0, 5);
        EXPECT_STREQ(sub.c_str(), "Hello");
    }

    // ============ Compare Tests ============
    class RZStringCompareTests : public ::testing::Test
    {
    };

    TEST_F(RZStringCompareTests, CompareEqual)
    {
        RZString str1("Hello");
        RZString str2("Hello");
        EXPECT_EQ(str1.compare(str2), 0);
    }

    TEST_F(RZStringCompareTests, CompareLess)
    {
        RZString str1("Apple");
        RZString str2("Banana");
        EXPECT_LT(str1.compare(str2), 0);
    }

    TEST_F(RZStringCompareTests, CompareGreater)
    {
        RZString str1("Banana");
        RZString str2("Apple");
        EXPECT_GT(str1.compare(str2), 0);
    }

    TEST_F(RZStringCompareTests, CompareCString)
    {
        RZString str("Hello");
        EXPECT_EQ(str.compare("Hello"), 0);
        EXPECT_LT(str.compare("World"), 0);
    }

    TEST_F(RZStringCompareTests, CompareWithPos)
    {
        RZString str1("Hello World");
        RZString str2("World");
        EXPECT_EQ(str1.compare(6, 5, str2), 0);
    }

    // ============ Equality Operator Tests ============
    class RZStringEqualityTests : public ::testing::Test
    {
    };

    TEST_F(RZStringEqualityTests, OperatorEqualRZString)
    {
        RZString str1("Hello");
        RZString str2("Hello");
        EXPECT_TRUE(str1 == str2);
    }

    TEST_F(RZStringEqualityTests, OperatorEqualCString)
    {
        RZString str("Hello");
        EXPECT_TRUE(str == "Hello");
    }

    TEST_F(RZStringEqualityTests, OperatorNotEqualRZString)
    {
        RZString str1("Hello");
        RZString str2("World");
        EXPECT_TRUE(str1 != str2);
    }

    TEST_F(RZStringEqualityTests, OperatorNotEqualCString)
    {
        RZString str("Hello");
        EXPECT_TRUE(str != "World");
    }

    // ============ Relational Operator Tests ============
    class RZStringRelationalTests : public ::testing::Test
    {
    };

    TEST_F(RZStringRelationalTests, OperatorLess)
    {
        RZString str1("Apple");
        RZString str2("Banana");
        EXPECT_TRUE(str1 < str2);
    }

    TEST_F(RZStringRelationalTests, OperatorLessEqual)
    {
        RZString str1("Apple");
        RZString str2("Apple");
        EXPECT_TRUE(str1 <= str2);
    }

    TEST_F(RZStringRelationalTests, OperatorGreater)
    {
        RZString str1("Banana");
        RZString str2("Apple");
        EXPECT_TRUE(str1 > str2);
    }

    TEST_F(RZStringRelationalTests, OperatorGreaterEqual)
    {
        RZString str1("Banana");
        RZString str2("Banana");
        EXPECT_TRUE(str1 >= str2);
    }

    // ============ Addition Operator Tests ============
    class RZStringAdditionTests : public ::testing::Test
    {
    };

    TEST_F(RZStringAdditionTests, AddRZStringRZString)
    {
        RZString str1("Hello");
        RZString str2(" World");
        RZString result = str1 + str2;
        EXPECT_STREQ(result.c_str(), "Hello World");
    }

    TEST_F(RZStringAdditionTests, AddRZStringCString)
    {
        RZString str("Hello");
        RZString result = str + " World";
        EXPECT_STREQ(result.c_str(), "Hello World");
    }

    TEST_F(RZStringAdditionTests, AddCStringRZString)
    {
        RZString str(" World");
        RZString result = "Hello" + str;
        EXPECT_STREQ(result.c_str(), "Hello World");
    }

    TEST_F(RZStringAdditionTests, AddRZStringChar)
    {
        RZString str("Hello");
        RZString result = str + '!';
        EXPECT_STREQ(result.c_str(), "Hello!");
    }

    TEST_F(RZStringAdditionTests, AddCharRZString)
    {
        RZString str("Hello");
        RZString result = '*' + str;
        EXPECT_STREQ(result.c_str(), "*Hello");
    }

    TEST_F(RZStringAdditionTests, AdditionOperatorDoesNotModifyOriginal)
    {
        RZString str1("Hello");
        RZString str2(" World");
        RZString result = str1 + str2;
        EXPECT_STREQ(str1.c_str(), "Hello");
        EXPECT_STREQ(str2.c_str(), " World");
    }

    // ============ Swap Tests ============
    class RZStringSwapTests : public ::testing::Test
    {
    };

    TEST_F(RZStringSwapTests, Swap)
    {
        RZString str1("Hello");
        RZString str2("World");
        str1.swap(str2);
        EXPECT_STREQ(str1.c_str(), "World");
        EXPECT_STREQ(str2.c_str(), "Hello");
    }

    // ============ SSO Tests (Small String Optimization) ============
    class RZStringSSoTests : public ::testing::Test
    {
    public:
        void SetUp() override
        {
            // Setup before each test case, for example initializing the logging system
            Razix::Debug::RZLog::StartUp();
        }

        void TearDown() override
        {
            // Cleanup after each test case
            Razix::Debug::RZLog::Shutdown();
        }
    };

    TEST_F(RZStringSSoTests, SmallStringShouldUseSso)
    {
        RZString str("Hi");
        EXPECT_EQ(str.length(), 2);
        EXPECT_STREQ(str.c_str(), "Hi");
    }

    TEST_F(RZStringSSoTests, LargeStringShouldUseHeap)
    {
        // Create a string larger than SSO buffer
        std::string large(100, 'a');
        RZString    str(large.c_str());
        EXPECT_EQ(str.length(), 100);
    }

    // ============ Edge Case Tests ============
    class RZStringEdgeCaseTests : public ::testing::Test
    {
    };

    TEST_F(RZStringEdgeCaseTests, EmptyStringOperations)
    {
        RZString str;
        str.append("Hello");
        EXPECT_STREQ(str.c_str(), "Hello");
    }

    TEST_F(RZStringEdgeCaseTests, MultipleAppends)
    {
        RZString str("Hello");
        str.append(" ");
        str.append("World");
        str.append("!");
        EXPECT_STREQ(str.c_str(), "Hello World!");
    }

    TEST_F(RZStringEdgeCaseTests, RepeatedOperations)
    {
        RZString str("a");
        for (int i = 0; i < 10; i++) {
            str += "b";
        }
        EXPECT_EQ(str.length(), 11);
    }

    TEST_F(RZStringEdgeCaseTests, LongStringOperations)
    {
        RZString str;
        for (int i = 0; i < 1000; i++) {
            str += "x";
        }
        EXPECT_EQ(str.length(), 1000);
    }

    TEST_F(RZStringEdgeCaseTests, CopyAndModify)
    {
        RZString str1("Hello");
        RZString str2 = str1;
        str2.append(" World");
        EXPECT_STREQ(str1.c_str(), "Hello");
        EXPECT_STREQ(str2.c_str(), "Hello World");
    }

    TEST_F(RZStringEdgeCaseTests, MoveAndAccess)
    {
        RZString str1("Hello");
        RZString str2 = std::move(str1);
        EXPECT_STREQ(str2.c_str(), "Hello");
        EXPECT_TRUE(str1.empty());
    }

    TEST_F(RZStringEdgeCaseTests, AssignmentChaining)
    {
        RZString str1, str2, str3;
        str1 = str2 = str3 = "Hello";
        EXPECT_STREQ(str1.c_str(), "Hello");
        EXPECT_STREQ(str2.c_str(), "Hello");
        EXPECT_STREQ(str3.c_str(), "Hello");
    }

    // ============ Heap String Concatenation Tests ============
    class RZStringHeapConcatenationTests : public ::testing::Test
    {
    public:
        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();
        }

        void TearDown() override
        {
            Razix::Debug::RZLog::Shutdown();
        }
    };

    // Test concatenation of a heap-allocated string (>64 bytes) using operator+
    TEST_F(RZStringHeapConcatenationTests, ConcatenateHeapStringWithOperatorPlus)
    {
        // Create a string larger than SSO (64 bytes)
        RZString large_str(70, 'a');
        EXPECT_EQ(large_str.length(), 70);
        EXPECT_GT(large_str.length(), RAZIX_SSO_STRING_SIZE);

        // Concatenate using operator+
        RZString result = large_str + "test";

        // Verify the result
        EXPECT_EQ(result.length(), 74);
        EXPECT_STREQ(result.c_str(), std::string(70, 'a').append("test").c_str());
    }

    // Test concatenation with RZString object
    TEST_F(RZStringHeapConcatenationTests, ConcatenateHeapStringWithRZString)
    {
        RZString large_str(80, 'b');
        RZString suffix("_suffix");

        RZString result = large_str + suffix;

        EXPECT_EQ(result.length(), 87);
        EXPECT_STREQ(result.c_str(), std::string(80, 'b').append("_suffix").c_str());
    }

    // Test multiple concatenations on heap string
    TEST_F(RZStringHeapConcatenationTests, MultipleConcatenationsOnHeapString)
    {
        RZString base(75, 'x');

        RZString result = base + "part1" + "part2" + "part3";

        EXPECT_EQ(result.length(), 75 + 5 + 5 + 5);
        std::string expected = std::string(75, 'x') + "part1part2part3";
        EXPECT_STREQ(result.c_str(), expected.c_str());
    }

    // Test += operator on heap string
    TEST_F(RZStringHeapConcatenationTests, AppendOperatorOnHeapString)
    {
        RZString heap_str(100, 'c');
        heap_str += "_appended";

        EXPECT_EQ(heap_str.length(), 109);
        std::string expected = std::string(100, 'c') + "_appended";
        EXPECT_STREQ(heap_str.c_str(), expected.c_str());
    }

    // Test append method on heap string
    TEST_F(RZStringHeapConcatenationTests, AppendMethodOnHeapString)
    {
        RZString heap_str(85, 'd');
        heap_str.append("_suffix", 7);

        EXPECT_EQ(heap_str.length(), 92);
        std::string expected = std::string(85, 'd') + "_suffix";
        EXPECT_STREQ(heap_str.c_str(), expected.c_str());
    }

    // Test null termination after append on heap string
    TEST_F(RZStringHeapConcatenationTests, NullTerminationAfterHeapAppend)
    {
        RZString heap_str(70, 'e');
        heap_str.append("test");

        const char* data = heap_str.c_str();
        EXPECT_EQ(data[heap_str.length()], '\0');
        EXPECT_EQ(data[74], '\0');    // 70 + 4 = 74
    }

    // Test char* + heap string using operator+
    TEST_F(RZStringHeapConcatenationTests, CharPtrPlusHeapString)
    {
        RZString heap_str(65, 'f');
        RZString result = "prefix_" + heap_str;

        EXPECT_EQ(result.length(), 72);    // 7 + 65
        std::string expected = std::string("prefix_") + std::string(65, 'f');
        EXPECT_STREQ(result.c_str(), expected.c_str());
    }

    // Test heap string + char
    TEST_F(RZStringHeapConcatenationTests, HeapStringPlusChar)
    {
        RZString heap_str(60, 'g');
        RZString result = heap_str + '!';

        EXPECT_EQ(result.length(), 61);
        std::string expected = std::string(60, 'g') + "!";

        EXPECT_STREQ(result.c_str(), expected.c_str());
    }

    // Test char + heap string
    TEST_F(RZStringHeapConcatenationTests, CharPlusHeapString)
    {
        RZString heap_str(70, 'h');
        RZString result = '@' + heap_str;

        EXPECT_EQ(result.length(), 71);
        std::string expected = "@" + std::string(70, 'h');
        EXPECT_STREQ(result.c_str(), expected.c_str());
    }

    // Test concatenation doesn't corrupt original
    TEST_F(RZStringHeapConcatenationTests, ConcatenationPreservesOriginal)
    {
        RZString original(75, 'i');
        RZString original_copy = original;

        RZString result = original + "extra";

        EXPECT_STREQ(original.c_str(), original_copy.c_str());
        EXPECT_EQ(original.length(), 75);
        EXPECT_EQ(result.length(), 80);
    }

    // Test large heap concatenations
    TEST_F(RZStringHeapConcatenationTests, LargeHeapConcatenation)
    {
        RZString large1(200, 'j');
        RZString large2(150, 'k');

        RZString result = large1 + large2;

        EXPECT_EQ(result.length(), 350);
        EXPECT_STREQ(result.substr(0, 200).c_str(), std::string(200, 'j').c_str());
        EXPECT_STREQ(result.substr(200).c_str(), std::string(150, 'k').c_str());
    }

    TEST_F(RZStringHeapConcatenationTests, SSOToHeapSwitchContatenation)
    {
        RZString small = "hi";    // 2 chars in SSO
        EXPECT_EQ(small.capacity(), RAZIX_SSO_STRING_SIZE);

        // First + operation that stays in SSO
        RZString temp1 = small + " there";    // "hi there" = 8 chars, still SSO
        EXPECT_EQ(temp1.capacity(), RAZIX_SSO_STRING_SIZE);
        EXPECT_STREQ(temp1.c_str(), "hi there");

        // Second + operation that crosses to heap
        RZString result = temp1 + " this is a much longer string to force heap allocation from SSO";
        EXPECT_GT(result.capacity(), RAZIX_SSO_STRING_SIZE);
        EXPECT_EQ(result.capacity(), RAZIX_SSO_STRING_SIZE * 2);

        const char* expected = "hi there this is a much longer string to force heap allocation from SSO";
        EXPECT_STREQ(result.c_str(), expected);

        RZString    temp2         = result + " and we also add something more to SSO to heap switched string";
        const char* expected_long = "hi there this is a much longer string to force heap allocation from SSO and we also add something more to SSO to heap switched string";
        EXPECT_STREQ(temp2.c_str(), expected_long);
    }

    class RZStringSignatureProgressiveTest : public ::testing::Test
    {
    protected:
        void VerifyNullTermination(const RZString& str, const char* label = "")
        {
            const char* data = str.c_str();
            ASSERT_NE(data, nullptr) << label << " - null pointer";
            EXPECT_EQ(data[str.length()], '\0')
                << label << " - Null terminator missing at position " << str.length()
                << ", string: '" << data << "'";
        }
    };

    class RZStringHeapAppendTest : public ::testing::Test
    {
    protected:
        void VerifyNullTermination(const RZString& str, const char* label = "")
        {
            const char* data = str.c_str();
            ASSERT_NE(data, nullptr) << label << " - null pointer";
            EXPECT_EQ(data[str.length()], '\0')
                << label << " - Null terminator missing at position " << str.length()
                << ", string: '" << data << "'";
        }
    };

    //-------------------------------------------------------------------------

    TEST_F(RZStringHeapAppendTest, Append_To_Heap_String)
    {
        RZString str = "Start";
        str.reserve(100);    // Ensure heap

        str.append("_End");

        EXPECT_GE(str.capacity(), str.length());
        VerifyNullTermination(str, "After append");
        EXPECT_STREQ(str.c_str(), "Start_End");
    }

    TEST_F(RZStringHeapAppendTest, Operator_Plus_Heap_String)
    {
        RZString str1 = "Hello";
        str1.reserve(100);    // Force heap

        RZString result = str1 + " World";

        EXPECT_EQ(result.length(), 11);
        EXPECT_GE(result.capacity(), result.length());
        VerifyNullTermination(result, "After operator+");
        EXPECT_STREQ(result.c_str(), "Hello World");
    }

    TEST_F(RZStringHeapAppendTest, Multiple_Operator_Plus_Heap)
    {
        RZString str1 = "A";
        str1.reserve(100);    // Force heap

        RZString str2 = str1 + "B";
        EXPECT_STREQ(str2.c_str(), "AB");
        VerifyNullTermination(str2, "After first +");

        RZString str3 = str2 + "C";
        EXPECT_STREQ(str3.c_str(), "ABC");
        VerifyNullTermination(str3, "After second +");

        RZString str4 = str3 + "D";
        EXPECT_STREQ(str4.c_str(), "ABCD");
        VerifyNullTermination(str4, "After third +");
    }

    TEST_F(RZStringHeapAppendTest, Cross_SSO_Boundary_Then_Add)
    {
        // Build a string that crosses SSO boundary
        RZString str = "1234567890123456789012345678901234567890123456789012345678901234567890";    // 71 chars, crosses SSO

        EXPECT_GT(str.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(str, "Crossed SSO");

        // Now add to it
        str = str + "_MORE";

        EXPECT_GT(str.length(), 71);
        VerifyNullTermination(str, "After adding more");
    }

    //-------------------------------------------------------------------------

    // Progressive building - step by step like real signature
    TEST_F(RZStringSignatureProgressiveTest, Step_1_ProjectName_Only)
    {
        RZString projectName = "RazixGfxTestApp";    // 15 chars
        RZString result      = projectName;

        EXPECT_EQ(result.length(), 15);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 1");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_2_Add_Separator)
    {
        RZString projectName = "RazixGfxTestApp";      // 15 chars
        RZString result      = projectName + " | ";    // +3 = 18 chars

        EXPECT_EQ(result.length(), 18);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 2");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | ");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_3_Add_Engine_Name)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString result      = projectName + " | " + "Razix Engine";    // +12 = 30 chars (STILL SSO!)

        EXPECT_EQ(result.length(), 30);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);    // Still in SSO
        VerifyNullTermination(result, "Step 3");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_4_Add_Version_Separator)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString result      = projectName + " | " + "Razix Engine" + " - ";    // +3 = 33 chars

        EXPECT_EQ(result.length(), 33);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 4");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - ");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_5_Add_Version_Number)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";                                                  // 6 chars
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version;    // +6 = 39 chars

        EXPECT_EQ(result.length(), 39);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 5");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_6_Add_Space_Before_Stage)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " ";    // +1 = 40 chars

        EXPECT_EQ(result.length(), 40);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 6");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 ");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_7_Add_Stage_Bracket)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[";    // +1 = 41 chars

        EXPECT_EQ(result.length(), 41);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 7");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_8_Add_Stage_Name)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";                                                                 // 11 chars
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[" + stage;    // +11 = 52 chars

        EXPECT_EQ(result.length(), 52);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 8");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [Development");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_9_Add_Stage_Close_Bracket)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[" + stage + "]";    // +1 = 53 chars

        EXPECT_EQ(result.length(), 53);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 9");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [Development]");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_10_Add_Space_Before_API)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[" + stage + "]" + " ";    // +1 = 54 chars

        EXPECT_EQ(result.length(), 54);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 10");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [Development] ");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_11_Add_API_Open_Bracket)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[" + stage + "]" + " " + "<";    // +1 = 55 chars

        EXPECT_EQ(result.length(), 55);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 11");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [Development] <");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_12_Add_API_Name)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";
        RZString api         = "Vulkan";                                                                                              // 6 chars
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[" + stage + "]" + " " + "<" + api;    // +6 = 61 chars

        EXPECT_EQ(result.length(), 61);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 12");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [Development] <Vulkan");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_13_Add_API_Close_Bracket)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";
        RZString api         = "Vulkan";
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[" + stage + "]" + " " + "<" + api + ">";    // +1 = 62 chars

        EXPECT_EQ(result.length(), 62);
        EXPECT_LT(result.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(result, "Step 13");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [Development] <Vulkan>");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_14_Add_Final_Separator)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";
        RZString api         = "Vulkan";
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[" + stage + "]" + " " + "<" + api + ">" + " | ";    // +3 = 65 chars

        EXPECT_EQ(result.length(), 65);
        EXPECT_GT(result.length(), RAZIX_SSO_STRING_SIZE);    // CROSSES SSO HERE!
        VerifyNullTermination(result, "Step 14 - HEAP TRANSITION");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [Development] <Vulkan> | ");
    }

    TEST_F(RZStringSignatureProgressiveTest, Step_15_Add_Build_Config)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";
        RZString api         = "Vulkan";
        RZString config      = "Debug-x86_64-windows";                                                                                                             // 5 chars
        RZString result      = projectName + " | " + "Razix Engine" + " - " + version + " " + "[" + stage + "]" + " " + "<" + api + ">" + " | " + config;    // +21 = 91 chars

        EXPECT_EQ(result.length(), 85);
        EXPECT_GT(result.length(), RAZIX_SSO_STRING_SIZE);
        EXPECT_GE(result.capacity(), result.length());
        VerifyNullTermination(result, "Step 16 - FINAL");
        EXPECT_STREQ(result.c_str(), "RazixGfxTestApp | Razix Engine - 0.50.0 [Development] <Vulkan> | Debug-x86_64-windows");
    }

    TEST_F(RZStringSignatureProgressiveTest, Full_Signature_All_At_Once)
    {
        RZString projectName = "RazixGfxTestApp";
        RZString version     = "0.50.0";
        RZString stage       = "Development";
        RZString api         = "Vulkan";
        RZString config      = "Debug-x86_64-windows";

        // Exact same as your code
        RZString SignatureTitle = projectName + " | " + "Razix Engine" + " - " + version + " " +
                                  "[" + stage + "]" + " " + "<" + api + ">" + " | " + config;

        EXPECT_EQ(SignatureTitle.length(), 85);
        EXPECT_GT(SignatureTitle.length(), RAZIX_SSO_STRING_SIZE);
        EXPECT_GE(SignatureTitle.capacity(), SignatureTitle.length());
        VerifyNullTermination(SignatureTitle, "FULL SIGNATURE");

        const char* expected = "RazixGfxTestApp | Razix Engine - 0.50.0 [Development] <Vulkan> | Debug-x86_64-windows";
        EXPECT_STREQ(SignatureTitle.c_str(), expected);
    }

    // Edge case: exactly at SSO boundary
    TEST_F(RZStringSignatureProgressiveTest, Exactly_At_SSO_Boundary)
    {
        RZString str = "1234567890123456789012345678901234567890123456789012345678901234";    // Exactly 64 chars

        EXPECT_EQ(str.length(), 64);
        EXPECT_LE(str.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(str, "At SSO boundary");
    }

    // Edge case: one char over SSO boundary
    TEST_F(RZStringSignatureProgressiveTest, One_Over_SSO_Boundary)
    {
        RZString str = "12345678901234567890123456789012345678901234567890123456789012345";    // 65 chars

        EXPECT_EQ(str.length(), 65);
        EXPECT_GT(str.length(), RAZIX_SSO_STRING_SIZE);
        VerifyNullTermination(str, "One over SSO boundary");
    }
}    // namespace Razix
