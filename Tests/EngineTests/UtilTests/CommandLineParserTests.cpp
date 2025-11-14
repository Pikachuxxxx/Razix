// CommandLineParserTests.cpp
// AI-generated unit tests for the RZCommandLineParser class

#include <Razix.h>

#include <gtest/gtest.h>

namespace Razix {

    class RZCommandLineParserTests : public ::testing::Test
    {
    protected:
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

    TEST_F(RZCommandLineParserTests, addCommandAndParse)
    {
        RZCommandLineParser parser;

        // Add a sample command
        parser.addCommand("test", {"--test", "-t"}, true, "A test command that requires a value");
        parser.addCommand("help", {"--help", "-h"}, false, "Shows the help message");

        // Simulate arguments
        Razix::RZDynamicArray<cstr> args = {"app", "--test", "value123", "--help"};
        parser.parse(args);

        // Check that commands were set correctly
        EXPECT_TRUE(parser.isSet("test"));
        EXPECT_EQ(parser.getValueAsString("test"), "value123");

        EXPECT_TRUE(parser.isSet("help"));
    }

    TEST_F(RZCommandLineParserTests, NoCommandSet)
    {
        RZCommandLineParser parser;

        // Add a sample command
        parser.addCommand("test", {"--test", "-t"}, true, "A test command that requires a value");

        // Simulate empty arguments
        Razix::RZDynamicArray<const char*> args = {"app"};
        parser.parse(args);

        // Check that commands were not set
        EXPECT_FALSE(parser.isSet("test"));
    }

    TEST_F(RZCommandLineParserTests, GetValueAsInt)
    {
        RZCommandLineParser parser;

        // Add a sample command
        parser.addCommand("NumCmd", {"--number", "-n"}, true, "A test command that requires a numeric value");

        // Simulate arguments
        Razix::RZDynamicArray<const char*> args = {"app", "--number", "42"};
        parser.parse(args);

        // Check that commands were set correctly and value is parsed as int
        EXPECT_TRUE(parser.isSet("NumCmd"));
        EXPECT_EQ(parser.getValueAsInt("NumCmd"), 42);
    }
}    // namespace Razix
