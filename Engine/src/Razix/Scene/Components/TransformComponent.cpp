// clang-format off
#include "rzxpch.h"
// clang-format on
#include "TransformComponent.h"

#include <glm/gtx/quaternion.hpp>

glm::mat4 Razix::TransformComponent::GetTransform() const

{
    glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

    return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
}
