#include "rzxpch.h"
#include "Application.h"

// ---------- Engine ----------
#include "Razix/Core/Engine.h"
// ----------------------------

#include "Razix/Core/RazixVersion.h"

#include "Razix/Core/OS/VFS.h"
#include "Razix/Core/OS/RazixInput.h"

#include "Razix/Events/ApplicationEvent.h"

#include "Razix/Graphics/API/GraphicsContext.h"

#include <glad/glad.h>

namespace Razix
{
    Application* Application::s_AppInstance = nullptr;

    Application::Application(const std::string& projectRoot, const std::string& appName /*= "Razix App"*/) : m_AppName(appName), m_Timestep(Timestep(0.0f))
    {
        RAZIX_CORE_ASSERT(!s_AppInstance, "Application already exists!");
        s_AppInstance = this;

        // Set the Application root path and Load the project settings
        const std::string& razixRoot = STRINGIZE(RAZIX_ROOT_DIR);
        // Path to the Project path (*.razixproject)
        // TODO: Since the Engine will be installed elsewhere and and Project will be else where this logic has to be re-factored
        m_AppFilePath = razixRoot + projectRoot + appName + std::string(".razixproject");
        RAZIX_CORE_TRACE("Application file path : {0}", m_AppFilePath);

        // Mount the VFS paths
        // TODO: Move this to the sandbox later or mount all/new paths dynamically from the project root path for the asset browser 
        VFS::Get().Mount("Assets", razixRoot + projectRoot + std::string("Assets"));
        VFS::Get().Mount("Meshes", razixRoot + projectRoot + std::string("Assets/Meshes"));
        VFS::Get().Mount("Scenes", razixRoot + projectRoot + std::string("Assets/Scenes"));
        VFS::Get().Mount("Scripts", razixRoot + projectRoot + std::string("Assets/Scripts"));
        VFS::Get().Mount("Sounds", razixRoot + projectRoot + std::string("Assets/Sounds"));
        VFS::Get().Mount("Textures", razixRoot + projectRoot + std::string("Assets/Textures"));
    }

    void Application::Init() {
        // Load the De-serialized data from the project file or use the command line argument to open the file
        // TODO: Add verification for Engine and Project Version
        std::ifstream AppStream;
        if (Engine::Get().commandLineParser.isSet("project filename")) {
            std::string fullPath = Engine::Get().commandLineParser.getValueAsString("project filename");
            RAZIX_CORE_TRACE("Command line filename : {0}", fullPath);
            AppStream.open(fullPath, std::ifstream::in);
            m_AppFilePath = fullPath;// .substr(0, fullPath.find_last_of("\\/"));
        }
        else
            AppStream.open(m_AppFilePath, std::ifstream::in);

        // Check the command line arguments for the rendering api
        if (Engine::Get().commandLineParser.isSet("rendering api"))
            Graphics::GraphicsContext::SetRenderAPI((Graphics::RenderAPI) Engine::Get().commandLineParser.getValueAsInt("project filename"));

        // De-serialize the application
        if (AppStream.is_open()) {
            RAZIX_CORE_TRACE("Loading project file...");
            cereal::JSONInputArchive inputArchive(AppStream);
            inputArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }



        // Override the Graphics API here! for testing
        Razix::Graphics::GraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);




        // The Razix Application Signature Name is generated here and passed to the window
        // TODO: Add render API being used to the Signature dynamically
        std::string SignatureTitle = m_AppName + " | " + "Razix Engine" + " - " + Razix::RazixVersion.GetVersionString() + " " + "[" + Razix::RazixVersion.GetReleaseStageString() + "]" + " " + "<" + Graphics::GraphicsContext::GetRenderAPIString() + ">" + " | " + " " + STRINGIZE(RAZIX_BUILD_CONFIG);

        // Create the timer
        m_Timer = CreateUniqueRef<Timer>();

        // Set the window properties and create the timer
        m_WindowProperties.Title = SignatureTitle;

        // Create the Window
        m_Window = UniqueRef<Window>(Window::Create(m_WindowProperties));
        m_Window->SetEventCallback(RAZIX_BIND_CB_EVENT_FN(Application::OnEvent));

        // Creating the Graphics Context
        Graphics::GraphicsContext::Create(m_WindowProperties, m_Window.get());
        Graphics::GraphicsContext::GetContext()->Init();

        // Create a default project file file if nothing exists
        if (!AppStream.is_open()) {
            RAZIX_CORE_ERROR("Project File does not exist!");
            std::ofstream opAppStream(m_AppFilePath);
            cereal::JSONOutputArchive defArchive(opAppStream);
            RAZIX_CORE_TRACE("Creating a default Project file...");
            defArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }

        // Convert the app to loaded state
        m_CurrentState = AppState::Running;

        // Enable V-Sync
        m_Window->SetVSync(true);
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
        return true;
    }

    void Application::Run()
    {
        OnStart();
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
        //RAZIX_CORE_TRACE("Time steps : {0} ms", stats.FrameTime);

        // Poll for Input events
        m_Window->ProcessInput();

        if (RazixInput::IsKeyPressed(Razix::KeyCode::Key::Escape))
            m_CurrentState = AppState::Closing;
        // Early close if the escape key is pressed or close button is pressed
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
        Graphics::GraphicsContext::GetContext()->SwapBuffers();

        if (now - m_SecondTimer > 1.0f)
        {
            m_SecondTimer += 1.0f;

            stats.FramesPerSecond = m_Frames;
            stats.UpdatesPerSecond = m_Updates;
            //RAZIX_CORE_TRACE("FPS : {0}", stats.FramesPerSecond);
            //RAZIX_CORE_TRACE("UPS : {0} ms", stats.UpdatesPerSecond);

            m_Frames = 0;
            m_Updates = 0;
        }

        return m_CurrentState != AppState::Closing;
    }

    void Application::OnStart() {
        if (Razix::Graphics::GraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
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

            glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
        }
    }

    void Application::OnUpdate(const Timestep& dt) {

    }

    void Application::OnRender() {
        if (Razix::Graphics::GraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            glClear(GL_COLOR_BUFFER_BIT);
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }

    void Application::Quit()
    {

        // TODO: Release the Graphics context
        Graphics::GraphicsContext::Release();

        // Save the app data before closing
        RAZIX_CORE_WARN("Saving project...");
        std::ofstream opAppStream(m_AppFilePath);
        cereal::JSONOutputArchive saveArchive(opAppStream);
        saveArchive(cereal::make_nvp("Razix Application", *s_AppInstance));

        RAZIX_CORE_ERROR("Closing Application!");
    }
}