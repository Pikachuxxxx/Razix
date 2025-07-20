#ifndef APPLICATION_H
#define APPLICATION_H

#include "Razix/Core/OS/RZWindow.h"
#include "Razix/Core/Profiling/RZProfiling.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZSTL/smart_pointers.h"
#include "Razix/Core/UUID/RZUUID.h"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/RZKeyEvent.h"
#include "Razix/Events/RZMouseEvent.h"

#include "Razix/Scene/RZEntity.h"

#include "Razix/Utilities/RZTimer.h"
#include "Razix/Utilities/RZTimestep.h"
#include "Razix/Utilities/TRZSingleton.h"

// Cereal
#pragma warning(push, 0)
#include <cereal/types/vector.hpp>
#pragma warning(pop)

//! Some style guide rules are waved off for RZApplication class

namespace Guizmo {
    enum OPERATION
    {
        TRANSLATE,
        ROTATE,
        SCALE
    };

    enum MODE
    {
        LOCAL,
        WORLD
    };
}    // namespace Guizmo

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
        kGame,
        kTool,
        // Networking mode
        kToolServer,    // Serves data to the engine via network to edit runtime data
        kGameClient,    // Razix app where the game is in client mode, can talk to another game server or tool server
        kGameServer     // either used as standalone server or central server
    };

    // TODO!!!: Add budget info to RZApplication and RZFrameGraph

    /* Creates an Razix Application (Used as the base for Editor, Sandbox and Game Project) */
    class RAZIX_API RZApplication
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
        RZApplication(const std::string& projectRoot, const std::string& appName = "Razix App");
        virtual ~RZApplication() {}

        void Init();
        void Begin();
        void Quit();
        void SaveApp();
        bool RenderFrame();

        virtual void OnStart() {}
        virtual void OnUpdate(const RZTimestep& dt) {}
        virtual void OnRender() {}
        virtual void OnQuit() {}
        virtual void OnImGui() {}
        virtual void OnResize(u32 width, u32 height) {}

        inline static RZApplication& Get() { return *s_AppInstance; }
        inline RZWindow*             getWindow() { return m_Window; }
        inline float2                getWindowSize() const { return float2(m_Window->getWidth(), m_Window->getHeight()); }
        inline std::string           getAppName() const { return m_ProjectName; }
        inline std::string           getProjectRoot() const { return m_ProjectPath; }
        inline void                  setProjectRoot(const std::string& projPath) { m_ProjectPath = projPath; }
        inline WindowProperties&     getWindowProps() { return m_WindowProperties; }
        inline RZTimer               getTimer() const { return *m_Timer.get(); }
        inline RZUUID                getProjectUUID() const { return m_ProjectID; }
        inline void                  setViewportWindow(RZWindow* viewportWindow) { m_Window = viewportWindow; }
        inline AppType               getAppType() const { return m_appType; }
        inline void                  setAppType(AppType appType) { m_appType = appType; }
        inline void                  disableGuizmoEditing() { m_EnableGuizmoEditing = false; }
        inline void                  setGuizmoOperation(Guizmo::OPERATION operation) { m_GuizmoOperation = operation; }
        inline void                  setGuizmoMode(Guizmo::MODE mode) { m_GuizmoMode = mode; }
        inline void                  setGuizmoSnapAmount(f32 snapAmount) { m_GuizmoSnapAmount = snapAmount; }
        inline const AppState&       getAppState() const { return m_CurrentState; }
        inline void                  setAppState(AppState state) { m_CurrentState = state; }

        // Application Save and Load Functions
        RAZIX_DEFINE_SAVE_LOAD

    private:
        static RZApplication* s_AppInstance;

        AppState                  m_CurrentState              = AppState::Loading;
        AppType                   m_appType                   = AppType::kGame;
        std::string               m_ProjectName               = "";
        std::string               m_ProjectPath               = "";
        u32                       m_RenderAPI                 = 1;    // Vulkan
        u32                       m_Frames                    = 0;
        u32                       m_Updates                   = 0;
        rzstl::UniqueRef<RZTimer> m_Timer                     = nullptr;
        f32                       m_TotalTimeElapsedInSeconds = 0;
        RZTimestep                m_FPSTimestep               = {};
        RZTimestep                m_UPSTimestep               = {};
        RZWindow*                 m_Window                    = nullptr;
        WindowProperties          m_WindowProperties          = {};
        RZUUID                    m_ProjectID                 = {};
        std::vector<std::string>  sceneFilePaths              = {};
        Guizmo::OPERATION         m_GuizmoOperation           = Guizmo::TRANSLATE;
        Guizmo::MODE              m_GuizmoMode                = Guizmo::WORLD;
        f32                       m_GuizmoSnapAmount          = 0.0f;
        bool                      m_EnableGuizmoEditing       = false;
        RZEventDispatcher         m_EventDispatcher           = {};

    private:
        RAZIX_NONCOPYABLE_CLASS(RZApplication);

        /* Starts the application */
        void Start();
        /* Updates the engine and application runtime systems */
        void Update(const RZTimestep& dt);
        /* Renders the application and Engine rendering commands */
        void Render();
        /* Used to render GUI */
        void RenderGUI();

        // Event callbacks
        void OnEvent(RZEvent& event);
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(RZWindowResizeEvent& e);
        bool OnMouseMoved(RZMouseMovedEvent& e);
        bool OnMouseButtonPressed(RZMouseButtonPressedEvent& e);
        bool OnMouseButtonReleased(RZMouseButtonReleasedEvent& e);
        bool OnKeyPress(RZKeyPressedEvent& e);
        bool OnKeyRelease(RZKeyReleasedEvent& e);

        void renderEngineImGuiElements();
        void renderRuntimeAssetsIconsOnImGui();
        void renderEngineStatsOnImGui();
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

#endif    // APPLICATION_H