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
         * @param projectRoot The root location of the application
         * @param appName The name of the Razix application
         */
        Application(const std::string& projectRoot , const std::string& appName = "Razix App");
        /* Simple Virtual destructor */
        virtual ~Application() {}

        /* Starts the Engine Runtime systems */
        void Run();

        /**
         * Renders the frame and displays the graphics and updates the window
         * 
         * @returns True, if the frame was successfully rendered by checking the state
         */
        bool OnFrame();
        /**
         * Calls the engine sub-systems to render the stuff calculated in OnFrame()
         * Begins the frame and submits the rendergraph to final display
         */
        void OnRender();
        /// <summary>
        /// Updates the Engine systems for every engine timestep
        /// </summary>
        /// <param name="dt"> The timestep taken for every frame </param>
        void OnUpdate(const Timestep& dt);
        /// <summary>
        /// Gets the Events from the engine, window and OS
        /// </summary>
        /// <param name="event"> The event reveived from all the sub-ssytems </param>
        void OnEvent(Event& event);

        /// <summary>
        /// Quits the application and releases any resources held by it
        /// </summary>
        void Quit();

        /// <summary>
        /// Gets the reference to the application window
        /// </summary>
        inline Window& GetWindow() { return *m_Window; }
        /// <summary>
        /// Reference to the Application instance
        /// </summary>
        inline static Application& GetApplication() { return *s_Instance; }

        // Serialization
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Engine Version", Razix::RazixVersion.GetVersionString()));
            archive(cereal::make_nvp("Project Version", 0));
            archive(cereal::make_nvp("Render API", m_RenderAPI));
            archive(cereal::make_nvp("Width", m_WindowProperties.Width));
            archive(cereal::make_nvp("Height", m_WindowProperties.Height));
            archive(cereal::make_nvp("Project Path", m_AppFilePath));
        }

    private:
        /// <summary>
        /// Called when the application was closed
        /// </summary>
        /// <param name="e"> The window close event </param>
        /// <returns> True, if the window was closed successfully </returns>
        bool OnWindowClose(WindowCloseEvent& e);
        /// <summary>
        /// Called when the window is resized
        /// </summary>
        /// <param name="e"> The window resize event </param>
        /// <returns> True, if the window was resized successfully </returns>
        bool OnWindowResize(WindowResizeEvent& e);

        NONCOPYABLE(Application);

    private:
        // TODO: Remove this!
        unsigned int m_VAO, m_VBO, m_IBO;

        std::string             m_AppName;                              /* The name of the application                              */
        std::string             m_AppFilePath;                          /* The path of the Razix Project file (*.razixproject)      */
        uint32_t                m_RenderAPI;                            /* The Render API being used to render the application      */
        uint32_t                m_Frames        = 0;                    /* The number of frames per second                          */
        uint32_t                m_Updates       = 0;                    /* The number of updated per second                         */
        UniqueRef<Timer>        m_Timer;                                /* The timer used to calculate the delta time and timesteps */
        float                   m_SecondTimer   = 0;                    /* A secondary timer to count the ticks per second          */
        Timestep                m_Timestep;                             /* The timesteps taken to update the application            */
        UniqueRef<Window>       m_Window;                               /* The window that will be used to view graphics            */
        AppState                m_CurrentState  = AppState::Loading;    /* The current state of the application                     */
        static Application*     s_Instance;                             /* The singleton instance of the application                */
        WindowProperties        m_WindowProperties;

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

 