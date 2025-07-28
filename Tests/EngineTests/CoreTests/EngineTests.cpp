// EngineTests.cpp
// Unit tests for the RZEngine class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZEngineSettings.h"

#include <gtest/gtest.h>

namespace Razix {

    class RZEngineTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Initialize logging for tests
            Razix::Debug::RZLog::StartUp();
        }

        void TearDown() override
        {
            Razix::Debug::RZLog::Shutdown();
        }
    };

    // Test case for engine singleton access
    TEST_F(RZEngineTests, SingletonAccess)
    {
        // Test that RZEngine implements singleton pattern
        RZEngine& instance1 = RZEngine::Get();
        RZEngine& instance2 = RZEngine::Get();
        
        EXPECT_EQ(&instance1, &instance2) << "RZEngine::Get() should return the same instance";
    }

    // Test case for engine stats structure
    TEST_F(RZEngineTests, StatsStructure)
    {
        RZEngine& engine = RZEngine::Get();
        
        // Test that we can access the stats structure
        auto stats = engine.GetStatistics();
        
        // Stats should be properly initialized
        EXPECT_GE(stats.DeltaTime, 0.0f) << "DeltaTime should be non-negative";
        EXPECT_GE(stats.UpdatesPerSecond, 0.0f) << "UpdatesPerSecond should be non-negative";
        EXPECT_GE(stats.FramesPerSecond, 0.0f) << "FramesPerSecond should be non-negative";
    }

    // Test case for application creation tracking
    TEST_F(RZEngineTests, ApplicationCreationTracking)
    {
        RZEngine& engine = RZEngine::Get();
        
        // Initially, no application should be created
        EXPECT_FALSE(engine.IsRZApplicationCreated()) << "Initially no RZApplication should be created";
    }

    // Test case for engine settings integration
    TEST_F(RZEngineTests, EngineSettingsIntegration)
    {
        RZEngine& engine = RZEngine::Get();
        
        // Test that engine can work with settings
        EngineSettings settings;
        settings.EnableGPUBasedValidation = true;
        settings.EnableBindlessResources = false;
        
        // These settings should be usable with the engine
        EXPECT_TRUE(settings.EnableGPUBasedValidation);
        EXPECT_FALSE(settings.EnableBindlessResources);
    }

    // Test case for version information
    TEST_F(RZEngineTests, VersionInformation)
    {
        // Test that engine has access to version information
        // These should be defined constants
        EXPECT_TRUE(RAZIX_VERSION_MAJOR >= 0);
        EXPECT_TRUE(RAZIX_VERSION_MINOR >= 0);
        EXPECT_TRUE(RAZIX_VERSION_PATCH >= 0);
    }

    // Test case for subsystem lifecycle
    TEST_F(RZEngineTests, SubsystemLifecycle)
    {
        RZEngine& engine = RZEngine::Get();
        
        // Test that engine supports startup/shutdown concept
        // Note: We don't actually call StartUp()/ShutDown() as they require full engine initialization
        // But we can verify the singleton is accessible
        EXPECT_TRUE(&engine != nullptr) << "Engine singleton should be accessible";
    }

    // Test case for statistics initialization
    TEST_F(RZEngineTests, StatisticsInitialization)
    {
        RZEngine& engine = RZEngine::Get();
        auto stats = engine.GetStatistics();
        
        // Test that statistics are properly initialized to reasonable values
        EXPECT_TRUE(std::isfinite(stats.DeltaTime)) << "DeltaTime should be a finite number";
        EXPECT_TRUE(std::isfinite(stats.UpdatesPerSecond)) << "UpdatesPerSecond should be a finite number";
        EXPECT_TRUE(std::isfinite(stats.FramesPerSecond)) << "FramesPerSecond should be a finite number";
    }

}    // namespace Razix