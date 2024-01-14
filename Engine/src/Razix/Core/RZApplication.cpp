// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZApplication.h"

// ---------- Engine ----------
#include "Razix/Core/RZEngine.h"
// ----------------------------

#include "Razix/Core/RZSplashScreen.h"
#include "Razix/Core/RazixVersion.h"

#include "Razix/Core/RZCPUMemoryManager.h"

#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Events/ApplicationEvent.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Scene/Components/CameraComponent.h"
#include "Razix/Scene/Components/LightComponent.h"

#include "Razix/Graphics/Renderers/RZForwardRenderer.h"
#include "Razix/Graphics/Renderers/RZImGuiRenderer.h"

#include "Razix/Scene/Components/TransformComponent.h"

#include <backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/plugins/IconsFontAwesome5.h>
#include <imgui/plugins/ImGuizmo.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <cereal/archives/json.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <entt.hpp>

namespace Razix {
    RZApplication* RZApplication::s_AppInstance = nullptr;

    // Editor-Graphics API Resize primitives won't make into final game so not an issues as of now!!!
    bool                    RZApplication::ready_for_execution = false;
    std::mutex              RZApplication::m;
    std::condition_variable RZApplication::halt_execution;

    RZApplication::RZApplication(const std::string& projectRoot, const std::string& appName /*= "Razix App"*/)
        : m_ProjectName(appName), m_Timestep(RZTimestep(0.0f)), m_GuizmoOperation(ImGuizmo::TRANSLATE), m_GuizmoMode(ImGuizmo::MODE::WORLD)
    {
        // Create the application instance
        RAZIX_CORE_ASSERT(!s_AppInstance, "Application already exists!");
        s_AppInstance = this;

        // Set the Application root path and Load the project settings
        //const std::string& razixRoot = RZEngine::Get().getEngineInstallationDir();    // RAZIX_STRINGIZE(RAZIX_ROOT_DIR);
        // Path to the Project path (*.razixproject)
        m_ProjectFilePath = projectRoot + "/";
        m_ProjectName     = appName;
        RAZIX_CORE_TRACE("Project file path : {0}", m_ProjectFilePath);

        Razix::RZSplashScreen::Get().setLogString("Loading Shader Cache...");
        Razix::RZSplashScreen::Get().setLogString("Loading Project Assets..");

        Razix::RZEngine::Get().isRZApplicationCreated = true;
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
            m_ProjectFilePath = fullPath.substr(0, fullPath.find_last_of("\\/")) + "/";
        } else {
            // TODO: If command line is not provided or doesn't use engine default sandbox project we need some way to resolve the project root directory, make this agnostic we need not redirect to sandbox by default it must be provided as a placeholder value instead as a fall back
            //m_AppFilePath = ??
            std::string projectFullPath = m_ProjectFilePath + "/" + m_ProjectName + std::string(".razixproject");
            AppStream.open(projectFullPath, std::ifstream::in);
        }

        // Mount the VFS paths based on the Project directory (done here cause the Application can make things easier by making this easy by loading some default directories, others can be added later sandbox shouldn't be troubled by all this labor work)
        // First the default sandbox or sample project is loaded that is provided by the engine that resides with the engine
        // Next it checks the command line for the project file directory
        // Project root directory
        RAZIX_CORE_TRACE("Mounting file systems... for Project at : {0}", m_ProjectFilePath);
        Razix::RZSplashScreen::Get().setLogString("Mounting file systems...");

        RZVirtualFileSystem::Get().mount("Project", m_ProjectFilePath);

        RZVirtualFileSystem::Get().mount("Assets", m_ProjectFilePath + std::string("/Assets"));
        RZVirtualFileSystem::Get().mount("Meshes", m_ProjectFilePath + std::string("/Assets/Meshes"));
        RZVirtualFileSystem::Get().mount("Scenes", m_ProjectFilePath + std::string("/Assets/Scenes"));
        RZVirtualFileSystem::Get().mount("Scripts", m_ProjectFilePath + std::string("/Assets/Scripts"));
        RZVirtualFileSystem::Get().mount("Sounds", m_ProjectFilePath + std::string("/Assets/Sounds"));
        RZVirtualFileSystem::Get().mount("Textures", m_ProjectFilePath + std::string("/Assets/Textures"));
        RZVirtualFileSystem::Get().mount("Materials", m_ProjectFilePath + std::string("/Assets/Materials"));

        // Check the command line arguments for the rendering api
        if (RZEngine::Get().commandLineParser.isSet("rendering api"))
            Graphics::RZGraphicsContext::SetRenderAPI((Graphics::RenderAPI) RZEngine::Get().commandLineParser.getValueAsInt("project filename"));

        // De-serialize the application
        if (AppStream.is_open()) {
            RAZIX_CORE_TRACE("Loading project file...");
            cereal::JSONInputArchive inputArchive(AppStream);
            inputArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }

        // The Razix Application Signature Name is generated here and passed to the window
        std::string SignatureTitle = m_ProjectName + " | " + "Razix Engine" + " - " + Razix::RazixVersion.getVersionString() + " " + "[" + Razix::RazixVersion.getReleaseStageString() + "]" + " " + "<" + Graphics::RZGraphicsContext::GetRenderAPIString() + ">" + " | " + " " + RAZIX_STRINGIZE(RAZIX_BUILD_CONFIG);

        // Create the timer
        m_Timer = rzstl::CreateUniqueRef<RZTimer>();

        // Set the window properties and create the timer
        m_WindowProperties.Title = SignatureTitle;

        // TODO: Load any other Engine systems that needs to be done only in the Application
        // Destroy the Splash Screen before we create the window
        //Razix::RZSplashScreen::Get().destroy();

        // Create the Window only if it's not set before (using the native window pointer, usually done by the QT editor)
        if (m_Window == nullptr) {
            m_Window = RZWindow::Create(m_WindowProperties);
        }
        m_Window->SetEventCallback(RAZIX_BIND_CB_EVENT_FN(RZApplication::OnEvent));

        // Create a default project file file if nothing exists
        if (!AppStream.is_open()) {
            RAZIX_CORE_ERROR("Project File does not exist!");
            std::string               projectFullPath = m_ProjectFilePath + m_ProjectName + std::string(".razixproject");
            std::ofstream             opAppStream(projectFullPath);
            cereal::JSONOutputArchive defArchive(opAppStream);
            RAZIX_CORE_TRACE("Creating a default Project file...");

            defArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }

        // Convert the app to loaded state
        m_CurrentState = AppState::Loading;

        // Enable V-Sync
        //m_Window->SetVSync(true);
    }

    void RZApplication::OnEvent(RZEvent& event)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        RZEventDispatcher dispatcher(event);
        // Window close event
        dispatcher.Dispatch<WindowCloseEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowClose));
        // Window resize event
        dispatcher.Dispatch<RZWindowResizeEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowResize));

        // Mouse Events
        // Mouse Moved event
        dispatcher.Dispatch<RZMouseMovedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseMoved));
        dispatcher.Dispatch<RZMouseButtonPressedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseButtonPressed));
        dispatcher.Dispatch<RZMouseButtonReleasedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseButtonReleased));
        dispatcher.Dispatch<RZKeyPressedEvent>(RAZIX_BIND_CB_EVENT_FN(OnKeyPress));
        dispatcher.Dispatch<RZKeyReleasedEvent>(RAZIX_BIND_CB_EVENT_FN(OnKeyRelease));
    }

    bool RZApplication::OnWindowClose(WindowCloseEvent& e)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        m_CurrentState = AppState::Closing;
        return true;
    }

    bool RZApplication::OnWindowResize(RZWindowResizeEvent& e)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            // Resize ImGui
            ImGuiIO& io    = ImGui::GetIO();
            io.DisplaySize = ImVec2(static_cast<f32>(e.GetWidth()), static_cast<f32>(e.GetHeight()));
            //io.DisplaySize             = ImVec2(static_cast<f32>(2560), static_cast<f32>(1440));
            io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        }

        if (Graphics::RHI::GetPointer() != nullptr) {
            Graphics::RHI::OnResize(e.GetWidth(), e.GetHeight());

            // Resize the frame graph resource before resizing the RHI
            Razix::RZEngine::Get().getWorldRenderer().getFrameGraph().resize(e.GetWidth(), e.GetHeight());
        }

        OnResize(e.GetWidth(), e.GetHeight());
        return true;
    }

    bool RZApplication::OnMouseMoved(RZMouseMovedEvent& e)
    {
#if 1
        auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos = ImVec2(e.GetX(), e.GetY());
        }
#endif
        return true;
    }

    bool RZApplication::OnMouseButtonPressed(RZMouseButtonPressedEvent& e)
    {
#if 1
        auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            ImGuiIO& io                          = ImGui::GetIO();
            io.MouseDown[e.GetMouseButton() - 1] = true;
            io.MouseDown[e.GetMouseButton() - 1] = true;
        }
#endif
        return true;
    }

    bool RZApplication::OnMouseButtonReleased(RZMouseButtonReleasedEvent& e)
    {
#if 1
        auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            ImGuiIO& io                          = ImGui::GetIO();
            io.MouseDown[e.GetMouseButton() - 1] = false;
        }
#endif
        return true;
    }

    bool RZApplication::OnKeyPress(RZKeyPressedEvent& e)
    {
#if 0
 auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            ImGuiIO& io                 = ImGui::GetIO();
            io.KeysDown[e.GetKeyCode()] = true;
        }
#endif
        return true;
    }

    bool RZApplication::OnKeyRelease(RZKeyReleasedEvent& e)
    {
#if 0
 auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            ImGuiIO& io                 = ImGui::GetIO();
            io.KeysDown[e.GetKeyCode()] = false;
        }
#endif
        return true;
    }

    void RZApplication::Run()
    {
        Razix::RZSplashScreen::Get().setLogString("Initializing RHI...");

        // Create the API renderer to issue render commands
        Graphics::RHI::Create(getWindow()->getWidth(), getWindow()->getHeight());
        // TODO: Enable window V-Sync here
        Graphics::RHI::Init();

        // TODO: Job system and Engine Systems(run-time) Initialization
        Razix::RZSplashScreen::Get().setLogString("Loading Scene...");

        // Now the scenes are loaded onto the scene manger here but they must be STATIC INITIALIZED shouldn't depend on the start up for the graphics context
        for (auto& sceneFilePath: sceneFilePaths)
            RZSceneManager::Get().enqueueSceneFromFile(sceneFilePath);

        // Load a scene into memory
        RZSceneManager::Get().loadScene(0);

        Razix::RZSplashScreen::Get().setLogString("Scene Loading Successful...");

        Razix::RZSplashScreen::Get().setLogString("Building FrameGraph...");

        Razix::RZEngine::Get().getWorldRenderer().buildFrameGraph(Razix::RZEngine::Get().getWorldSettings(), RZSceneManager::Get().getCurrentScene());

        m_CurrentState = AppState::Running;

        Razix::RZSplashScreen::Get().setLogString("Starting Razix Application...");

        Razix::RZSplashScreen::Get().destroy();

        //m_GPUProfiler.Init(&RZCPUMemoryManager::Get().getSystemAllocator(), RAZIX_MAX_FRAMES, 32);

        Start();
    }

    bool RZApplication::RenderFrame()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);
        RAZIX_PROFILE_FRAMEMARKER("RZApplication Main Thread");

        if (RZApplication::Get().getAppType() != AppType::GAME) {
            // Wait until Editor sends data
            std::unique_lock<std::mutex> lk(m);
            halt_execution.wait(lk, [] {
                return ready_for_execution;
            });
            // Manual unlocking is done before notifying, to avoid waking up
            // the waiting thread only to block again (see notify_one for details)
            lk.unlock();
            halt_execution.notify_one();
        }

        // TODO: Add Time stamp Queries for calculating GPU time here

        // Calculate the delta time
        f32 now = m_Timer->GetElapsedS();
        RZEngine::Get().ResetStats();
        auto& stats = RZEngine::Get().GetStatistics();
        m_Timestep.Update(now);

        // Update the stats
        stats.DeltaTime = m_Timestep.GetTimestepMs();
        //RAZIX_CORE_TRACE("Time steps : {0} ms", stats.FrameTime);

        // Poll for Input events
        m_Window->ProcessInput();

        // Early close if the escape key is pressed or close button is pressed
        if (RZInput::IsKeyPressed(Razix::KeyCode::Key::Escape))
            m_CurrentState = AppState::Closing;

        if (m_CurrentState == AppState::Closing)
            return false;

        // Reload shaders and FrameGraph resources
        if (RZInput::IsKeyPressed(Razix::KeyCode::Key::R)) {
            RAZIX_CORE_INFO("Reloading FrameGraph...");
            Graphics::RZShaderLibrary::Get().reloadShadersFromDisk();
            auto& worldRenderer = Razix::RZEngine::Get().getWorldRenderer();
            worldRenderer.destroy();
            Razix::Graphics::FrameGraph::RZFrameGraph::ResetFirstFrame();
            worldRenderer.buildFrameGraph(Razix::RZEngine::Get().getWorldSettings(), RZSceneManager::Get().getCurrentScene());

            RAZIX_CORE_INFO("FrameGraph reload Done!");
        }

        // Update the Engine systems
        Update(m_Timestep);
        m_Updates++;

        // Render the Graphics
        Render();
        m_Frames++;

        // RenderGUI
        RenderGUI();

        // Update the window and it's surface/video out
        m_Window->OnWindowUpdate();

        // FLip the swap chain to present the rendered image
        //swapchain->Flip();

        {
            RAZIX_PROFILE_SCOPEC("RZApplication::TimeStepUpdates", RZ_PROFILE_COLOR_APPLICATION);

            // Record the FPS
            if (now - m_SecondTimer > 1.0f) {
                m_SecondTimer += 1.0f;

                stats.FramesPerSecond  = m_Frames;
                stats.UpdatesPerSecond = m_Updates;
                //RAZIX_CORE_TRACE("FPS : {0}", stats.FramesPerSecond);
                //RAZIX_CORE_TRACE("UPS : {0} ms", stats.UpdatesPerSecond);

                m_Frames  = 0;
                m_Updates = 0;
            }
        }
        return m_CurrentState != AppState::Closing;
    }

    /* Application Serialization */
    // Load mechanism for the RZApplication class
    template<class Archive>
    void RZApplication::load(Archive& archive)
    {
        std::string projectName;
        archive(cereal::make_nvp("Project Name", projectName));
        RAZIX_ASSERT_MESSAGE((projectName == m_ProjectName), "Project name doesn't match with Executable");
        /**
             * Currently the project name will be verified with the one given in the sandbox or game project
             * If it doesn't match it updates the project name, it's not necessary that the name must match the 
             * executable name, since the Editor can load any *.razixproject file, this should also mean that
             * it should be able to load any project name since the project name is always mutable just all it's properties
             * There's not enforcement on the project names and other properties, the Razix Editor Application 
             * and can load any thing as long it is supplies with the required data to
             */
        m_ProjectName = projectName;
        // TODO: Verify these two!
        //archive(cereal::make_nvp("Engine Version", Razix::RazixVersion.GetVersionString()));
        //archive(cereal::make_nvp("Project Version", 0));
        archive(cereal::make_nvp("Render API", m_RenderAPI));
        // Set the render API from the De-serialized data
        Graphics::RZGraphicsContext::SetRenderAPI((Graphics::RenderAPI) m_RenderAPI);
        u32 Width, Height;
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
    void RZApplication::save(Archive& archive) const
    {
        RAZIX_TRACE("Window Resize override sandbox application! | W : {0}, H : {1}", m_Window->getWidth(), m_Window->getHeight());
        archive(cereal::make_nvp("Project Name", m_ProjectName));
        archive(cereal::make_nvp("Engine Version", Razix::RazixVersion.getVersionString()));
        archive(cereal::make_nvp("Project ID", m_ProjectID.prettyString()));
        archive(cereal::make_nvp("Render API", (u32) Graphics::RZGraphicsContext::GetRenderAPI()));
        archive(cereal::make_nvp("Width", m_Window->getWidth()));
        archive(cereal::make_nvp("Height", m_Window->getHeight()));
        archive(cereal::make_nvp("Project Path", m_ProjectFilePath));    // Why am I even serializing this?

        auto& paths = RZSceneManager::Get().getSceneFilePaths();

        std::vector<std::string> newPaths;
        for (auto& path: paths) {
            std::string newPath;
            RZVirtualFileSystem::Get().absolutePathToVFS(path, newPath);
            newPaths.push_back(path);
        }
        archive(cereal::make_nvp("Scenes", newPaths));
    }

    void RZApplication::Start()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        OnStart();

        // Run the OnStart method for all the scripts in the scene
        if (RZSceneManager::Get().getCurrentScene())
            RZEngine::Get().getScriptHandler().OnStart(RZSceneManager::Get().getCurrentScene());
    }

    void RZApplication::Update(const RZTimestep& dt)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        // TODO: Check if it's the primary or not and make sure you render only to the Primary Camera, if not then don't render!!!!
        // Update the renderer stuff here
        // Update Scene Graph here
        RZSceneManager::Get().getCurrentScene()->update();
        // Update the Scene Camera Here
        RZSceneManager::Get().getCurrentScene()->getSceneCamera().update(dt.GetTimestepMs());

        auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            // Update ImGui
            ImGuiIO& io = ImGui::GetIO();
            (void) io;
            // TODO: get the resolution from RHI before updating this
            io.DisplaySize             = ImVec2(static_cast<f32>(getWindow()->getWidth()), static_cast<f32>(getWindow()->getHeight()));
            io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
            //io.DisplayFramebufferScale = ImVec2(static_cast<f32>(io.DisplaySize.x / 2560.0f), static_cast<f32>(io.DisplaySize.y / 1440.0f));
        }

        // Update the Runtime Systems only on Game Application type
        //if (m_appType == AppType::GAME) {
        // Run the OnUpdate for all the scripts
        // FIXME: Enable this when the data driven rendering is finished
        //if (RZSceneManager::Get().getCurrentScene())
        //    RZEngine::Get().getScriptHandler().OnUpdate(RZSceneManager::Get().getCurrentScene(), dt);

        // TODO: Update the Physics Engine here
        /*RZEngine::Get().getPhysicsEngine().update(dt); */
        //}

        // Client App Update
        OnUpdate(dt);

        //m_GPUProfiler.update();

#if 1

#endif
    }

    void RZApplication::Render()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        Razix::RZEngine::Get().getWorldRenderer().drawFrame(Razix::RZEngine::Get().getWorldSettings(), RZSceneManager::Get().getCurrentScene());

        OnRender();
    }

    void RZApplication::RenderGUI()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        auto ctx = ImGui::GetCurrentContext();
        if (!ctx)
            return;

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
            ImGui_ImplOpenGL3_NewFrame();

        // TODO: Well GLFW needs to be removed at some point and we need to use native functions
        if (RZApplication::Get().getAppType() == AppType::GAME)
            ImGui_ImplGlfw_NewFrame();

        // FIXME: https://github.com/ocornut/imgui/issues/6064

        // Update ImGui
        ImGuiIO& io = ImGui::GetIO();
        (void) io;
#if 0
        io.DisplaySize = ImVec2(static_cast<f32>(getWindow()->getWidth()), static_cast<f32>(getWindow()->getHeight()));
        // TODO: get the resolution from RHI before updating this
        io.DisplayFramebufferScale = ImVec2(static_cast<f32>(io.DisplaySize.x / 2560.0f), static_cast<f32>(io.DisplaySize.y / 1440.0f));
#endif

        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        if (RZSceneManager::Get().getCurrentScene())
            RZEngine::Get().getScriptHandler().OnImGui(RZSceneManager::Get().getCurrentScene());

        OnImGui();

        if (m_appType == AppType::GAME) {
            // Engine ImGui Tools will be rendered here
            static bool showResourceViewer = false;
            static bool showBudgets        = false;
            static bool showMemStats       = true;
            {
                RAZIX_PROFILE_SCOPEC("Engine Tools", RZ_PROFILE_COLOR_CORE)

                // Main menu for Game view tools
                {
                    if (ImGui::BeginMainMenuBar()) {
                        if (ImGui::BeginMenu(ICON_FA_WRENCH " Tools")) {
                            if (ImGui::MenuItem(ICON_FA_TASKS " FG resource Viewer", nullptr, showResourceViewer)) {
                                showResourceViewer = !showResourceViewer;
                            }
                            if (ImGui::MenuItem(ICON_FA_MONEY_BILL " Frame Budgets", nullptr, showBudgets)) {
                                showBudgets = !showBudgets;
                            }
                            if (ImGui::MenuItem(ICON_FA_MEMORY " Memory Stats", nullptr, showMemStats)) {
                                showMemStats = !showMemStats;
                            }
                            ImGui::EndMenu();
                        }
                        ImGui::EndMainMenuBar();
                    }
                }

                // Framegraph resource viewer
                {
                    if (showResourceViewer) {
#if 1

                        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                        ImGui::SetNextWindowBgAlpha(0.8f);

                        if (ImGui::Begin("[Engine Tool] Frame Graph Resources Viewer")) {
                            // Hacks to rotate text in ImGui. From https://github.com/ocornut/imgui/issues/1286#issue-251214314
                            int  rotation_start_index;
                            auto ImRotateStart = [&]() { rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size; };
                            auto ImRotateEnd   = [&](float rad, ImVec2 center) {
                                float s = sin(rad), c = cos(rad);
                                center = ImRotate(center, s, c) - center;

                                auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
                                for (int i = rotation_start_index; i < buf.Size; i++)
                                    buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
                            };

                            i32 passIndex     = 0;
                            i32 resourceIndex = 0;

                            auto passNodesSize = RZEngine::Get().getWorldRenderer().getFrameGraph().getPassNodesSize();

                            float  passNameHeight    = 400.0f;
                            float  resourceNameWidth = 400.0f;
                            ImVec2 boxSize           = ImVec2(25.0f, ImGui::GetTextLineHeightWithSpacing() * 1.25f);
                            float  width             = (int) passNodesSize * boxSize.x + resourceNameWidth;
                            float  height            = 800;

                            ImGui::BeginChild("Table", ImVec2(width, height));
                            ImDrawList* pCmd = ImGui::GetWindowDrawList();

                            ImVec2 cursor      = ImGui::GetCursorScreenPos();
                            ImVec2 passNamePos = cursor + ImVec2(resourceNameWidth, 0);

                            const Graphics::FrameGraph::RZPassNode* pActivePass = nullptr;

                            for (size_t i = 0; i < passNodesSize; i++) {
                                const auto& passNode         = RZEngine::Get().getWorldRenderer().getFrameGraph().getPassNode(i);
                                const auto& passNodeName     = passNode.getName();
                                const auto& passNodeGraphIdx = passNode.getID();

                                ImRect itemRect(passNamePos + ImVec2(passIndex * boxSize.x, 0.0f), passNamePos + ImVec2((passIndex + 1) * boxSize.x, passNameHeight));
                                pCmd->AddLine(itemRect.Max, itemRect.Max + ImVec2(0, height), ImColor(1.0f, 1.0f, 1.0f, 0.2f));
                                ImRotateStart();
                                ImVec2 size = ImGui::CalcTextSize(passNodeName.c_str());
                                pCmd->AddText(itemRect.Max - ImVec2(size.x, 0), ImColor(1.0f, 1.0f, 1.0f), passNodeName.c_str());
                                ImRotateEnd(glm::pi<float>() * 2.2f, itemRect.Max + ImVec2(boxSize.x, 0));
                                ImGui::ItemAdd(itemRect, passIndex);
                                bool passActive = ImGui::IsItemHovered();
                                if (passActive) {
                                    ImGui::BeginTooltip();
                                    {
                                        ImGui::Text("Name           : %s", passNodeName.c_str());
                                        ImGui::Text("Graph ID       : %d", passNodeGraphIdx);
                                        ImGui::Text("Reads          : %d", passNode.getInputResources().size());
                                        ImGui::Text("Writes         : %d", passNode.getOutputResources().size());
                                        ImGui::Text("Standalone     : %s", passNode.isStandAlone() ? "true" : "false");
                                        ImGui::Text("Data driven    : %s", passNode.isDataDriven() ? "true" : "false");
                                    }
                                    ImGui::EndTooltip();
                                }
                                ++passIndex;
                            }

                            cursor += ImVec2(0.0f, passNameHeight);
                            ImVec2 resourceAccessPos = cursor + ImVec2(resourceNameWidth, 0.0f);

                            //std::unordered_map<GraphicsResource*, int> resourceToIndex;
                            //auto resourceNodes = RZEngine::Get().getWorldRenderer().getFrameGraph().getResourcesNodes();

                            auto resourceNodesSize = RZEngine::Get().getWorldRenderer().getFrameGraph().getResourceNodesSize();

    #if 1
                            for (size_t i = 0; i < resourceNodesSize; i++) {
                                auto idx = i;
                                //resourceNode.getResourceEntryId();
                                auto& resourceEntry = RZEngine::Get().getWorldRenderer().getFrameGraph().getResourceEntry((Graphics::FrameGraph::RZFrameGraphResource) idx);
                                //if(resourceEntry.getModel<Graphics::FrameGraph::RZFrameGraphTexture>())

                                if (resourceEntry.isImported())
                                    continue;

                                //const auto pFirstPass = resourceEntry.getProducerNode();
                                //const auto pLastPass  = resourceEntry.getLastNode();
                                // if (pFirstPass == nullptr || pLastPass == nullptr)
                                //     continue;

                                u32 firstPassOffset = idx;
                                u32 lastPassOffset  = 0;

                                ImRect itemRect(resourceAccessPos + ImVec2(firstPassOffset * boxSize.x + 1, idx * boxSize.y + 1), resourceAccessPos + ImVec2((lastPassOffset + 1) * boxSize.x - 1, (idx + 1) * boxSize.y - 1));
                                ImGui::ItemAdd(itemRect, idx);
                                bool isHovered = ImGui::IsItemHovered();

                                if (isHovered) {
                                    ImGui::BeginTooltip();
                                    //ImGui::Text("%s", resourceNode.getName());
                                    //
                                    //if (pResource->Type == RGResourceType::Texture) {
                                    //    const TextureDesc& desc = static_cast<const RGTexture*>(pResource)->Desc;
                                    //    ImGui::Text("Res: %dx%dx%d", desc.Width, desc.Height, desc.DepthOrArraySize);
                                    //    ImGui::Text("Fmt: %s", RHI::GetFormatInfo(desc.Format).pName);
                                    //    ImGui::Text("Mips: %d", desc.Mips);
                                    //    ImGui::Text("Size: %s", Math::PrettyPrintDataSize(RHI::GetTextureByteSize(desc.Format, desc.Width, desc.Height, desc.DepthOrArraySize)).c_str());
                                    //} else if (pResource->Type == RGResourceType::Buffer) {
                                    //    const BufferDesc& desc = static_cast<const RGBuffer*>(pResource)->Desc;
                                    //    ImGui::Text("Size: %s", Math::PrettyPrintDataSize(desc.Size).c_str());
                                    //    ImGui::Text("Fmt: %s", RHI::GetFormatInfo(desc.Format).pName);
                                    //    ImGui::Text("Stride: %d", desc.ElementSize);
                                    //    ImGui::Text("Elements: %d", desc.NumElements());
                                    //}
                                    ImGui::EndTooltip();
                                }

                                pCmd->AddRectFilled(itemRect.Min, itemRect.Max, ImColor(0.5f, 0.6f, 1.0f));

                                ImColor boxColor = ImColor(1.0f, 1.0f, 1.0f, 0.5f);

                                //bool isActivePass = false;
                                //if (pActivePass) {
                                //    auto it = std::find_if(pActivePass->Accesses.begin(), pActivePass->Accesses.end(), [pResource](const RGPass::ResourceAccess& access) {
                                //        return access.pResource == pResource;
                                //    });
                                //
                                //    if (it != pActivePass->Accesses.end()) {
                                //        isActivePass                         = true;
                                //        const RGPass::ResourceAccess& access = *it;
                                //        if (ResourceState::HasWriteResourceState(access.Access))
                                //            boxColor = ImColor(1.0f, 0.5f, 0.1f, 0.8f);
                                //        else
                                //            boxColor = ImColor(0.0f, 0.9f, 0.3f, 0.8f);
                                //    }
                                //}

                                if (isHovered)
                                    pCmd->AddRectFilled(itemRect.Min, itemRect.Max, boxColor);
                            }
    #endif

                            // for (auto& resource: resourceToIndex)
                            //     pCmd->AddText(ImVec2(cursor.x, cursor.y + resource.second * boxSize.y), ImColor(1.0f, 1.0f, 1.0f), resource.first->GetName());

                            ImGui::EndChild();
                        }
                        ImGui::End();
#endif
                        ImGui::PopStyleColor(1);
                    }
                }

                // Frame Budgets
                {
                    if (showBudgets) {
                        if (ImGui::Begin("[Engine Tool] Frame Budgets")) {
                            ImGui::BeginTable("#FrameBudgetsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                            {
                                // Declare columns
                                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                ImGui::TableSetupColumn("Time (Ms)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                ImGui::TableSetupColumn("Budget (Ms)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                ImGui::TableSetupColumn("Memory (MiB)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                ImGui::TableSetupColumn("Budget (MiB)", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide, 150.0f);
                                ImGui::TableHeadersRow();

                                ImGui::TableNextRow(ImGuiTableRowFlags_None, 20);
                                ImGui::TableNextColumn();

                                if (ImGui::TreeNode("Art")) {
                                    ImGui::Indent();
                                    {
                                        ImGui::TableNextRow(ImGuiTableRowFlags_None, 20);
                                        ImGui::TableNextColumn();

                                        ImGui::Text("Depth");
                                        ImGui::TableNextColumn();
                                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.75, 0, 1));
                                        {
                                            ImGui::Text("2.4");
                                        }
                                        ImGui::PopStyleColor(1);
                                        ImGui::TableNextColumn();
                                        ImGui::Text("1.2");
                                    }
                                    ImGui::Unindent();

                                    ImGui::TreePop();
                                }
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TableNextColumn();
                                ImGui::Text("2.4");
                                ImGui::TableNextColumn();
                                ImGui::Text("1.2");
                                //for (int row = 0; row < 4; row++) {
                                //    ImGui::TableNextRow();
                                //    for (int column = 0; column < 3; column++) {
                                //        ImGui::TableSetColumnIndex(column);
                                //        ImGui::Text("Row %d Column %d", row, column);
                                //    }
                                //}
                                ImGui::EndTable();
                            }
                        }
                        ImGui::End();
                    }
                }

                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                // Memory Stats
                {
                    if (showMemStats) {
                        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
                        ImGui::SetNextWindowBgAlpha(0.0f);    // Transparent background
                        ImGui::SetNextWindowPos(ImVec2(50, (f32) getWindow()->getHeight() - 50), ImGuiCond_Always);
                        ImGui::Begin("##MemStats", 0, window_flags);
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
                            ImGui::Text(ICON_FA_MEMORY "  GPU Memory: %4.2f", RZEngine::Get().GetStatistics().TotalGPUMemory);
                            ImGui::PopStyleColor(1);
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
                            ImGui::Text(ICON_FA_BALANCE_SCALE " Used GPU Memory: %4.2f |", RZEngine::Get().GetStatistics().GPUMemoryUsed);
                            ImGui::PopStyleColor(1);

                            ImGui::SameLine();
                            auto start = std::chrono::system_clock::now();
                            // Some computation here
                            auto        end      = std::chrono::system_clock::now();
                            std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
                            ImGui::Text(ICON_FA_CLOCK " current date/time : %s ", std::ctime(&end_time));
                            ImGui::PopStyleColor(1);

                            ImGui::SameLine();

                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
                            std::string engineBuildVersionFull = RazixVersion.getVersionString() + "." + RazixVersion.getReleaseStageString();
                            ImGui::Text("| Engine build version : %s | ", engineBuildVersionFull.c_str());
                            ImGui::PopStyleColor(1);

                            ImGui::SameLine();

                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
                            ImGui::Text(ICON_FA_ID_CARD " project UUID : %s", m_ProjectID.prettyString().c_str());
                            ImGui::PopStyleColor(1);
                        }
                        ImGui::End();
                    }
                }
                ImGui::PopStyleVar(1);
            }
        }

        // Guizmo Controls for an Entity
        if (m_EnableGuizmoEditing) {
            auto           currentScene = RZSceneManager::Get().getCurrentScene();
            auto&          registry     = currentScene->getRegistry();
            auto           cameraView   = registry.view<CameraComponent>();
            RZSceneCamera* cam          = nullptr;
            if (!cameraView.empty()) {
                // By using front we get the one and only or the first one in the list of camera entities
                cam = &cameraView.get<CameraComponent>(cameraView.front()).Camera;
            }
            // Guizmo Editing Here
            TransformComponent& tc = m_GuizmoEntity.GetComponent<TransformComponent>();
#if 1
            glm::mat4 transformMatrix = tc.GetLocalTransform();
            glm::mat4 deltaMatrix     = glm::mat4(1.0f);

            //ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(transformMatrix));

            // https://github.com/CedricGuillemet/ImGuizmo/issues/237
            ImGuizmo::Manipulate(glm::value_ptr(cam->getViewMatrix()), glm::value_ptr(cam->getProjectionRaw()), m_GuizmoOperation, m_GuizmoMode, glm::value_ptr(transformMatrix), glm::value_ptr(deltaMatrix), &m_GuizmoSnapAmount);

            // TODO: Add snap options and control them from editor
            f32 matrixTranslation[3], matrixRotation[3], matrixScale[3];
            ImGuizmo::DecomposeMatrixToComponents(&(transformMatrix[0][0]), matrixTranslation, matrixRotation, matrixScale);

            tc.Translation = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
            tc.Rotation    = glm::vec3(glm::radians(matrixRotation[0]), glm::radians(matrixRotation[1]), glm::radians(matrixRotation[2]));
            tc.Scale       = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
            tc.Transform   = transformMatrix;
#endif
        }

        ImGui::SetNextWindowBgAlpha(0.1f);    // Transparent background
        ImGui::Begin("Icons Test");
        {
            ImGui::Text(ICON_FA_PAINT_BRUSH "  Paint");    // use string literal concatenation
            ImGui::Button(ICON_FA_WATER);

            ImGui::Button(ICON_FA_CAMERA);
        }
        ImGui::End();

        // TODO: As for Icons of the components or any other entities we will get them using the entt
        // Get their position in the worldspace and check it against the camera frustum and
        // convert it to screen space and render a non-clickable ImGui::Button with the FontIcon as image

        // Icons for Components
        //{
        //    auto& registry = RZSceneManager::Get().getCurrentScene()->getRegistry();
        //    auto& scnCam   = RZSceneManager::Get().getCurrentScene()->getSceneCamera();
        //    auto  group    = registry.group<LightComponent>(entt::get<TransformComponent>);
        //
        //    for (auto entity: group) {
        //        const auto& [component, trans] = group.template get<LightComponent, TransformComponent>(entity);
        //
        //        glm::vec3 pos = trans.Translation;
        //
        //        glm::vec2  screenPos         = pos;
        //        const auto transformMat      = trans.GetWorldTransform();
        //        auto       screenSpaceCoords = glm::project(pos, transformMat, scnCam.getProjection(), glm::vec4(0.0f, 0.0f, getWindow()->getWidth(), getWindow()->getHeight()));
        //        ImGui::SetCursorPos({screenSpaceCoords.x, screenSpaceCoords.y});
        //        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.7f, 0.0f));
        //
        //        ImGui::Button(ICON_FA_LIGHTBULB);
        //
        //        ImGui::PopStyleColor();
        //    }
        //}

        // Engine Stats
        {
            // Engine stats
            ImGuiWindowFlags     window_flags     = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
            const f32            DISTANCE         = 10.0f;
            const ImGuiViewport* viewport         = ImGui::GetMainViewport();
            ImVec2               work_area_pos    = viewport->WorkPos;    // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2               work_area_size   = viewport->WorkSize;
            ImVec2               window_pos       = ImVec2((1 & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : (work_area_pos.x + DISTANCE), (1 & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
            ImVec2               window_pos_pivot = ImVec2((1 & 1) ? 1.0f : 0.0f, (1 & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::SetNextWindowBgAlpha(0.35f);    // Transparent background

            ImGui::Begin("Engine Stats", 0, window_flags);
            {
                auto& stats = RZEngine::Get().GetStatistics();
                ImGui::Text("Engine Stats");
                ImGui::Indent();
                {
                    // TODO: Add Average timings (CPU + GPU) and avg FPS
                    ImGui::Text("FPS                        : %.5d", stats.FramesPerSecond);
                    ImGui::Text("CPU time                   : %5.2f ms", stats.DeltaTime);

                    ImGui::Separator();
                    ImGui::Text("API calls");

                    ImGui::Text("Total Draw calls           : %d", stats.NumDrawCalls);
                    ImGui::Indent();
                    {
                        ImGui::BulletText("Draws                : %d", stats.Draws);
                        ImGui::BulletText("Indexed Draws        : %d", stats.IndexedDraws);
                        ImGui::BulletText("Compute Dispatches   : %d", stats.ComputeDispatches);
                    }
                    ImGui::Unindent();

                    ImGui::Separator();
                    ImGui::Text("Memory Usage");

                    ImGui::Indent();
                    {
                        ImGui::BulletText("Used VRAM            : %f Gib", stats.GPUMemoryUsed);
                        ImGui::BulletText("Total VRAM           : %f Gib", stats.TotalGPUMemory);
                        ImGui::BulletText("Total RAM            : %f Gib", stats.UsedRAM);
                    }
                    ImGui::Unindent();

                    ImGui::Separator();
                    ImGui::Text("Meshes Renderer            : %d", stats.MeshesRendered);
                    ImGui::Text("Vertices count             : %d", stats.VerticesCount);
                }
                ImGui::Unindent();

                ImGui::Separator();
                //ImGui::Text("Pass Timings");
                if (ImGui::TreeNode("Pass Timings")) {
                    f32 Totaldt = 0.0f;
                    for (auto& [name, dt]: stats.PassTimings) {
                        Totaldt += dt;
                        ImGui::BulletText("%-23s : %5.2f ms", name.c_str(), dt);
                    }
                    ImGui::Separator();
                    ImGui::BulletText("%-23s : %5.2f ms", "Passes Sum", Totaldt);
                    ImGui::BulletText("%-23s : %5.2f ms", "Acquire + Flip", stats.DeltaTime - Totaldt);
                    ImGui::TreePop();
                }
            }
            ImGui::End();
        }

        // GPU Profiler Pipeline Stats
        //m_GPUProfiler.onImGuiDraw();

        ImGui::Render();
    }

    void RZApplication::Quit()
    {
        Razix::RZEngine::Get().getWorldRenderer().destroy();

        // Client side quit customization
        OnQuit();

        // Save the scene and the Application
        RZSceneManager::Get().saveAllScenes();
        RZSceneManager::Get().destroyAllScenes();
        SaveApp();

        // FIXME: This is fucked up I'm not cleaning stuff for editor mode
        if (RZApplication::Get().getAppType() == AppType::GAME)
            Graphics::RHI::Release();

        RAZIX_CORE_ERROR("Closing Application!");
    }

    void RZApplication::SaveApp()
    {
        // Save the app data before closing
        RAZIX_CORE_WARN("Saving project...");
        std::string               projectFullPath = m_ProjectFilePath + m_ProjectName + std::string(".razixproject");
        std::ofstream             opAppStream(projectFullPath);
        cereal::JSONOutputArchive saveArchive(opAppStream);
        saveArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
    }
}    // namespace Razix