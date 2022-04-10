#pragma once

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    
    // Forward declaration
    class RZScene;

    /**
     * The Scene Manager loads the scenes and manages switching mechanism and other scene related functionalities
     */
    class RAZIX_API RZSceneManager :  public RZSingleton<RZSceneManager>
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
        void enqueueSceneFromFile(const std::string& sceneFilePath);

        /**
         * Loads the next scene in the queue or first scene if at the end
         */
        void loadScene();

        /**
         * Loads the scene with the index as they were stored in the scene queue (starting from 0)
         */
        void loadScene(uint32_t index);

        /**
         * Loads the scene with the given name
         */
        void loadScene(const std::string& sceneName);

        /**
         * Applies the necessary settings for the current scene that is being presented/loaded
         */
        void loadSceneSettings();

        /**
         * Loads/re-loads all the scenes that are stored through file paths and load lists
         */
        void loadAllScenes();

        void saveAllScenes();

        const std::vector<std::string>& getSceneFilePaths() const { return m_LoadedSceneFilePaths; }

        RAZIX_INLINE uint32_t getCurrentSceneIndex() const { return m_CurrentSceneIdx; }
        RAZIX_INLINE RZScene* getCurrentScene() { return m_CurrentScene; }
        void saveCurrentScene();

    private:
        uint32_t                    m_CurrentSceneIdx           = 0;            /* The current index of the scene that is being presented by the engine */
        RZScene*                    m_CurrentScene              = nullptr;      /* The reference to the currently presented scene                       */
        std::vector<RZScene*>       m_LoadedScenes;                             /* The queue of all the loaded scene in memory ready to be switched     */
        std::vector<std::string>    m_LoadedSceneFilePaths;                     /* List of files paths of the scenes that were loaded into memory       */
        bool                        m_IsSwitchingScenes         = false;        /* Is the scene switching is in progress or not                         */
        uint32_t                    m_QueuedSceneIndexToLoad    = -1;           /* The next scene index to which it will switch to                      */
    };
}

