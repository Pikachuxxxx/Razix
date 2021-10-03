#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/OS/Window.h"
#include "Razix/Core/SmartPointers.h"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/KeyEvent.h"
#include "Razix/Events/MouseEvent.h"

#include "Razix/Utilities/Timestep.h"
#include "Razix/Utilities/Timer.h"

// Cereal
#pragma warning(push, 0)
#include <cereal/archives/json.hpp>
#pragma warning(pop)
#include <fstream>

namespace Razix
{
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
    class RAZIX_API Application
    {
    public: 
        /**
         * Creates a Razix Application
         * 
         * @param projectRoot   The root location of the application
         * @param appName       The name of the Razix application
         */
        Application(const std::string& projectRoot , const std::string& appName = "Razix App");
        /* Simple Virtual destructor */
        virtual ~Application() {}

        void Init();

        /* Starts the Engine Runtime systems */
        void Run();

        /**
         * Renders the frame and displays the graphics and updates the window
         * 
         * @returns True, if the frame was successfully rendered by checking the state
         */
        bool OnFrame();
        /**
         * Updates the Engine systems for every engine timestep
         * 
         * @param dt The timestep taken for every frame
         */
        virtual void OnUpdate(const Timestep& dt);
        virtual void OnStart();
        /**
         * Calls the engine sub-systems to render the stuff calculated in OnFrame()
         * Begins the frame and submits the rendergraph to final display
         */
        virtual void OnRender();
        /**
         * Gets the Events from the engine, window and OS
         * 
         * @param event  The event received from all the sub-systems
         */
        void OnEvent(Event& event);

        /* Quits the application and releases any resources held by it */
        void Quit();
        
        /* Returns a reference to the application window */
        inline Window& GetWindow() { return *m_Window; }
        /* Returns a reference to the Application instance */
        inline static Application& Get() { return *s_AppInstance; }

        // Application Serialization
        template<class Archive>
        void load(Archive& archive) 
        {
            std::string projectName;
            archive(cereal::make_nvp("Project Name", projectName));
            RAZIX_ASSERT_MESSAGE((projectName == m_AppName), "Project name doesn't match with Executable");
            /**
             * Currently the project name will be verifies with the one given in the sandbox or game project
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
            Graphics::GraphicsContext::SetRenderAPI((Graphics::RenderAPI)m_RenderAPI);
            archive(cereal::make_nvp("Width", m_WindowProperties.Width));
            archive(cereal::make_nvp("Height", m_WindowProperties.Height));
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            Razix::Graphics::GraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::DIRECTX11);

            archive(cereal::make_nvp("Project Name", m_AppName));
            archive(cereal::make_nvp("Engine Version", Razix::RazixVersion.GetVersionString()));
            archive(cereal::make_nvp("Project Version", 0));
            archive(cereal::make_nvp("Render API", (uint32_t)Graphics::GraphicsContext::GetRenderAPI()));
            archive(cereal::make_nvp("Width", m_Window->GetWidth()));
            archive(cereal::make_nvp("Height", m_Window->GetHeight()));
            archive(cereal::make_nvp("Project Path", m_AppFilePath)); // Why am I even serializing this?
        }

    private:
        unsigned int m_VAO = 0, m_VBO, m_IBO;

        static Application*     s_AppInstance;                          /* The singleton instance of the application                */
        AppState                m_CurrentState  = AppState::Loading;    /* The current state of the application                     */
        std::string             m_AppName;                              /* The name of the application                              */
        std::string             m_AppFilePath;                          /* The path of the Razix Project file (*.razixproject)      */
        uint32_t                m_RenderAPI;                            /* The Render API being used to render the application      */
        uint32_t                m_Frames        = 0;                    /* The number of frames per second                          */
        uint32_t                m_Updates       = 0;                    /* The number of updated per second                         */
        UniqueRef<Timer>        m_Timer;                                /* The timer used to calculate the delta time and timesteps */
        float                   m_SecondTimer   = 0;                    /* A secondary timer to count the ticks per second          */
        Timestep                m_Timestep;                             /* The timesteps taken to update the application            */
        UniqueRef<Window>       m_Window;                               /* The window that will be used to view graphics            */
        WindowProperties        m_WindowProperties;                     /* The properties of the window to create with              */

    private:
        /**
         * Called when the application is about to be closed
         *
         * @param e The window close event
         *
         * @returns  True, if the window was closed successfully
         */
        bool OnWindowClose(WindowCloseEvent& e);
        /**
         * Called when the window is resized
         *
         * @param e The window resize event
         *
         * @returns  True, if the window was resized successfully
         */
        bool OnWindowResize(WindowResizeEvent& e);

        NONCOPYABLE(Application);
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
    Application* CreateApplication();
}

 