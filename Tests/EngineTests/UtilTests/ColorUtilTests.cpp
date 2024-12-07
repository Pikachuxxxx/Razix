// ColorUtilitiesTests.cpp
// AI-generated unit tests for the RZColorUtilities class
#include <Razix.h>
#include <Razix/Utilities/RZColorUtilities.h>

#include <gtest/gtest.h>

namespace Razix {
    class RZColorUtilitiesTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Setup code if needed (e.g., initialize subsystems)
        }

        void TearDown() override
        {
            // Cleanup code if needed
        }
    };

    // Test for GenerateHashedColor
    TEST_F(RZColorUtilitiesTests, GenerateHashedColor)
    {
        glm::vec3 color1 = Razix::Utilities::GenerateHashedColor(123);
        glm::vec3 color2 = Razix::Utilities::GenerateHashedColor(123);
        glm::vec3 color3 = Razix::Utilities::GenerateHashedColor(456);

        // Verify determinism
        EXPECT_EQ(color1, color2) << "GenerateHashedColor should return consistent results for the same seed.";

        // Verify different seeds produce different colors
        EXPECT_NE(color1, color3) << "GenerateHashedColor should produce different results for different seeds.";

        // Verify color components are within valid range
        EXPECT_GE(color1.r, 0.0f);
        EXPECT_LE(color1.r, 1.0f);
        EXPECT_GE(color1.g, 0.0f);
        EXPECT_LE(color1.g, 1.0f);
        EXPECT_GE(color1.b, 0.0f);
        EXPECT_LE(color1.b, 1.0f);
    }

    // Test for GenerateHashedColor4
    TEST_F(RZColorUtilitiesTests, GenerateHashedColor4)
    {
        glm::vec4 color1 = Razix::Utilities::GenerateHashedColor4(123);
        glm::vec4 color2 = Razix::Utilities::GenerateHashedColor4(123);
        glm::vec4 color3 = Razix::Utilities::GenerateHashedColor4(456);

        // Verify determinism
        EXPECT_EQ(color1, color2) << "GenerateHashedColor4 should return consistent results for the same seed.";

        // Verify different seeds produce different colors
        EXPECT_NE(color1, color3) << "GenerateHashedColor4 should produce different results for different seeds.";

        // Verify color components are within valid range
        EXPECT_GE(color1.r, 0.0f);
        EXPECT_LE(color1.r, 1.0f);
        EXPECT_GE(color1.g, 0.0f);
        EXPECT_LE(color1.g, 1.0f);
        EXPECT_GE(color1.b, 0.0f);
        EXPECT_LE(color1.b, 1.0f);
        EXPECT_GE(color1.a, 0.0f);
        EXPECT_LE(color1.a, 1.0f);
    }

    // Test for ColorToARGB
    TEST_F(RZColorUtilitiesTests, ColorToARGB)
    {
        glm::vec4 color = glm::vec4(0.5f, 0.25f, 0.75f, 1.0f);    // RGBA format
        uint32_t  argb  = Razix::Utilities::ColorToARGB(color);

        // Verify conversion to ARGB format
        uint8_t a = (argb >> 24) & 0xFF;
        uint8_t r = (argb >> 16) & 0xFF;
        uint8_t g = (argb >> 8) & 0xFF;
        uint8_t b = argb & 0xFF;

        EXPECT_EQ(a, static_cast<uint8_t>(color.a * 255.0f)) << "Alpha channel is incorrect.";
        EXPECT_EQ(r, static_cast<uint8_t>(color.r * 255.0f)) << "Red channel is incorrect.";
        EXPECT_EQ(g, static_cast<uint8_t>(color.g * 255.0f)) << "Green channel is incorrect.";
        EXPECT_EQ(b, static_cast<uint8_t>(color.b * 255.0f)) << "Blue channel is incorrect.";
    }
}    // namespace Razix