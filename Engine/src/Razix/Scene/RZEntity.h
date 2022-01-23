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
        RZEntity(const RZEntity& other) = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            RAZIX_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
            // TODO: callback to the scene when a component is added with the type of the component that was added
            //m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template<typename T>
        T& GetComponent()
        {
            RAZIX_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_Entity);
        }

        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.has<T>(m_Entity);
        }

        template<typename T>
        void RemoveComponent()
        {
            RAZIX_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_Entity);
        }

        operator bool() const { return m_Entity != entt::null; }
        operator entt::entity() const { return m_Entity; }
        operator uint32_t() const { return (uint32_t) m_Entity; }

    private:
        entt::entity    m_Entity{entt::null};
        RZUUID*         m_UUID;
        RZScene*        m_Scene = nullptr;
    };
}

