// clang-format off
#include "rzxpch.h"
// clang-format on
#include "TransformComponent.h"

#include <glm/gtx/quaternion.hpp>

// https://stackoverflow.com/questions/49609654/quaternion-based-first-person-view-camera

glm::mat4 Razix::TransformComponent::GetTransform() const
{
    glm::quat qPitch = glm::angleAxis(Rotation.x, glm::vec3(1, 0, 0));
    glm::quat qYaw   = glm::angleAxis(Rotation.y, glm::vec3(0, 1, 0));
    glm::quat qRoll  = glm::angleAxis(Rotation.z, glm::vec3(0, 0, 1));

    glm::quat orientation = qPitch * qYaw * qRoll;
    orientation           = glm::normalize(orientation);

    glm::mat4 model(1.0f);
    model = glm::translate(glm::mat4(1.0f), Translation);
    model *= glm::mat4_cast(orientation);
    model *= glm::scale(glm::mat4(1.0f), Scale);

    //return model;

    return Transform;
}