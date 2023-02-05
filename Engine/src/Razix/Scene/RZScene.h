#pragma once

#include <cereal/archives/json.hpp>
#include <entt.hpp>

#include "Razix/Core/RZUUID.h"

#include "Razix/Scene/RZSceneCamera.h"

namespace Razix {

    class RZEntity;
    struct CameraComponent;

    /**
     * Scene contains entities that will be used by the engine for rendering and other runtime systems
     */
    class RAZIX_API RZScene
    {
    public:
        RZScene();
        RZScene(std::string sceneName);

        void updatePhysics();
        void drawScene();

        void Destroy();

        /**
         * Create a Razix Entity in the scene
         * 
         * @param name The name of the entity
         * @returns The freshly created razix entity
         */
        RZEntity createEntity(const std::string& name = std::string());
        /**
         * Destroys the entity from the scene
         * 
         * @param The entity to destroy
         */
        void destroyEntity(RZEntity entity);

        /* Serialize the scene to the given file path */
        void serialiseScene(const std::string& filePath);
        /* De-Serialize the scene from the given file path */
        void deSerialiseScene(const std::string& filePath);

        /**
         * Gets the scene camera with which the world is rendered (if exists)
         */
        RZSceneCamera& getSceneCamera();

        // TODO: Get the reference to the components instead
        template<typename T>
        std::vector<T> GetComponentsOfType()
        {
            std::vector<T> components;
            
            auto view = m_Registry.view<T>();
            for (auto& entity: view)
                components.push_back(view.get<T>(entity));

            return components;
        }

        // TODO: Add ability to query for multiple components types followed by a comma
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
            archive(cereal::make_nvp("UUID", m_SceneUUID.prettyString()));
            archive(cereal::make_nvp("SceneName", m_SceneName));
            archive(cereal::make_nvp("Total Entities", (uint32_t) m_Registry.alive()));
        }

        template<class Archive>
        void load(Archive& archive)
        {
            std::string uuid_string;
            archive(cereal::make_nvp("UUID", uuid_string));
            m_SceneUUID = RZUUID::FromStrFactory(uuid_string);
            archive(cereal::make_nvp("SceneName", m_SceneName));
        }

    private:
        entt::registry m_Registry;                  /* Scene registry for storing all the entities  */
        RZUUID         m_SceneUUID;                 /* The UUID to identify the scene uniquely      */
        std::string    m_SceneName = "razix scene"; /* The name of the scene                        */

        friend class RZEntity;

    private:
        template<typename T>
        void OnComponentAdded(RZEntity entity, T& component, bool enable);
    };
}    // namespace Razix