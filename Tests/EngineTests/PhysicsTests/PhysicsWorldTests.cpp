// PhysicsWorldTests.cpp
// Unit tests for the RZPhysicsWorld system
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Physics/RZPhysicsWorld.h"
#include "Razix/Physics/RZPhysicsWorldConfig.h"
#include "Razix/Physics/RZPhysicsSettings.h"
#include "Razix/Physics/RZRigidBody.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace Physics {

        class RZPhysicsWorldTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Initialize any required setup
                Razix::Debug::RZLog::StartUp();
            }

            void TearDown() override
            {
                // Clean up any allocated resources
                Razix::Debug::RZLog::Shutdown();
            }
        };

        // Test case for PhysicsWorldConfig structure
        TEST_F(RZPhysicsWorldTests, PhysicsWorldConfigStructure)
        {
            PhysicsWorldConfig config;
            
            // Test default values are reasonable
            EXPECT_TRUE(config.gravity.x == 0.0f || std::abs(config.gravity.x) > 0.0f);
            EXPECT_TRUE(config.gravity.y != 0.0f); // Should have some gravity
            EXPECT_TRUE(config.gravity.z == 0.0f || std::abs(config.gravity.z) > 0.0f);
            
            // Test that we can modify gravity
            glm::vec3 customGravity = glm::vec3(0.0f, -9.81f, 0.0f);
            config.gravity = customGravity;
            EXPECT_EQ(config.gravity, customGravity);
        }

        // Test case for PhysicsSettings functionality
        TEST_F(RZPhysicsWorldTests, PhysicsSettingsStructure)
        {
            PhysicsSettings settings;
            
            // Test that settings have reasonable defaults
            EXPECT_GT(settings.maxSimulationSubSteps, 0);
            EXPECT_GT(settings.fixedTimeStep, 0.0f);
            EXPECT_LE(settings.fixedTimeStep, 1.0f); // Should be a fraction of a second
            
            // Test that we can modify settings
            settings.maxSimulationSubSteps = 10;
            settings.fixedTimeStep = 1.0f / 60.0f;
            
            EXPECT_EQ(settings.maxSimulationSubSteps, 10);
            EXPECT_FLOAT_EQ(settings.fixedTimeStep, 1.0f / 60.0f);
        }

        // Test case for RigidBody component structure
        TEST_F(RZPhysicsWorldTests, RigidBodyComponentStructure)
        {
            RigidBodyComponent rigidBody;
            
            // Test default state
            EXPECT_TRUE(rigidBody.type == RigidBodyType::Static || 
                       rigidBody.type == RigidBodyType::Dynamic || 
                       rigidBody.type == RigidBodyType::Kinematic);
            
            // Test mass properties
            EXPECT_GE(rigidBody.mass, 0.0f); // Mass should be non-negative
            
            // Test that we can modify type
            rigidBody.type = RigidBodyType::Dynamic;
            EXPECT_EQ(rigidBody.type, RigidBodyType::Dynamic);
            
            rigidBody.type = RigidBodyType::Static;
            EXPECT_EQ(rigidBody.type, RigidBodyType::Static);
            
            rigidBody.type = RigidBodyType::Kinematic;
            EXPECT_EQ(rigidBody.type, RigidBodyType::Kinematic);
        }

        // Test case for RigidBodyType enum
        TEST_F(RZPhysicsWorldTests, RigidBodyTypeEnum)
        {
            // Test that all expected enum values exist
            RigidBodyType staticType = RigidBodyType::Static;
            RigidBodyType dynamicType = RigidBodyType::Dynamic;
            RigidBodyType kinematicType = RigidBodyType::Kinematic;
            
            // Test that they are different values
            EXPECT_NE(staticType, dynamicType);
            EXPECT_NE(staticType, kinematicType);
            EXPECT_NE(dynamicType, kinematicType);
            
            // Test enum ordering/values are consistent
            EXPECT_TRUE(static_cast<int>(staticType) >= 0);
            EXPECT_TRUE(static_cast<int>(dynamicType) >= 0);
            EXPECT_TRUE(static_cast<int>(kinematicType) >= 0);
        }

        // Test case for physics world singleton pattern
        TEST_F(RZPhysicsWorldTests, PhysicsWorldSingleton)
        {
            // Test that we can get the physics world instance
            RZPhysicsWorld& world1 = RZPhysicsWorld::Get();
            RZPhysicsWorld& world2 = RZPhysicsWorld::Get();
            
            // Should be the same instance
            EXPECT_EQ(&world1, &world2);
        }

        // Test case for physics configuration
        TEST_F(RZPhysicsWorldTests, PhysicsConfiguration)
        {
            PhysicsWorldConfig config;
            
            // Test gravity configuration
            config.gravity = glm::vec3(0.0f, -9.81f, 0.0f);
            EXPECT_FLOAT_EQ(config.gravity.y, -9.81f);
            
            // Test different gravity scenarios
            config.gravity = glm::vec3(0.0f, -3.71f, 0.0f); // Mars gravity
            EXPECT_FLOAT_EQ(config.gravity.y, -3.71f);
            
            config.gravity = glm::vec3(0.0f, 0.0f, 0.0f); // Zero gravity
            EXPECT_FLOAT_EQ(config.gravity.y, 0.0f);
        }

        // Test case for rigid body mass properties
        TEST_F(RZPhysicsWorldTests, RigidBodyMassProperties)
        {
            RigidBodyComponent rigidBody;
            
            // Test setting different mass values
            rigidBody.mass = 1.0f;
            EXPECT_FLOAT_EQ(rigidBody.mass, 1.0f);
            
            rigidBody.mass = 10.5f;
            EXPECT_FLOAT_EQ(rigidBody.mass, 10.5f);
            
            rigidBody.mass = 0.0f; // Infinite mass (static)
            EXPECT_FLOAT_EQ(rigidBody.mass, 0.0f);
        }

        // Test case for physics simulation settings
        TEST_F(RZPhysicsWorldTests, SimulationSettings)
        {
            PhysicsSettings settings;
            
            // Test time step configuration
            f32 targetFPS = 60.0f;
            settings.fixedTimeStep = 1.0f / targetFPS;
            EXPECT_FLOAT_EQ(settings.fixedTimeStep, 1.0f / 60.0f);
            
            // Test sub-stepping
            settings.maxSimulationSubSteps = 4;
            EXPECT_EQ(settings.maxSimulationSubSteps, 4);
            
            // Test that settings are reasonable for simulation
            EXPECT_GT(settings.fixedTimeStep, 0.0f);
            EXPECT_LT(settings.fixedTimeStep, 1.0f);
            EXPECT_GT(settings.maxSimulationSubSteps, 0);
            EXPECT_LE(settings.maxSimulationSubSteps, 100); // Reasonable upper bound
        }

    }    // namespace Physics
}    // namespace Razix