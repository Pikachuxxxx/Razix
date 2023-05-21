// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEntity.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {

    RZEntity::RZEntity(entt::entity handle, RZScene* scene)
        : m_Entity(handle), m_Scene(scene)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    }

    bool RZEntity::IsActive()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        {
            if (HasComponent<ActiveComponent>())
                return m_Scene->m_Registry.get<ActiveComponent>(m_Entity).Active;

            return true;
        }
    }

    void RZEntity::SetActive(bool isActive)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        GetOrAddComponent<ActiveComponent>().Active = isActive;
    }

    void RZEntity::SetParent(RZEntity entity)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        bool acceptable         = false;
        auto hierarchyComponent = TryGetComponent<HierarchyComponent>();
        if (hierarchyComponent != nullptr) {
            acceptable = entity.m_Entity != m_Entity && (!entity.IsParent(*this)) && (hierarchyComponent->Parent != m_Entity);
        } else
            acceptable = entity.m_Entity != m_Entity;

        if (!acceptable)
            return;

        if (hierarchyComponent)
            HierarchyComponent::Reparent(m_Entity, entity.m_Entity, m_Scene->getRegistry(), *hierarchyComponent);
        else {
            m_Scene->getRegistry().emplace<HierarchyComponent>(m_Entity, entity.m_Entity);
        }
    }

    RZEntity RZEntity::GetParent()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        auto hierarchyComp = TryGetComponent<HierarchyComponent>();
        if (hierarchyComp)
            return RZEntity(hierarchyComp->Parent, m_Scene);
        else
            return RZEntity(entt::null, nullptr);
    }

    std::vector<RZEntity> RZEntity::GetAllChildren()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
        std::vector<RZEntity> children;
        auto                  hierarchyComponent = TryGetComponent<HierarchyComponent>();
        if (hierarchyComponent) {
            entt::entity child = hierarchyComponent->First;
            while (child != entt::null && m_Scene->getRegistry().valid(child)) {
                children.emplace_back(child, m_Scene);
                hierarchyComponent = m_Scene->getRegistry().try_get<HierarchyComponent>(child);
                if (hierarchyComponent)
                    child = hierarchyComponent->Next;
            }
        }

        return children;
    }

    RZEntity::operator bool() const
    {
        return m_Entity != entt::null;
    }

    bool RZEntity::IsParent(RZEntity potentialParent)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        auto nodeHierarchyComponent = m_Scene->getRegistry().try_get<HierarchyComponent>(m_Entity);
        if (nodeHierarchyComponent) {
            auto parent = nodeHierarchyComponent->Parent;
            while (parent != entt::null) {
                if (parent == potentialParent.m_Entity) {
                    return true;
                } else {
                    nodeHierarchyComponent = m_Scene->getRegistry().try_get<HierarchyComponent>(parent);
                    parent                 = nodeHierarchyComponent ? nodeHierarchyComponent->Parent : entt::null;
                }
            }
        }
        return false;
    }
}    // namespace Razix