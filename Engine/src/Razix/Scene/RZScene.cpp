// clang-format off
#include "rzxpch.h"
// clang-format on
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
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    }

    RZScene::RZScene(std::string sceneName)
        : m_SceneName(sceneName)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    }

    RZEntity RZScene::createEntity(const std::string& name /*= std::string()*/)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        RZEntity entity = {m_Registry.create(), this};
        // By default an entity has 3 components
        // 1. ID Component - Automatically allocates a UUID to the entity
        // 2. Tag component - Add a name/tag to the entity for human readable identification
        // 3. Transform
        entity.AddComponent<IDComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag   = name.empty() ? "Entity" : name;

        entity.AddComponent<TransformComponent>();

        return entity;
    }

    void RZScene::destroyEntity(RZEntity entity)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        m_Registry.destroy(entity);
    }

    void RZScene::serialiseScene(const std::string& filePath)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        std::string fullFilePath;
        bool        nope = RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, fullFilePath);
        RAZIX_CORE_WARN("[Scene] Saving scene to - {0} ({1})", filePath, fullFilePath);

        if (!nope) {
            std::string path = "//Scenes/";
            RZVirtualFileSystem::Get().resolvePhysicalPath(path, fullFilePath, true);
         
            fullFilePath += (m_SceneName + std::string(".rzscn"));
        }

        std::ofstream             opAppStream(fullFilePath);
        cereal::JSONOutputArchive defArchive(opAppStream);
        defArchive(cereal::make_nvp("Razix Scene", *this));

        entt::snapshot{m_Registry}.entities(defArchive).component<RAZIX_COMPONENTS>(defArchive);
    }

    void RZScene::deSerialiseScene(const std::string& filePath)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        std::string fullFilePath;
        RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, fullFilePath);
        RAZIX_CORE_WARN("[Scene] Loading scene from - {0} ({1})", filePath, fullFilePath);

        std::ifstream AppStream;
        AppStream.open(fullFilePath, std::ifstream::in);
        cereal::JSONInputArchive inputArchive(AppStream);
        inputArchive(cereal::make_nvp("Razix Scene", *this));
        //inputArchive(*this);
        entt::snapshot_loader{m_Registry}.entities(inputArchive).component<RAZIX_COMPONENTS>(inputArchive);
    }

    CameraComponent& RZScene::getSceneCamera()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        auto view = m_Registry.view<CameraComponent>();
        for (auto& entity: view)
            return view.get<CameraComponent>(entity);
    }

    template<typename T>
    void RZScene::OnComponentAdded(RZEntity entity, T& component, bool enable)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        if (enable)
            m_Registry.on_construct<T>().connect<&T::OnConstruct>();
        else
            m_Registry.on_construct<T>().disconnect<&T::OnConstruct>();
    }
}    // namespace Razix