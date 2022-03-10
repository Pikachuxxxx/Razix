#pragma once

#include "Razix/Core/RZUUID.h"

#include <entt.hpp>

namespace Razix {
    
    // Forward declaration
    class RZScene;

    /* An entity represents the basic building block of a scene, they are the objects the exist in the game world */
    class RAZIX_API RZEntity
    {
    public:
        RZEntity() = default;
        RZEntity(entt::entity handle, RZScene* scene);
        ~RZEntity() {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            RAZIX_CORE_ASSERT((!HasComponent<T>()), "RZEntity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
            // TODO: callback to the scene when a component is added with the type of the component that was added
            //m_Scene->OnComponentAdded<T>(*this, component);
        }

        template <typename T, typename... Args>
        T& GetOrAddComponent(Args&&... args)
        {
            return m_Scene->m_Registry.get_or_emplace<T>(m_Entity, std::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        void AddOrReplaceComponent(Args&&... args)
        {
            m_Scene->m_Registry.emplace_or_replace<T>(m_Entity, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            RAZIX_CORE_ASSERT(HasComponent<T>(), "RZEntity does not have component!");
            return m_Scene->m_Registry.get<T>(m_Entity);
        }

        template <typename T>
        T* TryGetComponent()
        {
            return m_Scene->m_Registry.try_get<T>(m_Entity);
        }

        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.has<T>(m_Entity);
        }

        template<typename T>
        void RemoveComponent()
        {
            RAZIX_CORE_ASSERT(HasComponent<T>(), "RZEntity does not have component!");
            m_Scene->m_Registry.remove<T>(m_Entity);
        }

        bool IsActive();
        void SetActive(bool isActive);

        void SetParent(RZEntity entity);
        RZEntity GetParent();
        std::vector<RZEntity> GetChildren();
        bool IsParent(RZEntity potentialParent);
        

        operator bool() const { return m_Entity != entt::null; }
        operator entt::entity() const { return m_Entity; }
        operator uint32_t() const { return (uint32_t) m_Entity; }
        bool operator==(const RZEntity& other) const { return m_Entity == other.m_Entity && m_Scene == other.m_Scene; }
        bool operator!=(const RZEntity& other) const { return !(*this == other); }

    private:
        entt::entity    m_Entity{entt::null};
        RZScene*        m_Scene = nullptr;
    };
}

