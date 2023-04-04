#pragma once

#include <cereal/archives/json.hpp>
#include <entt.hpp>

#include "Razix/Core/RZUUID.h"

#include "Razix/Scene/RZSceneCamera.h"

namespace Razix {

    class RZEntity;
    struct CameraComponent;

    namespace Graphics {
        class RZPipeline;
        class RZDescriptorSet;
    }

    /**
     * Scene contains entities that will be used by the engine for rendering and other runtime systems
     */
    class RAZIX_API RZScene
    {
    public:
        RZScene();
        RZScene(std::string sceneName);

        void updatePhysics();
        /* Draws the Scene using the current bound command buffer, we need to set the Descriptor Sets, Being rendering onto the CmdBuffer and the Pipeline for this to work */
        void drawScene(Graphics::RZPipeline* pipeline, Graphics::RZDescriptorSet* frameDataSet, Graphics::RZDescriptorSet* sceneLightsSet, std::vector<Graphics::RZDescriptorSet*> userSets = {});

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

        /* Utility method to save the scene */
        void saveScene();
        /* Reloads the scene by re-reading the scene file and initializing everything from scratch */
        void reloadScene();

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
            archive(cereal::make_nvp("Total Entities", (u32) m_Registry.alive()));
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
        std::string    m_SceneName = "Razix Scene"; /* The name of the scene                        */
        std::string    m_ScenePath;                 /* The Path of the scene file                   */
        friend class RZEntity;

    private:
        template<typename T>
        void OnComponentAdded(RZEntity entity, T& component, bool enable);
    };
}    // namespace Razix