// clang-format off
#include "rzxpch.h"
// clang-format on
#include "HierarchyComponent.h"

namespace Razix {
    void HierarchyComponent::OnConstruct(entt::registry& registry, entt::entity entity)
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

    void HierarchyComponent::OnDestroy(entt::registry& registry, entt::entity entity)
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

    void HierarchyComponent::Reparent(entt::entity entity, entt::entity parent, entt::registry& registry, HierarchyComponent& hierarchy)
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

}    // namespace Razix
