// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSceneManager.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZSplashScreen.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZStringUtilities.h"

namespace Razix {

    void RZSceneManager::StartUp()
    {
        // Instance is automatically created once the system is Started Up
        RAZIX_CORE_INFO("[Scene Manager] Starting Up Scene Manager");
        //Razix::RZSplashScreen::Get().setLogString("STATIC_INITIALIZATION : Starting Scene Manager...");
        Razix::RZSplashScreen::Get().setLogString("Starting Scene Manager...");
        // TODO: Load scenes from memory --> Static initialization using macros while building it as a game
    }

    void RZSceneManager::ShutDown()
    {
        // TODO: Unload everything from the memory and shut things property
        RAZIX_CORE_ERROR("[Scene Manager] Shutting Down Scene Manager");
    }

    void RZSceneManager::enqueScene(RZScene* scene)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        m_LoadedScenes.push_back(scene);
        // TODO: serialize this scene and add it's path to the list
        std::string scenePath = "//Scenes/" + scene->getSceneName() + ".rzscn";
        m_LoadedSceneFilePaths.push_back(scenePath);
        scene->serialiseScene(scenePath);
        RAZIX_CORE_INFO("[Scene Manager] - Enqueued Scene Index : {0}, Name : {1}", scene->getSceneName(), m_QueuedSceneIndexToLoad);
    }

    void RZSceneManager::enqueueSceneFromFile(const std::string& sceneFilePath)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        m_LoadedSceneFilePaths.push_back(sceneFilePath);

        auto name  = Utilities::RemoveFilePathExtension(Utilities::GetFileName(sceneFilePath));
        auto scene = new RZScene(name);
        // Once loaded to memory De-Serialize it
        scene->deSerialiseScene(sceneFilePath);
        m_LoadedScenes.push_back(scene);
        RAZIX_CORE_INFO("[Scene Manager] - Enqueued Scene Index : {0}, Name : {1}", scene->getSceneName(), m_QueuedSceneIndexToLoad);
    }

    void RZSceneManager::loadScene()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        loadScene((m_CurrentSceneIdx + 1) % m_LoadedScenes.size());
    }

    void RZSceneManager::loadScene(u32 index)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        if (m_LoadedSceneFilePaths.size() == 0) {
            // Create a default scene
            RZScene* defaultScene = new RZScene("default Scene");
            enqueScene(defaultScene);
            loadSceneSettings();
            // Add a camera and a cube just like Blender (+ a sun light when it's available)
            RZEntity camera = m_CurrentScene->createEntity("Camera");
            camera.AddComponent<CameraComponent>();
            if (camera.HasComponent<CameraComponent>()) {
                CameraComponent& cc = camera.GetComponent<CameraComponent>();
                cc.Camera.setViewportSize(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());
            }
        }

        m_QueuedSceneIndexToLoad = index;
        m_IsSwitchingScenes      = true;

        loadSceneSettings();
    }

    void RZSceneManager::loadScene(const std::string& sceneName)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        bool found          = false;
        m_IsSwitchingScenes = true;
        u32 idx        = 0;
        for (u32 i = 0; !found && i < m_LoadedScenes.size(); ++i) {
            if (m_LoadedScenes[i]->getSceneName() == sceneName) {
                found = true;
                idx   = i;
                break;
            }
        }

        if (found)
            loadScene(idx);
        else
            RAZIX_CORE_INFO("[Scene Manager] - Scene [{0}] not Found", sceneName.c_str());
    }

    void RZSceneManager::loadSceneSettings()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        if (m_IsSwitchingScenes == false) {
            if (m_CurrentScene)
                return;

            m_QueuedSceneIndexToLoad = 0;
        }

        if (m_QueuedSceneIndexToLoad < 0 || m_QueuedSceneIndexToLoad >= static_cast<int>(m_LoadedScenes.size())) {
            RAZIX_CORE_ERROR("[Scene Manager] - Invalid Scene Index : {0}", m_QueuedSceneIndexToLoad);
            m_QueuedSceneIndexToLoad = 0;
        }

        // Exit the old scene and load the new one
        if (m_CurrentScene) {
            RAZIX_CORE_INFO("[Scene Manager] - Exiting scene : {0}", m_CurrentScene->getSceneName());

            // Perform any physics pauses and other systems pause/exit
            // Initialize scene exit mechanism and clean up and unload it from memory
        }

        m_CurrentSceneIdx = m_QueuedSceneIndexToLoad;
        m_CurrentScene    = m_LoadedScenes[m_QueuedSceneIndexToLoad];

        // Load and resume other paused/exited engine systems related to scene functionality\

        // Deserialize the scene
        //std::string physicalPath;
        //if (Razix::RZVirtualFileSystem::Get().resolvePhysicalPath("//Scenes/" + m_CurrentScene->getSceneName() + ".rzscn", physicalPath)) {
        //}
        //m_CurrentScene->deSerialiseScene(physicalPath);

        RAZIX_CORE_INFO("[Scene Manager] - Scene switched to : {0}", m_CurrentScene->getSceneName().c_str());

        m_IsSwitchingScenes = false;
    }

    void RZSceneManager::loadAllScenes()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        for (auto& filePath: m_LoadedSceneFilePaths) {
            loadScene(filePath);
        }
    }

    void RZSceneManager::saveAllScenes()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        for (sz i = 0; i < m_LoadedSceneFilePaths.size(); i++) {
            auto path  = m_LoadedSceneFilePaths[i];
            auto scene = m_LoadedScenes[i];
            scene->serialiseScene(path);
        }
    }

    void RZSceneManager::destroyAllScenes()
    {
        for (auto scene: m_LoadedScenes)
            scene->Destroy();
    }

    void RZSceneManager::saveCurrentScene()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        // TODO: This isn't right
        std::string scenePath = "//Scenes/" + m_CurrentScene->getSceneName() + ".rzscn";
        m_CurrentScene->serialiseScene(scenePath);
    }

}    // namespace Razix