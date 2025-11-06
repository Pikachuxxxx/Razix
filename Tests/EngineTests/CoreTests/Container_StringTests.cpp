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
}    // namespace Razix
