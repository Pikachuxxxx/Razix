#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Scene/RZComponents.h"

#include <entt.hpp>
#include <cereal/archives/json.hpp>

namespace Razix {

    class RZEntity;

    /* Scene contains entities that will be used by the engine for rendering and other runtime systems */
    class RAZIX_API RZScene
    {
    public:
        RZScene();
        RZScene(std::string sceneName);

        RZEntity createEntity(const std::string& name = std::string());
        void destroyEntity(RZEntity entity);

        void SerialiseScene(const std::string& filePath);
        void DeSerialiseScene(const std::string& filePath);

        CameraComponent& GetSceneCamera()
        {
            auto& view = m_Registry.view<CameraComponent>();
            for (auto& entity : view)
                return view.get<CameraComponent>(entity);
        }

        // TODO: Add ability to query for multiple components types followed a comma
        // TODO: Get the reference to the components instead
        template<typename T>
        std::vector<T> GetComponentsOfType()
        {
            std::vector<T> components;
            auto& view = m_Registry.view<T>();
            for (auto& entity : view)
                components.push_back(view.get<T>(entity));

            return components;
        }

        //template<typename T>
        //std::vector<RZEntity> GetEntitiesWithComponentsOfType()
        //{
        //    std::vector<RZEntity> entities;
        //    auto view = m_Registry.view<T>();
        //    for (auto entity : view) {
        //        entities.push_back(RZEntity(entity, this));
        //    }
        //
        //    return entities;
        //}

        RAZIX_INLINE const std::string& getSceneName() const { return m_SceneName; }
        RAZIX_INLINE entt::registry& getRegistry() { return m_Registry; }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("SceneName", m_SceneName));
        }

        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("SceneName", m_SceneName));
        }

    private:
        entt::registry m_Registry;
        std::string m_SceneName = "default";

        friend class RZEntity;

    private:
        template<typename T>
        void OnComponentAdded(RZEntity entity, T& component, bool enable);
    };
}