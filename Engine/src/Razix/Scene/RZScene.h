#pragma once

#include <cereal/archives/json.hpp>
#include <entt.hpp>

#include "Razix/Core/RZUUID.h"

#include "Razix/Graphics/RHI/API/RZAPIHandles.h"

#include "Razix/Scene/RZSceneCamera.h"

namespace Razix {

    class RZEntity;
    struct CameraComponent;

    namespace Graphics {
        class RZPipeline;
        class RZDescriptorSet;
    }    // namespace Graphics

    using RZDescriptorSets = std::vector<Graphics::RZDescriptorSet*>;

    enum class SceneDrawGeometryMode
    {
        SceneGeometry, /* Draws the scene geometry using the scene graph       */
        Cubemap,       /* Draws a single cube for cubemap projection           */
        ScreenQuad,    /* Draws a full screen quad for Post processing etc.    */
        Quad,          /* Draws a quad for GS manipulation                     */
        UI,            /* Draws UI elements                                    */
        Custom,        /* Issues custom draw calls instead                     */
    };

    static std::map<std::string, Razix::SceneDrawGeometryMode> SceneGeometryModeStringMap = {
        {"SceneGeometry", Razix::SceneDrawGeometryMode::SceneGeometry},
        {"Cubemap", Razix::SceneDrawGeometryMode::Cubemap},
        {"ScreenQuad", Razix::SceneDrawGeometryMode::ScreenQuad},
        {"Quad", Razix::SceneDrawGeometryMode::Quad},
        {"UI", Razix::SceneDrawGeometryMode::UI},
        {"Custom", Razix::SceneDrawGeometryMode::Custom}};

    struct SceneDrawParams
    {
        SceneDrawGeometryMode geometryMode                 = SceneDrawGeometryMode::SceneGeometry;
        bool                  enableMaterials              = true;
        bool                  enableLights                 = true;
        bool                  enableFrameData              = true;
        bool                  enableBindlessTextures       = false;
        RZDescriptorSets      userSets                     = {};
        void*                 overridePushConstantData     = nullptr;
        u32                   overridePushConstantDataSize = 0;
        // TODO: Add support for Pixel PC data
    };

    /**
     * Scene contains entities that will be used by the engine for rendering and other runtime systems
     */
    class RAZIX_API RZScene
    {
    public:
        RZScene();
        RZScene(std::string sceneName);

        void updatePhysics();
        /* Updates the Scene Graph Hierarchy transformations */
        void update();
        void updateTransform(entt::entity entity);
        /* Draws the Scene using the current bound command buffer, we need to set the Descriptor Sets, Being rendering onto the CmdBuffer and the Pipeline for this to work */
        void drawScene(Graphics::RZPipelineHandle pipeline, SceneDrawParams sceneDrawParams = {});

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

#pragma warning(push)
#pragma warning(disable : 4267)
            auto view = m_Registry.view<T>();
            for (auto& entity: view)
                components.push_back(view.get<T>(entity));

#pragma warning(pop)

            return components;
        }

        /* Gets the name of the scene */
        RAZIX_INLINE const std::string& getSceneName() const { return m_SceneName; }
        /* Gets the entity registry of the current scene */
        RAZIX_INLINE entt::registry& getRegistry() { return m_Registry; }

        RAZIX_DEFINE_SAVE_LOAD

    private:
        entt::registry m_Registry;                  /* Scene registry for storing all the entities  */
        RZUUID         m_SceneUUID;                 /* The UUID to identify the scene uniquely      */
        std::string    m_SceneName = "Razix Scene"; /* The name of the scene                        */
        std::string    m_ScenePath;                 /* The Path of the scene file                   */
        friend class RZEntity;

    private:
        template<typename T>
        void OnComponentAdded(RZEntity entity, T& component);
    };
}    // namespace Razix