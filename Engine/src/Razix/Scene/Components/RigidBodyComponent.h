#pragma once

#include "Razix/Physics/RZRigidBody.h"

namespace Razix {
    /**
     * 
     */
    struct RAZIX_API RigidBodyComponent
    {
        Physics::RZRigidBody rigidbody;

        RigidBodyComponent()                          = default;
        RigidBodyComponent(const RigidBodyComponent&) = default;
    };
}    // namespace Razix