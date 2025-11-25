#pragma once

#include "Razix/Core/Utils/TRZSingleton.h"

namespace Razix {
    namespace Physics {

        // Jolt Physics World callbacks and settings

        class RAZIX_API RZPhysicsWorld : public RZSingleton<RZPhysicsWorld>
        {
        public:
            /* Initializes the Physics System */
            void StartUp();
            /* Shuts down the Physics systems and it's resources */
            void ShutDown();

            /* Gets the body interface that is used to create, manage and destroy the bodies in the physics world */
            RAZIX_FORCE_INLINE JPH::BodyInterface& getBodyManager() { return m_BodyInterface; }

        private:
            JPH::PhysicsSystem m_PhysicsSystem;
            JPH::BodyInterface m_BodyInterface;
        };
    }    // namespace Physics
}    // namespace Razix
