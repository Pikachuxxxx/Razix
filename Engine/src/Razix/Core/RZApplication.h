#pragma once

#include "Razix/Core/OS/RZWindow.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZProfiling.h"
#include "Razix/Core/RZSmartPointers.h"
#include "Razix/Core/RZUUID.h"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/RZKeyEvent.h"
#include "Razix/Events/RZMouseEvent.h"

#include "Razix/Scene/RZEntity.h"

#include "Razix/Utilities/RZTimer.h"
#include "Razix/Utilities/RZTimestep.h"
#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Graphics/API/RZSwapchain.h"

#include "Razix/Graphics/Renderers/RZGridRenderer.h"


// glm
//#include <glm/glm.hpp>

// Cereal
#pragma warning(push, 0)
#include <cereal/types/vector.hpp>
#pragma warning(pop)

//! Some style guide rules are waved off for RZApplication class

namespace Razix::Graphics {
    class RZTexture2D;
}

namespace Razix {
    /* Determines the state of the application */
    enum class AppState
    {
        Running,
        Loading,
        Closing
    };

    /* The type of the application (Editor or Game) */
    enum class AppType
    {
        EDITOR,
        GAME
    };

    /* Creates an Razix Application (Used as the base for Editor, Sandbox and Game Project) */
    class RAZIX_API RZApplication : public RZRoot
    {
    public:
        static std::mutex              m;
        static std::condition_variable halt_execution;
        static bool                    ready_for_execution;

    public:
        /**
         * Creates a Razix Application
         * 
         * @param projectRoot   The root location of the application
         * @param appName       The name of the Razix application
         */
        RAZIX_CALL RZApplication(const std::string& projectRoot, const std::string& appName = "Razix App");
        /* Simple Virtual destructor */
        virtual ~RZApplication() {}

        // TODO: Have 2 inits ==> Static and Runtime
        /* Initializes the application and other runtime systems */
        void RAZIX_CALL Init();

        /* Starts the Engine Runtime systems */
        void RAZIX_CALL Run();

        /* Quits the application and releases any resources held by it */
        void Quit();

        /* Save the application */
        void SaveApp();

        /**
         * Called before the application starts rendering
         * This is called after the application and all the Engine systems are Initialized and just before OnRender() is called
         */
        virtual void RAZIX_CALL OnStart() {}
        /**
         * Updates the Engine systems for every engine timestep
         * 
         * @param dt The timestep taken for every frame
         */
        virtual void RAZIX_CALL OnUpdate(const RZTimestep& dt) {}
        /**
         * Calls the engine sub-systems to render the stuff calculated in OnFrame()
         * Begins the frame and submits the rendergraph to final display
         */
        virtual void RAZIX_CALL OnRender() {}
        /**
         * Called before the application is quit
         */
        virtual void RAZIX_CALL OnQuit() {}
        /**
         * Called for Rendering ImGui UI
         */
        virtual void RAZIX_CALL OnImGui() {}

        // Event callbacks for client
        virtual void RAZIX_CALL OnResize(uint32_t width, uint32_t height) {}

        /* Gets the static reference to the application instance */
        inline static RZApplication& RAZIX_CALL Get() { return *s_AppInstance; }

        /* Returns a reference to the application window */
        inline RZWindow* RAZIX_CALL getWindow() { return m_Window; }
        /* Gets the window size */
        inline glm::vec2 RAZIX_CALL getWindowSize() { return glm::vec2(m_Window->getWidth(), m_Window->getHeight()); }
        /* Returns a reference to the Application instance */
        inline std::string RAZIX_CALL getAppName() const { return m_AppName; }
        /* Gets the razixproject file path */
        inline std::string getAppFilePath() const { return m_AppFilePath; }
        /* Gets the window properties */
        inline WindowProperties& RAZIX_CALL getWindowProps() { return m_WindowProperties; }
        /* Gets the application render loop timer */
        inline RZTimer RAZIX_CALL getTimer() { return *m_Timer.get(); }

        void setViewportWindow(RZWindow* viewportWindow) { m_Window = viewportWindow; }
        void setViewportHWND(HWND hwnd) { viewportHWND = hwnd; }
        HWND getViewportHWND() { return viewportHWND; }

        void setProjectRoot(const std::string& projPath) { m_AppFilePath = projPath; }

        void setGuzimoForEntity(RZEntity& entity) { m_GuizmoEntity = entity; }

        inline AppType getAppType() { return m_appType; }
        void           setAppType(AppType appType) { m_appType = appType; }

        void setAppState(AppState state) { m_CurrentState = state; }
        /* Application Serialization */

        // Load mechanism for the RZApplication class
        // TODO: Make this look more neat
        template<class Archive>
        void load(Archive& archive)
        {
            std::string projectName;
            archive(cereal::make_nvp("Project Name", projectName));
            RAZIX_ASSERT_MESSAGE((projectName == m_AppName), "Project name doesn't match with Executable");
            /**
             * Currently the project name will be verified with the one given in the sandbox or game project
             * If it doesn't match it updates the project name, it's not necessary that the name must match the 
             * executable name, since the Editor can load any *.razixproject file, this should also mean that
             * it should be able to load any project name since the project name is always mutable just all it's properties
             * There's not enforcement on the project names and other properties, the Razix Editor Application 
             * and can load any thing as long it is supplies with the required data to
             */
            m_AppName = projectName;
            // TODO: Verify these two!
            //archive(cereal::make_nvp("Engine Version", Razix::RazixVersion.GetVersionString()));
            //archive(cereal::make_nvp("Project Version", 0));
            archive(cereal::make_nvp("Render API", m_RenderAPI));
            // Set the render API from the De-serialized data
            Graphics::RZGraphicsContext::SetRenderAPI((Graphics::RenderAPI) m_RenderAPI);
            uint32_t Width, Height;
            archive(cereal::make_nvp("Width", Width));
            archive(cereal::make_nvp("Height", Height));
            m_WindowProperties.Width  = Width;
            m_WindowProperties.Height = Height;

            // Extract the project UUID as as string and convert it back to the RZUUID
            std::string uuid_string;
            archive(cereal::make_nvp("Project ID", uuid_string));
            m_ProjectID = RZUUID::FromStrFactory(uuid_string);

            // Load the scenes from the project file for the engine to load and present
            RAZIX_CORE_TRACE("Loading Scenes...");
            archive(cereal::make_nvp("Scenes", sceneFilePaths));
            for (auto& sceneFilePath: sceneFilePaths)
                RAZIX_CORE_TRACE("\t scene : {0}", sceneFilePath);
        }

        // Save mechanism for the RZApplication class
        template<class Archive>
        void save(Archive& archive) const
        {
            RAZIX_TRACE("Window Resize override sandbox application! | W : {0}, H : {1}", m_Window->getWidth(), m_Window->getHeight());
            archive(cereal::make_nvp("Project Name", m_AppName));
            archive(cereal::make_nvp("Engine Version", Razix::RazixVersion.getVersionString()));
            archive(cereal::make_nvp("Project ID", m_ProjectID.prettyString()));
            archive(cereal::make_nvp("Render API", (uint32_t) Graphics::RZGraphicsContext::GetRenderAPI()));
            archive(cereal::make_nvp("Width", m_Window->getWidth()));
            archive(cereal::make_nvp("Height", m_Window->getHeight()));
            archive(cereal::make_nvp("Project Path", m_AppFilePath));    // Why am I even serializing this?

            auto& paths = Razix::RZEngine::Get().getSceneManager().getSceneFilePaths();

            std::vector<std::string> newPaths;
            for (auto& path: paths) {
                std::string newPath;
                RZVirtualFileSystem::Get().absolutePathToVFS(path, newPath);
                newPaths.push_back(path);
            }
            archive(cereal::make_nvp("Scenes", newPaths));
        }

    private:
        HWND                       viewportHWND;
        static RZApplication*      s_AppInstance;                      /* The singleton instance of the application                */
        AppState                   m_CurrentState = AppState::Loading; /* The current state of the application                     */
        AppType                    m_appType      = AppType::GAME;     /* The type of the application                              */
        std::string                m_AppName;                          /* The name of the application                              */
        std::string                m_AppFilePath;                      /* The path of the Razix Project file (*.razixproject)      */
        uint32_t                   m_RenderAPI;                        /* The Render API being used to render the application      */
        uint32_t                   m_Frames  = 0;                      /* The number of frames per second                          */
        uint32_t                   m_Updates = 0;                      /* The number of updated per second                         */
        UniqueRef<RZTimer>         m_Timer;                            /* The timer used to calculate the delta time and timesteps */
        float                      m_SecondTimer = 0;                  /* A secondary timer to count the ticks per second          */
        RZTimestep                 m_Timestep;                         /* The timesteps taken to update the application            */
        RZWindow*                  m_Window = nullptr;                 /* The window that will be used to view graphics            */
        WindowProperties           m_WindowProperties;                 /* The properties of the window to create with              */
        RZUUID                     m_ProjectID;                        /* Project ID is a UUID to uniquely identify project        */
        std::vector<std::string>   sceneFilePaths;
        RZEntity                   m_GuizmoEntity;

    private:
        /**
         * Renders the frame and displays the graphics and updates the window
         * 
         * @returns True, if the frame was successfully rendered by checking the state
         */
        bool RenderFrame();
        /* Starts the application */
        void Start();
        /* Updates the engine and application runtime systems */
        void Update(const RZTimestep& dt);
        /* Renders the application and Engine rendering commands */
        void Render();
        /* Used to render GUI */
        void RenderGUI();

        // Event callbacks
        /**
         * Gets the Events from the engine, window and OS
         * 
         * @param event  The event received from all the sub-systems
         */
        void OnEvent(RZEvent& event);

        /**
         * Called when the application is about to be closed
         *
         * @param e The window close event
         *
         * @returns True, if the window was closed successfully
         */
        bool OnWindowClose(WindowCloseEvent& e);
        /**
         * Called when the window is resized
         *
         * @param e The window resize event
         *
         * @returns True, if the window was resized successfully
         */
        virtual bool OnWindowResize(RZWindowResizeEvent& e);

        virtual bool OnMouseMoved(RZMouseMovedEvent& e);
        virtual bool OnMouseButtonPressed(RZMouseButtonPressedEvent& e);
        virtual bool OnMouseButtonReleased(RZMouseButtonReleasedEvent& e);
        virtual bool OnKeyPress(RZKeyPressedEvent& e);
        virtual bool OnKeyRelease(RZKeyReleasedEvent& e);

        RAZIX_NONCOPYABLE_CLASS(RZApplication);
    };

    /**
     * To be defined on the CLIENT side to create the application
     *
     * What this means is that the Entry point thinks the engine will define this for sure.
     * In fact it does by just forward declaring, but who actually defines it?
     * Now the engine forces the client to implement this according to their needs. 
     * [Application(forward declaration)-->Entry Point(extern declaration)-->CLIENT(definition)]
     * Defined by the client to create the application definition
     */
    RZApplication* CreateApplication(int argc, char** argv);
}    // namespace Razix
