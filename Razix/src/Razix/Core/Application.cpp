#include "rzxpch.h"
#include "Application.h"

#include "Razix/Core/Engine.h"
#include "Razix/Core/RazixVersion.h"
#include "Razix/Core/OS/VFS.h"
#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Core/OS/Input.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace Razix
{
    Application* Application::sInstance = nullptr;

	Application::Application(const std::string& projectRoot, const std::string& appName /*= "Razix App"*/) : m_AppName(appName), m_Timestep(Timestep(0.0f))
    {
        RAZIX_CORE_ASSERT(!sInstance, "Application already exists!");
        sInstance = this;

        // Set the Application root path and mount the VFS paths
        const std::string& razixRoot = STRINGIZE(RAZIX_ROOT_DIR);
        // TODO: Still need to be implemented
        // Path to the Project path (*.razixproject)
        m_AppFilePath = razixRoot + projectRoot + appName + std::string(".razixproject");
        RAZIX_CORE_TRACE("Application file path : {0}", m_AppFilePath);

        // Mount the project asset paths
        // TODO: Move this to the sandbox later or mount all/new paths dynamically from the project root path for the asset browser 
		VFS::Get()->Mount("Assets", razixRoot + projectRoot + std::string("Assets"));
		VFS::Get()->Mount("Meshes", razixRoot + projectRoot + std::string("Assets/Meshes"));
		VFS::Get()->Mount("Scenes", razixRoot + projectRoot + std::string("Assets/Scenes"));
		VFS::Get()->Mount("Scripts", razixRoot + projectRoot + std::string("Assets/Scripts"));
		VFS::Get()->Mount("Sounds", razixRoot + projectRoot + std::string("Assets/Sounds"));
		VFS::Get()->Mount("Textures", razixRoot + projectRoot + std::string("Assets/Textures"));

        // The Razix Application Signature Name is generated here and passed to the window
        // TODO: Add render API being used to the Signature dynamically
        std::string SignatureTitle = appName + " | " + "Razix Engine" + " - " + Razix::RazixVersion.GetVersionString() + " " + "[" + Razix::RazixVersion.GetReleaseStage() + "]" + " " + "<" + "OpenGL" + ">" + " | " + " " + STRINGIZE(RAZIX_BUILD_CONFIG);

        // Create the timer
        m_Timer = std::make_unique<Timer>();

        // Set the window properties and create the timer
        WindowProperties windowProperties{};
		windowProperties.Title = SignatureTitle;
       
        m_Window = std::unique_ptr<Window>(Window::Create(windowProperties));
        m_Window->SetEventCallback(RAZIX_BIND_CB_EVENT_FN(Application::OnEvent));

        // Convert the app to loaded state
        m_CurrentState = AppState::Running;

        // TODO: Remove this test code!
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        float vertices[3 * 3] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f,
        };
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), nullptr);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, vertices, GL_STATIC_DRAW);
    }

	void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        // Window close event
        dispatcher.Dispatch<WindowCloseEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowClose));
        // Window resize event
        dispatcher.Dispatch<WindowResizeEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowResize));
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_CurrentState = AppState::Closing;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
		// TODO: Remove this test code!
        glViewport(0, 0, e.GetWidth(), e.GetHeight());
        return true;
    }

    void Application::Run()
    {
        while (OnFrame()) { }
        Quit();
    }

	bool Application::OnFrame()
	{
        // Calculate the delta time
        float now = m_Timer->GetElapsedS();
		auto& stats = Engine::Get().GetStatistics();
		m_Timestep.Update(now);

        // Update the stats
        stats.FrameTime = m_Timestep.GetTimestepMs();
        RAZIX_CORE_TRACE("Time steps : {0} ms", stats.FrameTime);

        // Poll for Input events
        m_Window->ProcessInput();

        // Early close if the escape key is pressed
        if (Input::IsKeyPressed(Razix::KeyCode::Key::Escape))
            m_CurrentState = AppState::Closing;

		if (m_CurrentState == AppState::Closing)
			return false;

        // Update the Engine systems
        OnUpdate(m_Timestep);
        m_Updates++;

        // Render the Graphics
        OnRender();
        m_Frames++;

        // Update the window (basically swap buffer)
        m_Window->OnWindowUpdate();

		if (now - m_SecondTimer > 1.0f)
		{
			m_SecondTimer += 1.0f;

			stats.FramesPerSecond = m_Frames;
			stats.UpdatesPerSecond = m_Updates;
			RAZIX_CORE_TRACE("FPS : {0} ms", stats.FramesPerSecond);
			RAZIX_CORE_TRACE("UPS : {0} ms", stats.UpdatesPerSecond);

			m_Frames = 0;
			m_Updates = 0;
		}

		return m_CurrentState != AppState::Closing;
	}

	void Application::OnRender()
	{
		// TODO: Remove this test code!
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void Application::OnUpdate(const Timestep& dt)
	{

	}

	void Application::Quit()
	{
        RAZIX_CORE_ERROR("Closing Application!");
	}

}
