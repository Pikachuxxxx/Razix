#include "rzxpch.h"
#include "RZScene.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Scene/RZComponents.h"
#include "Razix/Scene/RZEntity.h"

namespace Razix {

    RZScene::RZScene()
    {
        // TODO: Find a way to Add Camera skybox and environment settings
        // Default entities created in a scene 
        // 1. Default primary camera component entity
        // 2. Skybox entity with TODO components
        // 3. Environment settings entity with TODO components 
    }

    RZScene::RZScene(std::string sceneName)
        : m_SceneName(sceneName) 
    {

    }

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

    void RZScene::SerialiseScene(const std::string& filePath)
    {
        std::string fullFilePath;
        bool nope = RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, fullFilePath);
        RAZIX_CORE_WARN("Saving scene to - {0} ({1})", filePath, fullFilePath);

        if (!nope) {
            std::string path = "//Scenes/";
            bool nope = RZVirtualFileSystem::Get().resolvePhysicalPath(path, fullFilePath, true);
            fullFilePath += (m_SceneName + std::string(".rzscn"));
        }

        std::ofstream opAppStream(fullFilePath);
        cereal::JSONOutputArchive defArchive(opAppStream);
        defArchive(cereal::make_nvp("Razix Scene", *this));

        entt::snapshot{m_Registry}.entities(defArchive).component<RAZIX_COMPONENTS>(defArchive);
    }

    void RZScene::DeSerialiseScene(const std::string& filePath)
    {
        std::string fullFilePath;
        RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, fullFilePath);
        RAZIX_CORE_WARN("Saving scene to - {0} ({1})", filePath, fullFilePath);

        std::ifstream AppStream;
        AppStream.open(fullFilePath, std::ifstream::in);
        cereal::JSONInputArchive inputArchive(AppStream);
        inputArchive(cereal::make_nvp("Razix Scene", *this));
        //inputArchive(*this);
        entt::snapshot_loader{ m_Registry }.entities(inputArchive).component<RAZIX_COMPONENTS>(inputArchive);
    }

}