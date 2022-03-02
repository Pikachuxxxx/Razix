#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Scene/RZComponents.h"

#include <entt.hpp>
#include <cereal/archives/json.hpp>

namespace Razix {

    class RZEntity;

    /**
     * Scene contains entities that will be used by the engine for rendering and other runtime systems
     */
    class RAZIX_API RZScene
    {
    public:
        RZScene();
        RZScene(std::string sceneName);

        /**
         * Create a Razix Entity in the scene
         * 
         * @param name The name of the entity
         * @returns The freshly created razix entity
         */
        RZEntity createEntity(const std::string& name = std::string());
        /**
         * Detroys the entity from the scene
         * 
         * @param The entity to destroy
         */
        void destroyEntity(RZEntity entity);

        /* Serialize the scene to the given file path */
        void SerialiseScene(const std::string& filePath);
        /* De-Serialize the scene from the given file path */
        void DeSerialiseScene(const std::string& filePath);

        /**
         * Gets the scene camera component with which the world is rendered (if exists)
         */
        CameraComponent& getSceneCamera();

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

        /* Gets the name of the scene */
        RAZIX_INLINE const std::string& getSceneName() const { return m_SceneName; }
        /* Gets the entity registry of the current scene */
        RAZIX_INLINE entt::registry& getRegistry() { return m_Registry; }

        // Serialization Functions
        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("uuid", m_SceneUUID));
            archive(cereal::make_nvp("SceneName", m_SceneName));
        }

        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("uuid", m_SceneUUID));
            archive(cereal::make_nvp("SceneName", m_SceneName));
        }

    private:
        RZUUID              m_SceneUUID;                    /* The UUID to identify the scene uniquely      */
        entt::registry      m_Registry;                     /* Scene registry for storing all the entities  */
        std::string         m_SceneName = "razix scene";    /* The name of the scene                        */

        friend class RZEntity;

    private:
        template<typename T>
        void OnComponentAdded(RZEntity entity, T& component, bool enable);
    };
}