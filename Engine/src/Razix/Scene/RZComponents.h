#pragma once

#include "Razix/Core/RZUUID.h"
#include "Razix/Scene/RZSceneCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <cereal/archives/json.hpp>

#include <entt.hpp>

// TODO: Move this to utilities + Make them look pretty
namespace glm {
    // glm vectors
    template<class Archive> void serialize(Archive& archive, glm::vec2& v) { archive(v.x, v.y); }
    template<class Archive> void serialize(Archive& archive, glm::vec3& v) { archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z)); }
    template<class Archive> void serialize(Archive& archive, glm::vec4& v) { archive(v.x, v.y, v.z, v.w); }
    template<class Archive> void serialize(Archive& archive, glm::ivec2& v) { archive(v.x, v.y); }
    template<class Archive> void serialize(Archive& archive, glm::ivec3& v) { archive(v.x, v.y, v.z); }
    template<class Archive> void serialize(Archive& archive, glm::ivec4& v) { archive(v.x, v.y, v.z, v.w); }
    template<class Archive> void serialize(Archive& archive, glm::uvec2& v) { archive(v.x, v.y); }
    template<class Archive> void serialize(Archive& archive, glm::uvec3& v) { archive(v.x, v.y, v.z); }
    template<class Archive> void serialize(Archive& archive, glm::uvec4& v) { archive(v.x, v.y, v.z, v.w); }
    template<class Archive> void serialize(Archive& archive, glm::dvec2& v) { archive(v.x, v.y); }
    template<class Archive> void serialize(Archive& archive, glm::dvec3& v) { archive(v.x, v.y, v.z); }
    template<class Archive> void serialize(Archive& archive, glm::dvec4& v) { archive(v.x, v.y, v.z, v.w); }

    // glm matrices
    template<class Archive> void serialize(Archive& archive, glm::mat2& m) { archive(m[0], m[1]); }
    template<class Archive> void serialize(Archive& archive, glm::dmat2& m) { archive(m[0], m[1]); }
    template<class Archive> void serialize(Archive& archive, glm::mat3& m) { archive(m[0], m[1], m[2]); }
    template<class Archive> void serialize(Archive& archive, glm::mat4& m) { archive(m[0], m[1], m[2], m[3]); }
    template<class Archive> void serialize(Archive& archive, glm::dmat4& m) { archive(m[0], m[1], m[2], m[3]); }

    // glm quats
    template<class Archive> void serialize(Archive& archive, glm::quat& q) { archive(q.x, q.y, q.z, q.w); }
    template<class Archive> void serialize(Archive& archive, glm::dquat& q) { archive(q.x, q.y, q.z, q.w); }
}

namespace Razix {
    /**
     * Components are various classes that are added to the entities to provide functionality in a decoupled way
     * They have no info about entities at all
     */ 
    
    /**
     * An ID component contains an UUID that helps us to identify the entity uniquely in a scene
     */
    struct IDComponent
    {
        /* Used to uniquely identify the entity */
        RZUUID UUID;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            std::string uuid_string;
            archive(cereal::make_nvp("UUID", uuid_string));
            UUID = RZUUID::FromStrFactory(uuid_string);
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("UUID", UUID.str()));
        }
    };

    /**
     * A Tag components allows an entity to have a name
     */
    struct TagComponent
    {
        /* Name of the entity */
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Tag", Tag));
        }
    };

    /**
     * Active component is used to tell whether the component is active or inactive in the scene
     */
    struct ActiveComponent
    {
        bool Active = true;

        ActiveComponent()
            : Active(true) { }
        ActiveComponent(bool act)
            : Active(act) { }
        ActiveComponent(const ActiveComponent&) = default;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("isActive", Active));
        }
    };

    /**
     * A transform components represents the transformation of the entity in the game world
     */
    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation) { }

        /* Gets the transformation matrix */
        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Translation", Translation), cereal::make_nvp("Rotation", Rotation), cereal::make_nvp("Scale", Scale));
        }
    };

    /**
     * The camera component is attaches a camera to the entity that can be used to view the world from
     */
    struct CameraComponent
    {
        RZSceneCamera Camera;
        bool Primary = true; // TODO: think about moving to Scene

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("isPrimary", Primary));
            archive(cereal::make_nvp("Camera", Camera));
        }
    };

    /**
     * Establishes a hierarchical relationship between the entities in a scene
     */
    // TODO: WIP
    struct HierarchyComponent
    {
        entt::entity Parent = entt::null;
        entt::entity Root = entt::null;
        entt::entity Next = entt::null;
        entt::entity Prev = entt::null;

        HierarchyComponent(entt::entity p)
            : Parent(p)
        {
            Root = entt::null;
            Next = entt::null;
            Prev = entt::null;
        }
        HierarchyComponent(const HierarchyComponent&) = default;

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Root", Root), cereal::make_nvp("Next", Next), cereal::make_nvp("Previous", Prev), cereal::make_nvp("Parent", Parent));
        }
    };

    // List of all components that razix implements that is used while serialization
    #define RAZIX_COMPONENTS IDComponent, TagComponent, ActiveComponent, TransformComponent, CameraComponent

}