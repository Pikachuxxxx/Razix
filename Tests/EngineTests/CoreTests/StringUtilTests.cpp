// CommandLineParserTests.cpp
// AI-generated unit tests for the RZCommandLineParser class

#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include <Razix/Core/Log/RZLog.h>

#include "Razix/Core/Containers/string_utils.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include <gtest/gtest.h>

namespace Razix {
    TEST(RZStringUtilities, to_string)
    {
        EXPECT_EQ(rz_to_string(42), "42");
        EXPECT_EQ(rz_to_string(3.14), "3.140000");
    }

    TEST(RZStringUtilities, GetFilePathExtension)
    {
        EXPECT_EQ(GetFilePathExtension("file.txt"), "txt");
        EXPECT_EQ(GetFilePathExtension("file.tar.gz"), "gz");
        EXPECT_EQ(GetFilePathExtension("file"), "");
    }

    TEST(RZStringUtilities, RemoveFilePathExtension)
    {
        EXPECT_EQ(RemoveFilePathExtension("file.txt"), "file");
        EXPECT_EQ(RemoveFilePathExtension("file.tar.gz"), "file.tar");
        EXPECT_EQ(RemoveFilePathExtension("file"), "file");
    }

    TEST(RZStringUtilities, GetFileName)
    {
        EXPECT_EQ(GetFileName("/path/to/file.txt"), "file.txt");
        EXPECT_EQ(GetFileName("file.txt"), "file.txt");
    }

    TEST(RZStringUtilities, RemoveName)
    {
        EXPECT_EQ(RemoveName("/path/to/file.txt"), "/path/to/");
        EXPECT_EQ(RemoveName("/file.txt"), "/");
    }

    TEST(RZStringUtilities, GetFileLocation)
    {
        EXPECT_EQ(GetFileLocation("/path/to/file.txt"), "/path/to/");
        EXPECT_EQ(GetFileLocation("/file.txt"), "/");
    }

    TEST(RZStringUtilities, IsHiddenFile)
    {
        EXPECT_TRUE(IsHiddenFile(".hidden"));
        EXPECT_FALSE(IsHiddenFile("visible"));
    }

    TEST(RZStringUtilities, SplitString)
    {
        auto result = SplitString("a,b,c", ',');
        EXPECT_EQ(result.size(), 3);
        EXPECT_EQ(result[0], "a");
        EXPECT_EQ(result[1], "b");
        EXPECT_EQ(result[2], "c");
    }

    TEST(RZStringUtilities, Tokenize)
    {
        auto result = Tokenize("a b c");
        EXPECT_EQ(result.size(), 3);
        EXPECT_EQ(result[0], "a");
        EXPECT_EQ(result[1], "b");
        EXPECT_EQ(result[2], "c");
    }

    TEST(RZStringUtilities, FindToken)
    {
        const char* str = "This is a test";
        EXPECT_NE(FindToken(str, "test"), "test");
        EXPECT_EQ(FindToken(str, "not_found"), nullptr);
    }

    TEST(RZStringUtilities, StringRange)
    {
        EXPECT_EQ(StringRange("Hello, World!", 0, 5), "Hello");
    }

    TEST(RZStringUtilities, RemoveStringRange)
    {
        EXPECT_EQ(RemoveStringRange("Hello, World!", 0, 7), "World!");
    }

    TEST(RZStringUtilities, TrimWhitespaces)
    {
        auto trim = TrimWhitespaces("   test   ");
        auto str  = RemoveSpaces(trim);
        EXPECT_EQ(str, "test");
    }

    TEST(RZStringUtilities, StringContains)
    {
        EXPECT_TRUE(StringContains("Hello, World!", "World"));
        EXPECT_FALSE(StringContains("Hello, World!", "Universe"));
    }

    TEST(RZStringUtilities, StartsWith)
    {
        EXPECT_TRUE(StartsWith("Hello, World!", "Hello"));
        EXPECT_FALSE(StartsWith("Hello, World!", "World"));
    }

    TEST(RZStringUtilities, StringReplace)
    {
        EXPECT_EQ(StringReplace("a_b_c", '_', '-'), "a-b-c");
    }

    TEST(RZStringUtilities, BackSlashesToSlashes)
    {
        RZString path = "path\\to\\file";
        BackSlashesToSlashes(path);
        EXPECT_EQ(path, "path/to/file");
    }

    TEST(RZStringUtilities, SlashesToBackSlashes)
    {
        RZString path = "path/to/file";
        SlashesToBackSlashes(path);
        EXPECT_EQ(path, "path\\to\\file");
    }

    TEST(RZStringUtilities, RemoveSpaces)
    {
        RZString str = "a b c";
        RemoveSpaces(str);
        EXPECT_EQ(str, "abc");
    }
}    // namespace Razix
