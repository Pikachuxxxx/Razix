#pragma once

namespace Razix {
    /**
     * A transform components represents the transformation of the entity in the game world
     */
    struct RAZIX_API TransformComponent
    {
        glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
        glm::vec3 Rotation    = {0.0f, 0.0f, 0.0f};
        glm::vec3 Scale       = {1.0f, 1.0f, 1.0f};

        TransformComponent()                          = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation) {}

        /* Gets the transformation matrix */
        glm::mat4 GetTransform() const;

        template<typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Translation", Translation), cereal::make_nvp("Rotation", Rotation), cereal::make_nvp("Scale", Scale));
        }
    };
}    // namespace Razix