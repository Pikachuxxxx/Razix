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
            float3 InitialPosition;
            float3 InitialRotation;
            float3 HalfExtent;
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
