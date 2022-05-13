#pragma once

#include "Razix/Core/RZUUID.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZSprite.h"

#include "Razix/Graphics/RZModel.h"

#include "Razix/Scene/RZSceneCamera.h"
#include "Razix/Scripting/LuaScriptComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cereal/archives/json.hpp>

#include <entt.hpp>

namespace glm {
    // TODO: Move this to utilities + Make them look pretty
    // glm vectors
    template<class Archive>
    void serialize(Archive& archive, glm::vec2& v)
    {
        archive(v.x, v.y);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::vec3& v)
    {
        archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z));
    }
    template<class Archive>
    void serialize(Archive& archive, glm::vec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::ivec2& v)
    {
        archive(v.x, v.y);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::ivec3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::ivec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::uvec2& v)
    {
        archive(v.x, v.y);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::uvec3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::uvec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::dvec2& v)
    {
        archive(v.x, v.y);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::dvec3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::dvec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }

    // glm matrices
    template<class Archive>
    void serialize(Archive& archive, glm::mat2& m)
    {
        archive(m[0], m[1]);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::dmat2& m)
    {
        archive(m[0], m[1]);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::mat3& m)
    {
        archive(m[0], m[1], m[2]);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::mat4& m)
    {
        archive(m[0], m[1], m[2], m[3]);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::dmat4& m)
    {
        archive(m[0], m[1], m[2], m[3]);
    }

    // glm quats
    template<class Archive>
    void serialize(Archive& archive, glm::quat& q)
    {
        archive(q.x, q.y, q.z, q.w);
    }
    template<class Archive>
    void serialize(Archive& archive, glm::dquat& q)
    {
        archive(q.x, q.y, q.z, q.w);
    }
}    // namespace glm

namespace Razix {
    /**
     * Components are various classes that are added to the entities to provide functionality in a decoupled way
     * They have no info about entities at all
     */

    /**
     * An ID component contains an UUID that helps us to identify the entity uniquely in a scene
     */
    struct RAZIX_API IDComponent
    {
        /* Used to uniquely identify the entity */
        int UUID;

        IDComponent()                   = default;
        IDComponent(const IDComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            int uuid_string;
            archive(cereal::make_nvp("UUID", uuid_string));
            UUID = 0;
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("UUID", UUID));
        }
    };

    /**
     * A Tag components allows an entity to have a name
     */
    struct RAZIX_API TagComponent
    {
        /* Name of the entity */
        std::string Tag;

        TagComponent()                    = default;
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
    struct RAZIX_API ActiveComponent
    {
        bool Active = true;

        ActiveComponent()
            : Active(true) {}
        ActiveComponent(bool act)
            : Active(act) {}
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
        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

        template<typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Translation", Translation), cereal::make_nvp("Rotation", Rotation), cereal::make_nvp("Scale", Scale));
        }
    };

    /**
     * Establishes a hierarchical relationship between the entities in a scene
     */
    struct RAZIX_API HierarchyComponent
    {
        entt::entity First  = entt::null;
        entt::entity Parent = entt::null;
        entt::entity Prev   = entt::null;
        entt::entity Next   = entt::null;

        HierarchyComponent(entt::entity p)
            : Parent(p)
        {
            First = entt::null;
            Next  = entt::null;
            Prev  = entt::null;
        }
        HierarchyComponent()                          = default;
        HierarchyComponent(const HierarchyComponent&) = default;

        /* update hierarchy components when hierarchy component is added */
        static void OnConstruct(entt::registry& registry, entt::entity entity)
        {
            auto& hierarchy = registry.get<HierarchyComponent>(entity);
            if (hierarchy.Parent != entt::null) {
                auto& parent_hierarchy = registry.get_or_emplace<HierarchyComponent>(hierarchy.Parent);

                if (parent_hierarchy.First == entt::null) {
                    parent_hierarchy.First = entity;
                } else {
                    // get last children
                    auto prev_ent          = parent_hierarchy.First;
                    auto current_hierarchy = registry.try_get<HierarchyComponent>(prev_ent);
                    while (current_hierarchy != nullptr && current_hierarchy->Next != entt::null) {
                        prev_ent          = current_hierarchy->Next;
                        current_hierarchy = registry.try_get<HierarchyComponent>(prev_ent);
                    }
                    // add new
                    current_hierarchy->Next = entity;
                    hierarchy.Prev          = prev_ent;
                }
            }
        }
        // update hierarchy components when hierarchy component is removed
        static void OnDestroy(entt::registry& registry, entt::entity entity)
        {
            auto& hierarchy = registry.get<HierarchyComponent>(entity);
            // if is the first child
            if (hierarchy.Prev == entt::null || !registry.valid(hierarchy.Prev)) {
                if (hierarchy.Parent != entt::null && registry.valid(hierarchy.Parent)) {
                    auto parent_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.Parent);
                    if (parent_hierarchy != nullptr) {
                        parent_hierarchy->First = hierarchy.Next;
                        if (hierarchy.Next != entt::null) {
                            auto next_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.Next);
                            if (next_hierarchy != nullptr) {
                                next_hierarchy->Prev = entt::null;
                            }
                        }
                    }
                }
            } else {
                auto prev_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.Prev);
                if (prev_hierarchy != nullptr) {
                    prev_hierarchy->Next = hierarchy.Next;
                }
                if (hierarchy.Next != entt::null) {
                    auto next_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.Next);
                    if (next_hierarchy != nullptr) {
                        next_hierarchy->Prev = hierarchy.Prev;
                    }
                }
            }
        }
        static void Reparent(entt::entity entity, entt::entity parent, entt::registry& registry, HierarchyComponent& hierarchy)
        {
            HierarchyComponent::OnDestroy(registry, entity);

            hierarchy.Parent = entt::null;
            hierarchy.Next   = entt::null;
            hierarchy.Prev   = entt::null;

            if (parent != entt::null) {
                hierarchy.Parent = parent;
                HierarchyComponent::OnConstruct(registry, entity);
            }
        }

        template<typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("First", First), cereal::make_nvp("Next", Next), cereal::make_nvp("Previous", Prev), cereal::make_nvp("Parent", Parent));
        }
    };

    //-----------------------------------------------------------------------------------------------------
    // Engine components

    /**
     * The camera component attaches a camera to the entity that can be used to view the world from
     */
    struct RAZIX_API CameraComponent
    {
        RZSceneCamera Camera;
        bool          Primary = true;    // TODO: think about moving to Scene

        CameraComponent()                       = default;
        CameraComponent(const CameraComponent&) = default;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("isPrimary", Primary));
            archive(cereal::make_nvp("Camera", Camera));
        }
    };

    /**
     * Mesh renderer component references a mesh that will taken by the render to render a mesh on the 3D scene
     * It holds the reference to a 3D model or a primitive mesh to be rendered, so if a model is instantiated as
     * a Entity in the scene, each of it's children(which are also entities) will have a mesh renderer component
     *  will be instantiated as entities in the scene with a Hierarchy, Transform,Tag, Active and default components
     * attached to them, these all meshes are taken at once by the Renderer and rendered to the scene, any additional
     * information required to rendered can be inferred as needed
     */
    struct RAZIX_API MeshRendererComponent
    {
        Graphics::RZMesh* Mesh;

        MeshRendererComponent()
            : Mesh(nullptr) {}
        MeshRendererComponent(Graphics::MeshPrimitive primitive)
            : Mesh(Graphics::MeshFactory::CreatePrimitive(primitive)) {}
        MeshRendererComponent(Graphics::RZMesh* mesh)
            : Mesh(mesh) {}
        MeshRendererComponent(const MeshRendererComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            if (Mesh) {
                std::string meshName;
                archive(cereal::make_nvp("MeshName", meshName));
                Mesh->setName(meshName);
            }
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            if (Mesh)
                archive(cereal::make_nvp("MeshName", Mesh->getName()));
        }
    };

    /**
     * Renders a sprite in the Screen space, currently can be rendered only as a 2D entity within the view
     */
    struct RAZIX_API SpriteRendererComponent
    {
        Graphics::RZSprite* Sprite = nullptr;

        SpriteRendererComponent() {}
        SpriteRendererComponent(glm::vec4 color)
        {
            Sprite = new Graphics::RZSprite(color);
        }
        SpriteRendererComponent(Graphics::RZTexture2D* texture)
        {
            Sprite = new Graphics::RZSprite(texture);
        }
        SpriteRendererComponent(const SpriteRendererComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            //Sprite = new Graphics::RZSprite;
            std::string texturePath;
            archive(cereal::make_nvp("TexturePath", texturePath));
            if (!texturePath.empty()) {
                Graphics::RZTexture2D* texture = Graphics::RZTexture2D::CreateFromFile(texturePath, "sprite", Graphics::RZTexture2D::Wrapping::CLAMP_TO_EDGE);
                Sprite                         = new Graphics::RZSprite(texture);
            } else {
                glm::vec4 color;
                archive(cereal::make_nvp("Color", color));
                Sprite = new Graphics::RZSprite(color);
            }
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            if (Sprite->getTexture() != nullptr)
                archive(cereal::make_nvp("TexturePath", Sprite->getTexture()->getPath()));
            archive(cereal::make_nvp("Color", Sprite->getColour()));
        }

        //-----------------------------------------------------------------------------------------------------
        // Scripting System
    };

// List of all components that razix implements that is used while serialization
#define RAZIX_COMPONENTS IDComponent, TagComponent, ActiveComponent, TransformComponent, CameraComponent, SpriteRendererComponent, MeshRendererComponent, Graphics::RZModel, LuaScriptComponent    // The model component is a temporary hack, which will be replaced by MeshRenderer component soon
}    // namespace Razix