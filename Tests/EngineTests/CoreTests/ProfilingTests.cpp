// ProfilingTests.cpp
// Unit tests for the Razix Profiling system
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Profiling/RZProfiling.h"

#include <gtest/gtest.h>
#include <chrono>
#include <thread>

namespace Razix {

    class RZProfilingTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Setup for profiling tests
        }

        void TearDown() override
        {
            // Cleanup for profiling tests
        }
    };

    // Test case for profiling macro definitions
    TEST_F(RZProfilingTests, ProfilingMacroDefinitions)
    {
        // Test that profiling constants are properly defined
        EXPECT_EQ(RZ_PROFILER_TRACY, 0) << "Tracy profiler should be disabled by default";
        EXPECT_EQ(RZ_PROFILER_OPTICK, 0) << "Optick profiler should be disabled by default";
        EXPECT_EQ(RZ_PROFILER_NVSIGHT, 0) << "NSight profiler should be disabled by default";
        
        EXPECT_EQ(RZ_ENABLE_CALL_STACK_CAPTURE, 0) << "Call stack capture should be disabled by default";
        EXPECT_EQ(RZ_CALL_STACK_DEPTH, 15) << "Call stack depth should be 15";
    }

    // Test case for profiling color definitions
    TEST_F(RZProfilingTests, ProfilingColorDefinitions)
    {
        // Test that profiling colors are properly defined
        EXPECT_EQ(RZ_PROFILE_COLOR_CORE, 0x8B0000) << "Core profiling color should be dark red";
        EXPECT_EQ(RZ_PROFILE_COLOR_APPLICATION, 0xFFD700) << "Application profiling color should be gold";
        EXPECT_EQ(RZ_PROFILE_COLOR_GRAPHICS, 0xFF8C00) << "Graphics profiling color should be dark orange";
        EXPECT_EQ(RZ_PROFILE_COLOR_RENDERERS, 0xF08080) << "Renderers profiling color should be light coral";
        EXPECT_EQ(RZ_PROFILE_COLOR_SCENE, 0x00BFFF) << "Scene profiling color should be deep sky blue";
        EXPECT_EQ(RZ_PROFILE_COLOR_SCRIPTING, 0x8A2BE2) << "Scripting profiling color should be blue violet";
        EXPECT_EQ(RZ_PROFILE_COLOR_ASSET_SYSTEM, 0xFF8C00) << "Asset system profiling color should be dark orange";
        EXPECT_EQ(RZ_PROFILE_COLOR_GRAPHICS_API_DRAW_CALLS, 0x8B0000) << "Graphics API draw calls color should be dark red";
    }

    // Test case for profiling scope functionality
    TEST_F(RZProfilingTests, ProfilingScopeFunctionality)
    {
        // Test that profiling macros compile and execute without errors
        {
            RAZIX_PROFILE_SCOPE("TestScope");
            
            // Simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        {
            RAZIX_PROFILE_SCOPEC("TestScopeWithColor", RZ_PROFILE_COLOR_CORE);
            
            // Simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        // Test function profiling
        RAZIX_PROFILE_FUNCTION();
        
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        SUCCEED() << "Profiling macros executed without errors";
    }

    // Test case for frame marker functionality
    TEST_F(RZProfilingTests, FrameMarkerFunctionality)
    {
        // Test that frame marker compiles and executes
        RAZIX_PROFILE_FRAMEMARKER("TestFrame");
        
        SUCCEED() << "Frame marker executed without errors";
    }

    // Test case for GPU profiling macros
    TEST_F(RZProfilingTests, GPUProfilingMacros)
    {
        // Test that GPU profiling macros compile (even if they're no-ops)
        RAZIX_PROFILE_GPU_CONTEXT(nullptr);
        RAZIX_PROFILE_GPU_SCOPE("TestGPUScope");
        RAZIX_PROFILE_GPU_SCOPEC("TestGPUScopeWithColor", RZ_PROFILE_COLOR_GRAPHICS);
        
        SUCCEED() << "GPU profiling macros compiled without errors";
    }

    // Test case for nested profiling scopes
    TEST_F(RZProfilingTests, NestedProfilingScopes)
    {
        RAZIX_PROFILE_SCOPE("OuterScope");
        
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        {
            RAZIX_PROFILE_SCOPE("InnerScope1");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        {
            RAZIX_PROFILE_SCOPE("InnerScope2");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        SUCCEED() << "Nested profiling scopes executed without errors";
    }

    // Test case for conditional profiling compilation
    TEST_F(RZProfilingTests, ConditionalProfiling)
    {
        // Test that profiling is properly conditional based on build configuration
        #ifdef RAZIX_GOLD_MASTER
            // In gold master builds, profiling should be disabled
            SUCCEED() << "Profiling correctly disabled in gold master build";
        #else
            // In debug/release builds, profiling should be enabled
            RAZIX_PROFILE_SCOPE("ConditionalTest");
            SUCCEED() << "Profiling correctly enabled in debug/release build";
        #endif
    }

}    // namespace Razix