// clang-format off
#include "rzxpch.h"
// clang-format on
#include "TransformComponent.h"

#include <glm/gtx/quaternion.hpp>

glm::mat4 Razix::TransformComponent::GetTransform() const
{
    glm::mat4 model(1.0f);
    model = glm::translate(glm::mat4(1.0f), Translation);
    model *= glm::toMat4(glm::quat(Rotation));
    model *= glm::scale(glm::mat4(1.0f), Scale);

    return model;
}
