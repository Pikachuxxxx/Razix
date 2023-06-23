// clang-format off
#include "rzxpch.h"
// clang-format on
#include "TransformComponent.h"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

// https://stackoverflow.com/questions/49609654/quaternion-based-first-person-view-camera

glm::mat4 Razix::TransformComponent::GetGlobalTransform()
{
#if 0
glm::quat qPitch = glm::angleAxis(Rotation.x, glm::vec3(1, 0, 0));
    glm::quat qYaw   = glm::angleAxis(Rotation.y, gl m::vec3(0, 1, 0));
    glm::quat qRoll  = glm::angleAxis(Rotation.z, glm::vec3(0, 0, 1));

    glm::quat orientation = qPitch * qYaw;
    orientation           = glm::normalize(orientation);

    model = glm::translate(glm::mat4(1.0f), Translation);
    model *= glm::eulerAngleYZX(Rotation.y, Rotation.x, Rotation.z);
    model *= glm::scale(glm::mat4(1.0f), Scale);
#endif

    Transform = glm::mat4(1.0f);

    glm::quat rotationQuat{};
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    // Convert the Euler angles into
    rotationQuat   = glm::quat(Rotation);
    rotationMatrix = glm::mat4_cast(rotationQuat);

    Transform = glm::translate(Transform, Translation);
    Transform *= rotationMatrix;
    Transform = glm::scale(Transform, Scale);

    return WorldMatrix * Transform;
}

glm::mat4 Razix::TransformComponent::GetLocalTransform()
{
    Transform = glm::mat4(1.0f);

    glm::quat rotationQuat{};
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    // Convert the Euler angles into
    rotationQuat   = glm::quat(Rotation);
    rotationMatrix = glm::mat4_cast(rotationQuat);

    Transform = glm::translate(Transform, Translation);
    Transform *= rotationMatrix;
    Transform = glm::scale(Transform, Scale);

    return Transform;
}