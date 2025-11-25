#pragma once

#include "Razix/Core/Utils/TRZSingleton.h"

namespace Razix {

    // Forward declaration
    class RZScene;

    /**
     * The Scene Manager loads the scenes and manages switching mechanism and other scene related functionalities
     */
    class RAZIX_API RZSceneManager : public RZSingleton<RZSceneManager>
    {
    public:
        /* Initializes the VFS */
        void StartUp();
        /* Shuts down the VFSand releases any resources hold by this */
        void ShutDown();

        /**
         * Adds a scene on the queue to be presented when requested
         * 
         * @param scene The scene to add to the queue
         */
        void enqueScene(RZScene* scene);
        /**
         * Adds a scene from the file on the queue and loads it into the memory to be presented when requested
         * 
         * @param sceneFilePath The scene file that will be loaded into the memory
         */
        void enqueueSceneFromFile(const RZString& sceneFilePath);

        /**
         * Loads the next scene in the queue or first scene if at the end
         */
        void loadScene();

        /**
         * Loads the scene with the index as they were stored in the scene queue (starting from 0)
         */
        void loadScene(u32 index);

        /**
         * Loads the scene with the given name
         */
        void loadScene(const RZString& sceneName);

        /**
         * Applies the necessary settings for the current scene that is being presented/loaded
         */
        void loadSceneSettings();

        /**
         * Loads/re-loads all the scenes that are stored through file paths and load lists
         */
        void loadAllScenes();

        void saveAllScenes();

        void destroyAllScenes();

        const RZDynamicArray<RZString>& getSceneFilePaths() const { return m_LoadedSceneFilePaths; }

        RAZIX_INLINE u32      getCurrentSceneIndex() const { return m_CurrentSceneIdx; }
        RAZIX_INLINE RZScene* getCurrentSceneMutablePtr() const { return m_CurrentScene; }
        void                  saveCurrentScene();

    private:
        u32                      m_CurrentSceneIdx = 0;            /* The current index of the scene that is being presented by the engine */
        RZScene*                 m_CurrentScene    = nullptr;      /* The reference to the currently presented scene                       */
        RZDynamicArray<RZScene*> m_LoadedScenes;                   /* The queue of all the loaded scene in memory ready to be switched     */
        RZDynamicArray<RZString> m_LoadedSceneFilePaths;           /* List of files paths of the scenes that were loaded into memory       */
        bool                     m_IsSwitchingScenes      = false; /* Is the scene switching is in progress or not                         */
        u32                      m_QueuedSceneIndexToLoad = -1;    /* The next scene index to which it will switch to                      */
        friend class RZEntity;
    };
}    // namespace Razix
