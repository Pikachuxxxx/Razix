#pragma once

#include "Razix/Core/RZCore.h"

#include <cereal/cereal.hpp>
#include <entt.hpp>

namespace Razix {
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
        static void OnConstruct(entt::registry& registry, entt::entity entity);
        // update hierarchy components when hierarchy component is removed
        static void OnDestroy(entt::registry& registry, entt::entity entity);
        static void Reparent(entt::entity entity, entt::entity parent, entt::registry& registry, HierarchyComponent& hierarchy);

        template<typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("First", First), cereal::make_nvp("Next", Next), cereal::make_nvp("Previous", Prev), cereal::make_nvp("Parent", Parent));
        }
    };
}    // namespace Razix