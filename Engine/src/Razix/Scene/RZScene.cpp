#include "rzxpch.h"
#include "RZScene.h"

#include "Razix/Scene/RZComponents.h"
#include "Razix/Scene/RZEntity.h"

namespace Razix {

    RZScene::RZScene(std::string sceneName)
        : m_SceneName(sceneName) { }

    RZEntity RZScene::createEntity(const std::string& name /*= std::string()*/)
    {
        RZEntity entity = { m_Registry.create(), this };
        // By default an entity has 3 components
        // 1. ID Component - Automatically allocates a UUID to the entity
        // 2. Tag component - Add a name/tag to the entity for human readable identification
        // 3. Transform
        entity.AddComponent<IDComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        entity.AddComponent<TransformComponent>();

        return entity;
    }

    void RZScene::destroyEntity(RZEntity entity)
    {
        m_Registry.destroy(entity);
    }
}