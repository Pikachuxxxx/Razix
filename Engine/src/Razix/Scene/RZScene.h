#pragma once

#include <cereal/archives/json.hpp>
#include <entt.hpp>

#include "Razix/Core/Profiling/RZProfiling.h"
#include "Razix/Core/UUID/RZUUID.h"

#include "Razix/Gfx/Cameras/RZCamera3D.h"

namespace Razix {

    class RZEntity;
    struct CameraComponent;

    enum class SceneDrawGeometryMode
    {
        SceneGeometry, /* Draws the scene geometry using the scene graph       */
        Cubemap,       /* Draws a single cube for cubemap projection           */
        ScreenQuad,    /* Draws a full screen quad for Post processing etc.    */
        Quad,          /* Draws a quad for GS manipulation                     */
        UI,            /* Draws UI elements                                    */
        Custom,        /* Issues custom draw calls instead                     */
        COUNT
    };

    static const char* SceneDrawGeometryModeNames[] = {
        "SceneGeometry",
        "Cubemap",
        "ScreenQuad",
        "Quad",
        "UI",
        "Custom"};

    RAZIX_ENUM_NAMES_ASSERT(SceneDrawGeometryModeNames, SceneDrawGeometryMode);

    static RZHashMap<RZString, Razix::SceneDrawGeometryMode> SceneGeometryModeStringMap = {
        {"SceneGeometry", Razix::SceneDrawGeometryMode::SceneGeometry},
        {"Cubemap", Razix::SceneDrawGeometryMode::Cubemap},
        {"ScreenQuad", Razix::SceneDrawGeometryMode::ScreenQuad},
        {"Quad", Razix::SceneDrawGeometryMode::Quad},
        {"UI", Razix::SceneDrawGeometryMode::UI},
        {"Custom", Razix::SceneDrawGeometryMode::Custom}};

    /**
     * Scene contains entities that will be used by the engine for rendering and other runtime systems
     */
    class RAZIX_API RZScene
    {
    public:
        RZScene();
        RZScene(RZString sceneName);

        void updatePhysics();
        /* Updates the Scene Graph Hierarchy transformations */
        void update();
        void updateTransform(entt::entity entity);
        /* Draws the Scene using the current bound command buffer, we need to set the Descriptor Sets, Being rendering onto the CmdBuffer and the Pipeline for this to work */
        //void drawScene(Gfx::RZPipelineHandle pipeline, SceneDrawGeometryMode geometryMode = SceneDrawGeometryMode::SceneGeometry);
        /* Draw Decals into the GBuffer Render Targets */
        //void drawDecals(Gfx::RZPipelineHandle pipeline);

        void Destroy();

        /**
         * Create a Razix Entity in the scene
         * 
         * @param name The name of the entity
         * @returns The freshly created razix entity
         */
        RZEntity createEntity(const RZString& name = RZString());
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
        void serialiseScene(const RZString& filePath) {}
        /* De-Serialize the scene from the given file path */
        void deSerialiseScene(const RZString& filePath) {}

        /**
         * Gets the scene camera with which the world is rendered (if exists)
         */
        RZCamera3D& getSceneCamera()
        {
            return m_DefaultSceneCameraToAvoidCompileErrors;
        }

        // TODO: Get the reference to the components instead
        template<typename T>
        RZDynamicArray<T> GetComponentsOfType()
        {
            RZDynamicArray<T> components;

#pragma warning(push)
#pragma warning(disable : 4267)
            auto view = m_Registry.view<T>();
            for (auto& entity: view)
                components.push_back(view.template get<T>(entity));

#pragma warning(pop)

            return components;
        }

        /* Gets the name of the scene */
        RAZIX_INLINE const RZString& getSceneName() const { return m_SceneName; }
        /* Gets the entity registry of the current scene */
        RAZIX_INLINE entt::registry& getRegistry() { return m_Registry; }

        RAZIX_DEFINE_SAVE_LOAD

    private:
        entt::registry m_Registry;                  /* Scene registry for storing all the entities  */
        RZUUID         m_SceneUUID;                 /* The UUID to identify the scene uniquely      */
        RZString       m_SceneName = "Razix Scene"; /* The name of the scene                        */
        RZString       m_ScenePath;                 /* The Path of the scene file                   */
        u32            m_LastMeshesCount = 0;
        RZCamera3D  m_EditorModeCamera;
        //Gfx::RZMesh*   m_Cube = nullptr;
        RZCamera3D m_DefaultSceneCameraToAvoidCompileErrors;

        friend class RZEntity;

    private:
        template<typename T>
        void OnComponentAdded(RZEntity entity, T& component);
    };
}    // namespace Razix
