// SoundEngineTests.cpp
// Unit tests for the RZSoundEngine class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Audio/RZSoundEngine.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace Audio {

        class RZSoundEngineTests : public ::testing::Test
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

        // Test case for singleton access
        TEST_F(RZSoundEngineTests, SingletonAccess)
        {
            RZSoundEngine& engine1 = RZSoundEngine::Get();
            RZSoundEngine& engine2 = RZSoundEngine::Get();
            
            EXPECT_EQ(&engine1, &engine2) << "Singleton pattern failed - different instances returned.";
        }

        // Test case for StartUp method
        TEST_F(RZSoundEngineTests, StartUpMethod)
        {
            RZSoundEngine& engine = RZSoundEngine::Get();
            
            // Should not throw exceptions
            EXPECT_NO_THROW(engine.StartUp()) << "StartUp method should not throw exceptions.";
        }

        // Test case for ShutDown method
        TEST_F(RZSoundEngineTests, ShutDownMethod)
        {
            RZSoundEngine& engine = RZSoundEngine::Get();
            
            // Should not throw exceptions
            EXPECT_NO_THROW(engine.ShutDown()) << "ShutDown method should not throw exceptions.";
        }

        // Test case for renderAudio method
        TEST_F(RZSoundEngineTests, RenderAudioMethod)
        {
            RZSoundEngine& engine = RZSoundEngine::Get();
            
            // Should not throw exceptions
            EXPECT_NO_THROW(engine.renderAudio()) << "renderAudio method should not throw exceptions.";
        }

        // Test case for StartUp/ShutDown sequence
        TEST_F(RZSoundEngineTests, StartUpShutDownSequence)
        {
            RZSoundEngine& engine = RZSoundEngine::Get();
            
            EXPECT_NO_THROW({
                engine.StartUp();
                engine.renderAudio();
                engine.ShutDown();
            }) << "StartUp/renderAudio/ShutDown sequence should work without exceptions.";
        }

    }    // namespace Audio
}    // namespace Razix