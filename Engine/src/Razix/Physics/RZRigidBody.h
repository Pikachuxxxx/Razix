#pragma once

namespace Razix {
    namespace Physics {

        enum BodyType
        {
            STATIC,
            KINEMATIC,
            DYNAMIC
        };

        struct RigidBodyCreateSettings
        {
            glm::vec3 InitialPosition;
            glm::vec3 InitialRotation;
            glm::vec3 HalfExtent;
            f32       Restitution;
            f32       Friction;
            f32       Mass;
            f32       InitialVelocity;
            BodyType  Type;
        };

        class RAZIX_API RZRigidBody
        {
        public:
            RZRigidBody() {}
            ~RZRigidBody() {}

            void create(RigidBodyCreateSettings settings);

        private:
        };

    }    // namespace Physics
}    // namespace Razix
