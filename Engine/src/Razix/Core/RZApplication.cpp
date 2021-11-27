#include "rzxpch.h"
#include "RZApplication.h"

// ---------- Engine ----------
#include "Razix/Core/RZEngine.h"
// ----------------------------

#include "Razix/Core/RazixVersion.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/OS/RZInput.h"

#include "Razix/Events/ApplicationEvent.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/API/RZTexture.h"
#include "Razix/Graphics/API/RZSwapchain.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include <glad/glad.h>
#endif

namespace Razix
{
    RZApplication* RZApplication::s_AppInstance = nullptr;

    RZApplication::RZApplication(const std::string& projectRoot, const std::string& appName /*= "Razix App"*/) : m_AppName(appName), m_Timestep(Timestep(0.0f))
    {
        // Create the application instance
        RAZIX_CORE_ASSERT(!s_AppInstance, "Application already exists!");
        s_AppInstance = this;

        // Set the Application root path and Load the project settings
        const std::string& razixRoot = STRINGIZE(RAZIX_ROOT_DIR);
        // Path to the Project path (*.razixproject)
        // TODO: Since the Engine will be installed elsewhere and and Project will be else where this logic has to be re-factored to use the proper project path to resolve the VFS to mount the project Assets
        m_AppFilePath = razixRoot + projectRoot;
        RAZIX_CORE_TRACE("Project file path : {0}", m_AppFilePath);
    }

    void RZApplication::Init()
    {
        // Load the De-serialized data from the project file or use the command line argument to open the file
        // TODO: Add verification for Engine and Project Version
        std::ifstream AppStream;
        if (RZEngine::Get().commandLineParser.isSet("project filename")) {
            std::string fullPath = RZEngine::Get().commandLineParser.getValueAsString("project filename");
            RAZIX_CORE_TRACE("Command line filename : {0}", fullPath);
            AppStream.open(fullPath, std::ifstream::in);
            m_AppFilePath = fullPath.substr(0, fullPath.find_last_of("\\/")) + "/";
        }
        else {
            // TODO: If command line is not provided or doesn't use engine default sandbox project we need some way to resolve the project root directory, make this agnostic we need not redirect to sandbox by default it must be provided as a placeholder value instead as a fall back
            //m_AppFilePath = ??
            std::string projectFullPath = m_AppFilePath + m_AppName + std::string(".razixproject");
            AppStream.open(projectFullPath, std::ifstream::in);
        }

        // Check the command line arguments for the rendering api
        if (RZEngine::Get().commandLineParser.isSet("rendering api"))
            Graphics::RZGraphicsContext::SetRenderAPI((Graphics::RenderAPI) RZEngine::Get().commandLineParser.getValueAsInt("project filename"));

        // De-serialize the application
        if (AppStream.is_open()) {
            RAZIX_CORE_TRACE("Loading project file...");
            cereal::JSONInputArchive inputArchive(AppStream);
            inputArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }

        // Mount the VFS paths based on the Project directory (done here cause the Application can make things easier by making this easy by loading some default directories, others can be added later sandbox shouldn't be troubled by all this labor work)
        // First the default sandbox or sample project is loaded that is provided by the engine that resides with the engine 
        // Next it checks the command line for the project file directory
        // Project root directory
        RAZIX_CORE_TRACE("Mounting file systems... for Project at : {0}", m_AppFilePath);
        
        RZVirtualFileSystem::Get().mount("Project", m_AppFilePath);

        RZVirtualFileSystem::Get().mount("Assets",      m_AppFilePath + std::string("Assets"));
        RZVirtualFileSystem::Get().mount("Meshes",      m_AppFilePath + std::string("Assets/Meshes"));
        RZVirtualFileSystem::Get().mount("Scenes",      m_AppFilePath + std::string("Assets/Scenes"));
        RZVirtualFileSystem::Get().mount("Scripts",     m_AppFilePath + std::string("Assets/Scripts"));
        RZVirtualFileSystem::Get().mount("Sounds",      m_AppFilePath + std::string("Assets/Sounds"));
        RZVirtualFileSystem::Get().mount("Textures",    m_AppFilePath + std::string("Assets/Textures"));
        

        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------


        // The Razix Application Signature Name is generated here and passed to the window
        // TODO: Add render API being used to the Signature dynamically
        std::string SignatureTitle = m_AppName + " | " + "Razix Engine" + " - " + Razix::RazixVersion.getVersionString() + " " + "[" + Razix::RazixVersion.getReleaseStageString() + "]" + " " + "<" + Graphics::RZGraphicsContext::GetRenderAPIString() + ">" + " | " + " " + STRINGIZE(RAZIX_BUILD_CONFIG);

        // Create the timer
        m_Timer = CreateUniqueRef<Timer>();

        // Set the window properties and create the timer
        m_WindowProperties.Title = SignatureTitle;

        // Create the Window
        m_Window = UniqueRef<RZWindow>(RZWindow::Create(m_WindowProperties));
        m_Window->SetEventCallback(RAZIX_BIND_CB_EVENT_FN(RZApplication::OnEvent));

        //-------------------------------------------------------------------------------------
        // Creating the Graphics Context and Swapchain and Initialize it
        Graphics::RZGraphicsContext::Create(m_WindowProperties, m_Window.get());
        Graphics::RZGraphicsContext::GetContext()->Init();
        swapchain = Graphics::RZSwapchain::Create(m_Window->getWidth(), m_Window->getHeight());
        //-------------------------------------------------------------------------------------

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

    void RZApplication::OnEvent(RZEvent& event)
    {
        RZEventDispatcher dispatcher(event);
        // Window close event
        dispatcher.Dispatch<WindowCloseEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowClose));
        // Window resize event
        dispatcher.Dispatch<RZWindowResizeEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowResize));
    }

    bool RZApplication::OnWindowClose(WindowCloseEvent& e)
    {
        m_CurrentState = AppState::Closing;
        return true;
    }

    bool RZApplication::OnWindowResize(RZWindowResizeEvent& e)
    {
        return true;
    }

    void RZApplication::Run()
    {
        OnStart();
        while (OnFrame()) { }
        Quit();
    }

    bool RZApplication::OnFrame()
    {
        // Calculate the delta time
        float now = m_Timer->GetElapsedS();
        auto& stats = RZEngine::Get().GetStatistics();
        m_Timestep.Update(now);

        // Update the stats
        stats.FrameTime = m_Timestep.GetTimestepMs();
        //RAZIX_CORE_TRACE("Time steps : {0} ms", stats.FrameTime);

        // Poll for Input events
        m_Window->ProcessInput();

        // Early close if the escape key is pressed or close button is pressed
        if (RZInput::IsKeyPressed(Razix::KeyCode::Key::Escape))
            m_CurrentState = AppState::Closing;

        if (m_CurrentState == AppState::Closing)
            return false;

        // Update the Engine systems
        OnUpdate(m_Timestep);
        m_Updates++;

        // Render the Graphics
        OnRender();
        m_Frames++;

        // Update the window and it's surface/video out
        m_Window->OnWindowUpdate();

        // FLip the swapchain to present the rendered image
        swapchain->Flip();

        // Record the FPS
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

    void RZApplication::OnStart()
    {
        //! Testing Texture loading and other demo stuff REMOVE THIS!!!
        Graphics::RZTexture::Filtering filtering = {};
        filtering.minFilter = Graphics::RZTexture::Filtering::FilterMode::LINEAR;
        filtering.magFilter = Graphics::RZTexture::Filtering::FilterMode::LINEAR;

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
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

            glViewport(0, 0, m_Window->getWidth(), m_Window->getHeight());

            Graphics::RZTexture2D* logoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/RazixLogo.png", "TextureAttachmentGLTest", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE, filtering); 
        }
        else if (Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::VULKAN) {
            Graphics::RZTexture2D* logoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/RazixLogo.png", "TextureAttachmentVKTest", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE, filtering);
            logoTexture->Release();
        }
    }

    void RZApplication::OnUpdate(const Timestep& dt) 
    {

    }

    void RZApplication::OnRender() {
        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) 
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }

    void RZApplication::Quit()
    {
        // TODO: Release the Graphics context at the right place
        swapchain->Destroy();
        Graphics::RZGraphicsContext::Release();

        // Save the app data before closing
        RAZIX_CORE_WARN("Saving project...");
        std::ofstream opAppStream(m_AppFilePath);
        cereal::JSONOutputArchive saveArchive(opAppStream);
        saveArchive(cereal::make_nvp("Razix Application", *s_AppInstance));

        RAZIX_CORE_ERROR("Closing Application!");
    }
}