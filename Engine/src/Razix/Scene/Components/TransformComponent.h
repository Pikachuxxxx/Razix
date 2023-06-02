#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace Razix {
    /**
     * A transform components represents the transformation of the entity in the game world
     */
    struct RAZIX_API TransformComponent
    {
        glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
        glm::vec3 Rotation    = {0, 0, 0}; /* Stores the Rotation in radians */
        glm::vec3 Scale       = {1.0f, 1.0f, 1.0f};
        glm::mat4 Transform   = glm::mat4(1.0);
        // TODO: Serialize this?
        glm::mat4 WorldMatrix = glm::mat4(1.0);

        TransformComponent()                          = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation) {}

        /* Gets the transformation matrix */
        glm::mat4 GetGlobalTransform();
        glm::mat4 GetLocalTransform();
        glm::mat4 GetWorldTransform() { return GetGlobalTransform(); }
        void      SetWorldTransform(glm::mat4 matrix) { WorldMatrix = matrix; }

        template<typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Translation", Translation), cereal::make_nvp("Rotation", Rotation), cereal::make_nvp("Scale", Scale), cereal::make_nvp("Transform", Transform));
        }
    };
}    // namespace Razix