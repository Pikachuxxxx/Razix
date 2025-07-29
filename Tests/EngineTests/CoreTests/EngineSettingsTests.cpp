// EngineSettingsTests.cpp
// Unit tests for the RZEngineSettings structures and enums
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZEngineSettings.h"

#include <gtest/gtest.h>

namespace Razix {

    class RZEngineSettingsTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Initialize any required setup
        }

        void TearDown() override
        {
            // Clean up any allocated resources
        }
    };

    // Test case for PerfMode enum values
    TEST_F(RZEngineSettingsTests, PerfModeEnumValues)
    {
        // Test that PerfMode enum has expected values
        EXPECT_EQ(static_cast<int>(PerfMode::kNone), 0);
        EXPECT_EQ(static_cast<int>(PerfMode::kFidelity), 1);
        EXPECT_EQ(static_cast<int>(PerfMode::kPerformance), 2);
        EXPECT_EQ(static_cast<int>(PerfMode::COUNT), 3);
    }

    // Test case for GfxQualityMode enum values
    TEST_F(RZEngineSettingsTests, GfxQualityModeEnumValues)
    {
        // Test that GfxQualityMode enum has expected values
        EXPECT_EQ(static_cast<int>(GfxQualityMode::kLow), 0);
        EXPECT_EQ(static_cast<int>(GfxQualityMode::kMedium), 1);
        EXPECT_EQ(static_cast<int>(GfxQualityMode::kHigh), 2);
        EXPECT_EQ(static_cast<int>(GfxQualityMode::COUNT), 3);
    }

    // Test case for EngineSettings structure default values
    TEST_F(RZEngineSettingsTests, EngineSettingsDefaults)
    {
        EngineSettings settings;
        
        // Test default boolean settings
        EXPECT_TRUE(settings.EnableAPIValidation);
        EXPECT_FALSE(settings.EnableMSAA);
        EXPECT_FALSE(settings.EnableBindless);
        EXPECT_FALSE(settings.EnableBarrierLogging);
        
        // Test default quality settings
        EXPECT_EQ(settings.GfxQuality, GfxQualityMode::kHigh);
        EXPECT_EQ(settings.PerformanceMode, PerfMode::kFidelity);
        
        // Test default numeric settings
        EXPECT_EQ(settings.MaxShadowCascades, 4);
        EXPECT_EQ(settings.MSAASamples, 4);
        
        // Test default resolution and FPS
        EXPECT_EQ(settings.PreferredResolution, Gfx::Resolution::k1440p);
        EXPECT_EQ(settings.TargetFPSCap, Gfx::TargetFPS::k120);
    }

    // Test case for EngineSettings structure modification
    TEST_F(RZEngineSettingsTests, EngineSettingsModification)
    {
        EngineSettings settings;
        
        // Test modifying boolean settings
        settings.EnableAPIValidation = false;
        settings.EnableMSAA = true;
        settings.EnableBindless = true;
        settings.EnableBarrierLogging = true;
        
        EXPECT_FALSE(settings.EnableAPIValidation);
        EXPECT_TRUE(settings.EnableMSAA);
        EXPECT_TRUE(settings.EnableBindless);
        EXPECT_TRUE(settings.EnableBarrierLogging);
        
        // Test modifying quality settings
        settings.GfxQuality = GfxQualityMode::kLow;
        settings.PerformanceMode = PerfMode::kPerformance;
        
        EXPECT_EQ(settings.GfxQuality, GfxQualityMode::kLow);
        EXPECT_EQ(settings.PerformanceMode, PerfMode::kPerformance);
        
        // Test modifying numeric settings
        settings.MaxShadowCascades = 2;
        settings.MSAASamples = 8;
        
        EXPECT_EQ(settings.MaxShadowCascades, 2);
        EXPECT_EQ(settings.MSAASamples, 8);
    }

    // Test case for enum range validation
    TEST_F(RZEngineSettingsTests, EnumRangeValidation)
    {
        // Test that enum values are within expected ranges
        EXPECT_LT(static_cast<int>(PerfMode::kNone), static_cast<int>(PerfMode::COUNT));
        EXPECT_LT(static_cast<int>(PerfMode::kFidelity), static_cast<int>(PerfMode::COUNT));
        EXPECT_LT(static_cast<int>(PerfMode::kPerformance), static_cast<int>(PerfMode::COUNT));
        
        EXPECT_LT(static_cast<int>(GfxQualityMode::kLow), static_cast<int>(GfxQualityMode::COUNT));
        EXPECT_LT(static_cast<int>(GfxQualityMode::kMedium), static_cast<int>(GfxQualityMode::COUNT));
        EXPECT_LT(static_cast<int>(GfxQualityMode::kHigh), static_cast<int>(GfxQualityMode::COUNT));
    }

    // Test case for settings copy and assignment
    TEST_F(RZEngineSettingsTests, SettingsCopyAssignment)
    {
        EngineSettings settings1;
        settings1.EnableAPIValidation = false;
        settings1.GfxQuality = GfxQualityMode::kMedium;
        settings1.MaxShadowCascades = 3;
        
        // Test copy construction
        EngineSettings settings2 = settings1;
        EXPECT_EQ(settings2.EnableAPIValidation, settings1.EnableAPIValidation);
        EXPECT_EQ(settings2.GfxQuality, settings1.GfxQuality);
        EXPECT_EQ(settings2.MaxShadowCascades, settings1.MaxShadowCascades);
        
        // Test assignment
        EngineSettings settings3;
        settings3 = settings1;
        EXPECT_EQ(settings3.EnableAPIValidation, settings1.EnableAPIValidation);
        EXPECT_EQ(settings3.GfxQuality, settings1.GfxQuality);
        EXPECT_EQ(settings3.MaxShadowCascades, settings1.MaxShadowCascades);
    }

    // Test case for boolean settings combinations
    TEST_F(RZEngineSettingsTests, BooleanSettingsCombinations)
    {
        EngineSettings settings;
        
        // Test various combinations of boolean settings
        settings.EnableAPIValidation = true;
        settings.EnableMSAA = true;
        EXPECT_TRUE(settings.EnableAPIValidation && settings.EnableMSAA);
        
        settings.EnableBindless = true;
        settings.EnableBarrierLogging = true;
        EXPECT_TRUE(settings.EnableBindless && settings.EnableBarrierLogging);
        
        // Test mixed combinations
        settings.EnableAPIValidation = false;
        settings.EnableMSAA = true;
        EXPECT_TRUE(!settings.EnableAPIValidation && settings.EnableMSAA);
    }

    // Test case for quality vs performance trade-offs
    TEST_F(RZEngineSettingsTests, QualityPerformanceTradeoffs)
    {
        EngineSettings settings;
        
        // Test high quality with fidelity mode
        settings.GfxQuality = GfxQualityMode::kHigh;
        settings.PerformanceMode = PerfMode::kFidelity;
        EXPECT_EQ(settings.GfxQuality, GfxQualityMode::kHigh);
        EXPECT_EQ(settings.PerformanceMode, PerfMode::kFidelity);
        
        // Test low quality with performance mode
        settings.GfxQuality = GfxQualityMode::kLow;
        settings.PerformanceMode = PerfMode::kPerformance;
        EXPECT_EQ(settings.GfxQuality, GfxQualityMode::kLow);
        EXPECT_EQ(settings.PerformanceMode, PerfMode::kPerformance);
    }

    // Test case for MSAA configuration consistency
    TEST_F(RZEngineSettingsTests, MSAAConfigurationConsistency)
    {
        EngineSettings settings;
        
        // When MSAA is disabled, samples value doesn't affect rendering
        settings.EnableMSAA = false;
        settings.MSAASamples = 8;
        EXPECT_FALSE(settings.EnableMSAA);
        EXPECT_EQ(settings.MSAASamples, 8);
        
        // When MSAA is enabled, samples should be reasonable values
        settings.EnableMSAA = true;
        settings.MSAASamples = 2;
        EXPECT_TRUE(settings.EnableMSAA);
        EXPECT_EQ(settings.MSAASamples, 2);
        
        settings.MSAASamples = 16;
        EXPECT_EQ(settings.MSAASamples, 16);
    }

}    // namespace Razix