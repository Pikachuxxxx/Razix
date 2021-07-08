#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/OS/Window.h"
#include "Razix/Core/Reference.h"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/KeyEvent.h"
#include "Razix/Events/MouseEvent.h"

#include "Razix/Utilities/Timestep.h"
#include "Razix/Utilities/Timer.h"

namespace Razix
{
	/// <summary>
	/// Determines the state of the application
	/// </summary>
    enum class AppState
    {
        Running,
        Loading,
        Closing
    };

    /// <summary>
    /// Creates an Razix Application (Used as the base for Editor , Sandbox and Game Project)
    /// </summary>
    class RAZIX_API Application
    {
    public:
        /// <summary>
        /// Creates a Razix Application 
        /// </summary>
        /// <param name="projectRoot"> The root location of the application </param>
        /// <param name="appName"> The name of the Razix application </param>
        Application(const std::string& projectRoot , const std::string& appName = "Razix App");

        /// Simple Virtual destructor
        virtual ~Application() {}

        /// <summary>
        /// Begins the Engine Runtime systems
        /// </summary>
        void Run();

		bool OnFrame();

		void OnRender();

		void OnUpdate(const Timestep& dt);

        void Quit();

        /// <summary>
        /// Gets the Events from the engine, window and OS
        /// </summary>
        /// <param name="event"> The event reveived from all the sub-ssytems </param>
        void OnEvent(Event& event);

        /// <summary>
        /// Gets the reference to the application window
        /// </summary>
        inline Window& GetWindow() { return *m_Window; }

        /// <summary>
        /// Reference to the Application instance
        /// </summary>
        inline static Application& GetApplication() { return *sInstance; }
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
    private:
        // TODO: Remove this!
        unsigned int m_VAO, m_VBO, m_IBO;

        /// The name of the application
        std::string             m_AppName;
        /// The path of the Razix Project file (*.razixproject)
        std::string             m_AppFilePath;
        /// The number of frames per second
		uint32_t                m_Frames = 0;
        /// The number of updated per second
		uint32_t                m_Updates = 0;
        /// The timer used to calculate the delta time and timesteps
        Scope<Timer>             m_Timer;
        /// A secondary timer to
        float                   m_SecondTimer = 0;
        /// The timesteps taken to update the application
        Timestep                m_Timestep;
        /// The window that will be used to view graphics
        Ref<Window>             m_Window;
        /// The current state of the application
        AppState                m_CurrentState = AppState::Loading;
    private:
        /// The singleton instance of the application
        static Application*     sInstance;
    };

    // To be defined on the CLIENT side to create the application
    //
    // What this means is that the Entry point thinks the engine will define this for sure.
    // In fact it does by just forward declaring, but who actually defines it?
    // Now the engine forces the client to implement this according to their needs. 
    // [Application(forward declaration)-->Entry Point(extern declaration)-->CLIENT(definition)]
    /// Defined by the client to create the application definition
    Application* CreateApplication();
}

 