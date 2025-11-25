// CommandLineParserTests.cpp
// AI-generated unit tests for the RZCommandLineParser class

#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include <Razix/Core/Log/RZLog.h>

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/Utils/RZiniParser.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace Utilities {

        class RZiniTests : public ::testing::Test
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

        class MockRZiniParser : public RZiniParser
        {
        public:
            void mockParse(const RZHashMap<RZString, Section>& sections)
            {
                m_Sections = sections;
            }
        };

        TEST_F(RZiniTests, ParseFile)
        {
            // Mock setup for testing parse
            MockRZiniParser parser;

            // Simulate parsed sections
            Section globalSection;
            globalSection.variables["FrameBudget"] = 16.67f;
            globalSection.variables["DebugMode"]   = true;

            Section coreSubSection;
            coreSubSection.variables["MaxThreads"] = 4;

            globalSection.subsections["Core"] = coreSubSection;

            parser.mockParse({{"Global", globalSection}});

            // Test fetching values
            float frameBudget;
            EXPECT_TRUE(parser.getValue("Global", "FrameBudget", frameBudget));
            EXPECT_FLOAT_EQ(frameBudget, 16.67f);

            bool debugMode;
            EXPECT_TRUE(parser.getValue("Global", "DebugMode", debugMode));
            EXPECT_TRUE(debugMode);

            int maxThreads;
            EXPECT_TRUE(parser.getValue("Global", "Core.MaxThreads", maxThreads));
            EXPECT_EQ(maxThreads, 4);
        }

        TEST_F(RZiniTests, MissingSectionOrKey)
        {
            // Mock setup for testing missing keys
            MockRZiniParser parser;

            // Simulate parsed sections
            Section globalSection;
            globalSection.variables["FrameBudget"] = 16.67f;

            parser.mockParse({{"Global", globalSection}});

            // Test missing section
            float frameBudget;
            EXPECT_FALSE(parser.getValue("NonExistent", "FrameBudget", frameBudget));

            // Test missing key
            bool debugMode;
            EXPECT_FALSE(parser.getValue("Global", "DebugMode", debugMode));
        }

        TEST_F(RZiniTests, IncorrectType)
        {
            // Mock setup for type mismatch
            MockRZiniParser parser;

            // Simulate parsed sections
            Section globalSection;
            globalSection.variables["FrameBudget"] = 16.67f;

            parser.mockParse({{"Global", globalSection}});

            // Test fetching with incorrect type
            int frameBudget;
            EXPECT_FALSE(parser.getValue("Global", "FrameBudget", frameBudget));
        }

        TEST_F(RZiniTests, SubsectionError)
        {
            // Mock setup for subsections
            MockRZiniParser parser;

            // Simulate parsed sections
            Section globalSection;
            globalSection.variables["FrameBudget"] = 16.67f;

            Section nestedSubSection;
            Section deeperSubSection;
            nestedSubSection.subsections["Deeper"] = deeperSubSection;

            globalSection.subsections["Core"] = nestedSubSection;

            parser.mockParse({{"Global", globalSection}});

            // Test unsupported subsection depth
            float someValue;
            EXPECT_FALSE(parser.getValue("Global", "Core.Deeper.SomeValue", someValue));
        }
    }    // namespace Utilities
}    // namespace Razix
