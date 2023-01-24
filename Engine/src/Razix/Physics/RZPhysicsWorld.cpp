// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPhysicsWorld.h"

namespace Razix {
    namespace Physics {

        void RZPhysicsWorld::StartUp()
        {
            RAZIX_CORE_INFO("[Physics System] Starting Up Jolt Physics System");

            // Initialize the JPH Factory instance

            JPH::Factory::sInstance = new JPH::Factory;
        }

        void RZPhysicsWorld::ShutDown()
        {
            RAZIX_CORE_INFO("[Physics System] Shutting down Jolt Physics System");

            // Destroy the factory
            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }

    }    // namespace Physics
}    // namespace Razix