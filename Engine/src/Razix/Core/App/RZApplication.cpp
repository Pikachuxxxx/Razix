// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZApplication.h"

// ---------- Engine ----------
#include "Razix/Core/RZEngine.h"
// ----------------------------

#include "Razix/Core/SplashScreen/RZSplashScreen.h"
#include "Razix/Core/Version/RazixVersion.h"

#include "Razix/Core/OS/RZFileSystem.h"

#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Events/ApplicationEvent.h"

#include <Core/Log/RZLog.h>
#include <Gfx/RHI/RHI.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/plugins/IconsFontAwesome5.h>
//#include <imgui/plugins/ImGuizmo.h>
#ifdef RAZIX_USE_GLFW_WINDOWS
    #include <GLFW/glfw3.h>
#endif

#define ENABLE_IMGUI_EVENT_DATA_CAPTURE 0

namespace Razix {

    RZApplication* RZApplication::s_AppInstance = NULL;

    static RZString GetAppWindowTitleSignature(const RZString& projectName)
    {
        RZString SignatureTitle = projectName + " | " + "Razix Engine" + " - " + Razix::RazixVersion.getVersionString() + " " + "[" + Razix::RazixVersion.getReleaseStageString() + "]" + " " + "<" + rzGfxCtx_GetRenderAPIString() + ">" + " | " + RAZIX_STRINGIZE(RAZIX_BUILD_CONFIG);
        return SignatureTitle;
    }

    RZApplication::RZApplication(const RZString& projectRoot, const RZString& appName /*= "Razix App"*/)
        : m_ProjectName(appName), m_ProjectPath(projectRoot)
    {
        // Create the application instance
        RAZIX_CORE_ASSERT(!s_AppInstance, "Application already exists!");
        s_AppInstance = this;

        // Set the Application root path and Load the project settings
        // Path to the Project path (*.razixproject), this is also the place where the Assets folder exist
        RAZIX_CORE_TRACE("Project file path : {0}", m_ProjectPath);

        Razix::RZSplashScreen::Get().setLogString("Loading Project Assets..");
        Razix::RZEngine::Get().isRZApplicationCreated = true;
    }

    void RZApplication::Init()
    {
        // Load the De-serialized data from the project file or use the command line argument to open the file
        // TODO: Add verification for Engine and Project Version
        if (RZEngine::Get().getCommandLineParser().isSet("project file name") && RZEngine::Get().getCommandLineParser().isSet("project file path")) {
            RZString projectPath = RZEngine::Get().getCommandLineParser().getValueAsString("project file path");
            RZString projectName = RZEngine::Get().getCommandLineParser().getValueAsString("project file name");
            RZString fullPath    = projectPath + projectName + RZString(".razixproject");
            RAZIX_CORE_TRACE("[Application] Command line resolved full project path : {0}", fullPath);
            RAZIX_CORE_INFO("[Application] Opening the project file de-serialization...");

            RAZIX_CORE_TRACE("Loading project file...");
            RZString physicalPath;
            if (!RZVirtualFileSystem::Get().resolvePhysicalPath(fullPath, physicalPath)) {
                RAZIX_CORE_ERROR("[Application] Failed to resolve path for *.razixproject: {0}", fullPath);
                RAZIX_DEBUG_BREAK();
                return;
            }
            auto           jsonStrData = RZFileSystem::ReadTextFile(physicalPath);
            nlohmann::ordered_json data        = nlohmann::ordered_json::parse(jsonStrData);
            // pass this off to the load function below
            loadRazixProject(data);
            m_ProjectPath = projectPath;
        } else {
            RAZIX_CORE_WARN("[Application] command line args for project file path and name are not set...using App args to resolve *.razixproject");
            // TODO: If command line is not provided or doesn't use engine default sandbox project we need some way to resolve the project root directory, make this agnostic we need not redirect to sandbox by default it must be provided as a placeholder value instead as a fall back
            //m_AppFilePath = ??
            RZString projectFullPath = m_ProjectPath + "/" + m_ProjectName + RZString(".razixproject");
            RAZIX_CORE_INFO("[Application] Opening the project file de-serialization... from RZApplication resolved path: {0}", projectFullPath);

            RZString physicalPath;
            if (!RZVirtualFileSystem::Get().resolvePhysicalPath(projectFullPath, physicalPath)) {
                RAZIX_CORE_ERROR("[Application] Failed to resolve path for *.razixproject: {0}", projectFullPath);
                RAZIX_CORE_ERROR("Project File does not exist!");
                RAZIX_CORE_TRACE("Creating a default Project file...");
                SaveApp();
            }
            auto           jsonStrData = RZFileSystem::ReadTextFile(physicalPath);
            nlohmann::ordered_json data        = nlohmann::ordered_json::parse(jsonStrData);

            loadRazixProject(data);
        }

        // Mount the VFS paths based on the Project directory (done here cause the Application can make things easier by making this easy by loading some default directories, others can be added later sandbox shouldn't be troubled by all this labor work)
        // First the default sandbox or sample project is loaded that is provided by the engine that resides with the engine
        // Next it checks the command line for the project file directory
        // Project root directory
        RAZIX_CORE_TRACE("Mounting file systems... for Project at : {0}", m_ProjectPath);
        Razix::RZSplashScreen::Get().setLogString("Mounting file systems...");

        RZVirtualFileSystem::Get().mount("Project", m_ProjectPath);
        RZVirtualFileSystem::Get().mount("Assets", m_ProjectPath + RZString("/Assets"));
        RZVirtualFileSystem::Get().mount("Meshes", m_ProjectPath + RZString("/Assets/Meshes"));
        RZVirtualFileSystem::Get().mount("Scenes", m_ProjectPath + RZString("/Assets/Scenes"));
        RZVirtualFileSystem::Get().mount("Scripts", m_ProjectPath + RZString("/Assets/Scripts"));
        RZVirtualFileSystem::Get().mount("Sounds", m_ProjectPath + RZString("/Assets/Sounds"));
        RZVirtualFileSystem::Get().mount("Textures", m_ProjectPath + RZString("/Assets/Textures"));
        RZVirtualFileSystem::Get().mount("Materials", m_ProjectPath + RZString("/Assets/Materials"));

        // The Razix Application Signature Name is generated here and passed to the window
        // Set the window properties and create the timer
        m_WindowProperties.Title = GetAppWindowTitleSignature(m_ProjectName);
        if (m_Window == NULL)
            m_Window = RZWindow::Create(m_WindowProperties);
        m_Window->SetEventCallback(RAZIX_BIND_CB_EVENT_FN(RZApplication::OnEvent));

        // If we change the API, then update the window title
        m_Window->setTitle(GetAppWindowTitleSignature(m_ProjectName).c_str());

        m_CurrentState = AppState::kLoading;
    }

    void RZApplication::OnEvent(RZEvent& event)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        m_EventDispatcher.dispatch(event);
    }

    bool RZApplication::OnWindowClose(WindowCloseEvent& e)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        m_CurrentState = AppState::kClosing;
        return true;
    }

    bool RZApplication::OnWindowResize(RZWindowResizeEvent& e)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            // Resize ImGui
            ImGuiIO& io                = ImGui::GetIO();
            io.DisplaySize             = ImVec2(static_cast<f32>(e.GetWidth()), static_cast<f32>(e.GetHeight()));
            io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        }

        // Resize the frame graph resource before resizing the RHI
        Razix::RZEngine::Get().getWorldRenderer().OnResize(e.GetWidth(), e.GetHeight());

        OnResize(e.GetWidth(), e.GetHeight());
        return true;
    }

    bool RZApplication::OnMouseMoved(RZMouseMovedEvent& e)
    {
#if ENABLE_IMGUI_EVENT_DATA_CAPTURE
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
#if ENABLE_IMGUI_EVENT_DATA_CAPTURE
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
#if ENABLE_IMGUI_EVENT_DATA_CAPTURE
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
#if ENABLE_IMGUI_EVENT_DATA_CAPTURE
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
#if ENABLE_IMGUI_EVENT_DATA_CAPTURE
        auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            ImGuiIO& io                 = ImGui::GetIO();
            io.KeysDown[e.GetKeyCode()] = false;
        }
#endif
        return true;
    }

    void RZApplication::Begin()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        Razix::RZSplashScreen::Get().setLogString("Loading Scene Graph...");
        // Load a scene into memory
        //Razix::RZSplashScreen::Get().setLogString("Scene Loading Successful...");

        Razix::RZSplashScreen::Get().setLogString("Creating world renderer");
        Razix::RZEngine::Get().getWorldRenderer().create(m_Window, m_Window->getWidth(), m_Window->getHeight());

        if (RZEngine::Get().isEngineInTestMode() == false) {
            Razix::RZSplashScreen::Get().setLogString("Building FrameGraph...");
            Razix::RZEngine::Get().getWorldRenderer().buildFrameGraph(Razix::RZEngine::Get().getWorldSettings(), NULL);
        }

        m_CurrentState = AppState::kRunning;

        Razix::RZSplashScreen::Get().setLogString("Starting Razix Application...");
        Razix::RZSplashScreen::Get().ShutDown();

        m_EventDispatcher.registerCallback<WindowCloseEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowClose));
        m_EventDispatcher.registerCallback<RZWindowResizeEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowResize));
        m_EventDispatcher.registerCallback<RZMouseMovedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseMoved));
        m_EventDispatcher.registerCallback<RZMouseButtonPressedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseButtonPressed));
        m_EventDispatcher.registerCallback<RZMouseButtonReleasedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseButtonReleased));
        m_EventDispatcher.registerCallback<RZKeyPressedEvent>(RAZIX_BIND_CB_EVENT_FN(OnKeyPress));
        m_EventDispatcher.registerCallback<RZKeyReleasedEvent>(RAZIX_BIND_CB_EVENT_FN(OnKeyRelease));

        //-----------------
        // Start the Engine Client side!
        Start();
        //-----------------
    }

    bool RZApplication::RenderFrame()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);
        const char* mainThreadName_Tanu = g_ThreadNames_Tanu[RZ_THREAD_NAME_TANU_GAME_MAIN];
        RAZIX_PROFILE_FRAMEMARKER(mainThreadName_Tanu);
        RAZIX_PROFILE_SETTHREADNAME(mainThreadName_Tanu);
        rz_thread_set_name(mainThreadName_Tanu);

        RZEngine::Get().getFrameAllocator().beginFrame();

        // TODO: Add Time stamp Queries for calculating GPU time

        rz_time_stamp currTime = rz_time_now();
        RZEngine::Get().ResetStats();
        auto& stats = RZEngine::Get().GetStatistics();
        m_FPSTimestep.Update(currTime);
        m_UPSTimestep.Update(currTime);

        // Update the stats
        stats.DeltaTime = m_FPSTimestep.GetTimestepMs();

        // Poll for Input events
        m_Window->ProcessInput();

        // Early close if the escape key is pressed or close button is pressed
        if (RZInput::IsKeyPressed(Razix::KeyCode::Key::Escape))
            m_CurrentState = AppState::kClosing;

        if (m_CurrentState == AppState::kClosing)
            return false;

        // Reload shaders and FrameGraph resources
        if (RZInput::IsKeyPressed(Razix::KeyCode::Key::R)) {
            RAZIX_CORE_INFO("Reloading FrameGraph...");
            //Gfx::RZShaderLibrary::Get().reloadShadersFromDisk();
            //auto& worldRenderer = Razix::RZEngine::Get().getWorldRenderer();
            //worldRenderer.destroy();
            //Razix::Gfx::RZFrameGraph::ResetFirstFrame();
            //worldRenderer.buildFrameGraph(Razix::RZEngine::Get().getWorldSettings(), RZSceneManager::Get().getCurrentSceneMutablePtr());
            //RAZIX_CORE_INFO("FrameGraph reload Done!");
        }

        // Update the Engine systems
        Update(m_UPSTimestep);

        // Render the Graphics
        Render();
        m_Frames++;

        // RenderGUI
        RenderGUI();

        // Update the window and it's surface/video out
        m_Window->OnWindowUpdate();

        {
            RAZIX_PROFILE_SCOPEC("RZApplication::TimeStepUpdates", RZ_PROFILE_COLOR_APPLICATION);

            // Record the FPS
            if (rz_get_elapsed_ms(m_TotalTimeElapsedInSeconds, currTime) > 1000.0f) {
                m_TotalTimeElapsedInSeconds = currTime;

                stats.FramesPerSecond  = (u32) m_FPSTimestep.GetCurrentFPS();
                stats.UpdatesPerSecond = (u32) m_UPSTimestep.GetCurrentFPS();
                //RAZIX_CORE_TRACE("FPS : {0} (dt: {1}ms) | Avg FPS: {2}", stats.FramesPerSecond, stats.DeltaTime, (u32) m_FPSTimestep.GetAverageFPS());
                //RAZIX_CORE_TRACE("UPS : {0} ms", stats.UpdatesPerSecond);

                // update window signature with FPS
                auto sig = GetAppWindowTitleSignature(m_ProjectName) + " | FPS: " + rz_to_string(stats.FramesPerSecond) + " Avg. FPS: " + rz_to_string((u32) m_FPSTimestep.GetAverageFPS());
                m_Window->setTitle(sig.c_str());
            }
        }

        RZEngine::Get().getFrameAllocator().endFrame();

        return m_CurrentState != AppState::kClosing;
    }

    void RZApplication::Start()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        OnStart();

        // Run the OnStart method for all the scripts in the scene
        //if (RZSceneManager::Get().getCurrentSceneMutablePtr())
        //RZEngine::Get().getScriptHandler().OnStart(NULL);
    }

    void RZApplication::Update(const RZTimestep& dt)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        // TODO: Check if it's the primary or not and make sure you render only to the Primary Camera, if not then don't render!!!!
        // TODO: Update the renderer stuff here
        // TODO: Update Scene Graph here

        auto ctx = ImGui::GetCurrentContext();
        if (ctx) {
            // Update ImGui
            ImGuiIO& io                = ImGui::GetIO();
            io.DisplaySize             = ImVec2(static_cast<f32>(getWindow()->getWidth()), static_cast<f32>(getWindow()->getHeight()));
            io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        }

        // Client App Update
        OnUpdate(dt);
    }

    void RZApplication::Render()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        OnRender();

        Razix::RZEngine::Get().getWorldRenderer().drawFrame(Razix::RZEngine::Get().getWorldSettings(), NULL);
    }

    void RZApplication::RenderGUI()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        auto ctx = ImGui::GetCurrentContext();
        if (!ctx || RZEngine::Get().isEngineInTestMode())
            return;

        // TODO: Well GLFW needs to be removed at some point and we need to use native functions
        ImGui_ImplGlfw_NewFrame();

        // FIXME: https://github.com/ocornut/imgui/issues/6064

        // Update ImGui
        ImGuiIO& io = ImGui::GetIO();
        (void) io;

        ImGui::NewFrame();
        //ImGuizmo::BeginFrame();

        //ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        // World Renderer Tools
        RZEngine::Get().getWorldRenderer().OnImGui();

        // User GUI
        RZEngine::Get().getScriptHandler().OnImGui(NULL);

        // Client side
        OnImGui();

        // Engine App GUI
        renderEngineStatsOnImGui();
    }

    void RZApplication::Quit()
    {
        // Client side quit customization
        OnQuit();

        Razix::RZEngine::Get().getWorldRenderer().destroy();

        // Save the scene and the Application
        SaveApp();

        RAZIX_CORE_ERROR("Closing Application!");
    }

    struct RZStringSerialize
    {
        std::string value;

        RZStringSerialize() = default;
        explicit RZStringSerialize(const RZString& str)
            : value(str.c_str()) {}

        operator RZString() const { return RZString(value.c_str()); }
    };

    /* Application Serialization */
#define RZ_JSON_KEY_ROOT           "Razix Application"
#define RZ_JSON_KEY_PROJECT_NAME   "Project Name"
#define RZ_JSON_KEY_ENGINE_VERSION "Engine Version"
#define RZ_JSON_KEY_PROJECT_ID     "Project ID"
#define RZ_JSON_KEY_WIDTH          "Width"
#define RZ_JSON_KEY_HEIGHT         "Height"
#define RZ_JSON_KEY_SCENES         "Scenes"

    static bool ValidateRequiredProjectFields(const nlohmann::ordered_json& root)
    {
        const bool ok =
            root.contains(RZ_JSON_KEY_PROJECT_NAME) && root[RZ_JSON_KEY_PROJECT_NAME].is_string() &&
            root.contains(RZ_JSON_KEY_ENGINE_VERSION) && root[RZ_JSON_KEY_ENGINE_VERSION].is_string() &&
            root.contains(RZ_JSON_KEY_PROJECT_ID) && root[RZ_JSON_KEY_PROJECT_ID].is_string() &&
            root.contains(RZ_JSON_KEY_WIDTH) && root[RZ_JSON_KEY_WIDTH].is_number_unsigned() &&
            root.contains(RZ_JSON_KEY_HEIGHT) && root[RZ_JSON_KEY_HEIGHT].is_number_unsigned();

        if (!ok) {
            RAZIX_CORE_ERROR("[Serialization] Missing or invalid required fields. Aborting project load.");
        }
        return ok;
    }

    void RZApplication::loadRazixProject(const nlohmann::ordered_json& j)
    {
        if (!j.contains(RZ_JSON_KEY_ROOT) || !j[RZ_JSON_KEY_ROOT].is_object()) {
            RAZIX_CORE_ERROR("[Serialization] Missing root object '{}'.", RZ_JSON_KEY_ROOT);
            m_SceneFilePaths.clear();
            return;
        }
        const auto& root = j[RZ_JSON_KEY_ROOT];

        if (!ValidateRequiredProjectFields(root)) {
            m_SceneFilePaths.clear();
            return;
        }

        m_ProjectName = RZString(root[RZ_JSON_KEY_PROJECT_NAME].get<std::string>().c_str());

        RZString              storedVersionStr(root[RZ_JSON_KEY_ENGINE_VERSION].get<std::string>().c_str());
        const Razix::Version  loadedVersion  = Version::ParseVersionString(storedVersionStr);
        const Razix::Version& currentVersion = Razix::RazixVersion;

        if (storedVersionStr != currentVersion.getVersionString()) {
            RAZIX_CORE_WARN("[Serialization] Engine version mismatch detected!");
            RAZIX_CORE_WARN("[Serialization] Loaded asset was saved with version: {}", storedVersionStr);
            RAZIX_CORE_WARN("[Serialization] Current engine version is: {}", currentVersion.getVersionString());

            if (loadedVersion.getVersionMajor() < currentVersion.getVersionMajor()) {
                RAZIX_CORE_ERROR("[Serialization] Major version is older! Incompatibility likely!");
            } else if (loadedVersion.getVersionMinor() < currentVersion.getVersionMinor()) {
                RAZIX_CORE_ERROR("[Serialization] Minor version is older! May be partially compatible.");
            } else if (loadedVersion.getVersionPatch() < currentVersion.getVersionPatch()) {
                RAZIX_CORE_ERROR("[Serialization] Patch version is older! Usually safe, but changes may exist.");
            } else {
                RAZIX_CORE_ERROR("[Serialization] Version is newer than current engine! Unsupported forward compatibility.");
            }
        } else {
            RAZIX_CORE_INFO("[Serialization] Engine version matches exactly: {}", storedVersionStr);
        }

        {
            RZString uuid_string(root[RZ_JSON_KEY_PROJECT_ID].get<std::string>().c_str());
            m_ProjectID = rz_uuid_from_pretty_str(uuid_string.c_str());
        }

        m_WindowProperties.Width  = static_cast<u32>(root[RZ_JSON_KEY_WIDTH].get<u32>());
        m_WindowProperties.Height = static_cast<u32>(root[RZ_JSON_KEY_HEIGHT].get<u32>());

        m_SceneFilePaths.clear();
        if (root.contains(RZ_JSON_KEY_SCENES) && root[RZ_JSON_KEY_SCENES].is_array()) {
            for (const auto& pathJson: root[RZ_JSON_KEY_SCENES]) {
                if (pathJson.is_string()) {
                    m_SceneFilePaths.push_back(RZString(pathJson.get<std::string>().c_str()));
                } else {
                    RAZIX_CORE_WARN("[Serialization] Skipping non-string scene entry in project file.");
                }
            }
        } else {
            RAZIX_CORE_WARN("[Serialization] No scenes array found; scene list cleared.");
        }

        RAZIX_CORE_ASSERT(!m_ProjectName.empty(), "Project name must not be empty after load.");
        RAZIX_CORE_ASSERT(m_WindowProperties.Width > 0 && m_WindowProperties.Height > 0, "Window size must be non-zero after load.");
    }

    void RZApplication::saverazixproject(nlohmann::ordered_json& j) const
    {
        nlohmann::ordered_json root = nlohmann::ordered_json::object();

        const u32 width  = m_Window ? m_Window->getWidth() : m_WindowProperties.Width;
        const u32 height = m_Window ? m_Window->getHeight() : m_WindowProperties.Height;

        root[RZ_JSON_KEY_PROJECT_NAME]   = std::string(m_ProjectName.c_str());
        root[RZ_JSON_KEY_ENGINE_VERSION] = std::string(Razix::RazixVersion.getVersionString().c_str());
        char uuid_str[37];
        rz_uuid_to_pretty_str(&m_ProjectID, uuid_str);
        root[RZ_JSON_KEY_PROJECT_ID]     = std::string(uuid_str);
        root[RZ_JSON_KEY_WIDTH]          = width;
        root[RZ_JSON_KEY_HEIGHT]         = height;

        nlohmann::ordered_json scenesJson = nlohmann::ordered_json::array();
        for (const auto& path: m_SceneFilePaths) {
            scenesJson.push_back(std::string(path.c_str()));
        }
        root[RZ_JSON_KEY_SCENES] = std::move(scenesJson);

        j[RZ_JSON_KEY_ROOT] = std::move(root);

        RAZIX_CORE_ASSERT(j[RZ_JSON_KEY_ROOT][RZ_JSON_KEY_PROJECT_NAME].is_string(), "Project Name must serialize as string.");
        RAZIX_CORE_ASSERT(j[RZ_JSON_KEY_ROOT][RZ_JSON_KEY_ENGINE_VERSION].is_string(), "Engine Version must serialize as string.");
        RAZIX_CORE_ASSERT(j[RZ_JSON_KEY_ROOT][RZ_JSON_KEY_PROJECT_ID].is_string(), "Project ID must serialize as string.");
        RAZIX_CORE_ASSERT(j[RZ_JSON_KEY_ROOT][RZ_JSON_KEY_WIDTH].is_number_unsigned() && j[RZ_JSON_KEY_ROOT][RZ_JSON_KEY_WIDTH].get<u32>() > 0, "Width must be > 0.");
        RAZIX_CORE_ASSERT(j[RZ_JSON_KEY_ROOT][RZ_JSON_KEY_HEIGHT].is_number_unsigned() && j[RZ_JSON_KEY_ROOT][RZ_JSON_KEY_HEIGHT].get<u32>() > 0, "Height must be > 0.");
    }

    void RZApplication::SaveApp()
    {
        // Save the app data before closing
        RAZIX_CORE_WARN("Saving App...");
        RZString       projectFullPath = m_ProjectPath + m_ProjectName + RZString(".razixproject");
        nlohmann::ordered_json data;
        saverazixproject(data);

        std::string out = data.dump(4);    // pretty print with indent 4
        const i64   sz  = static_cast<i64>(out.size());
        if (!RZFileSystem::WriteFile(projectFullPath, reinterpret_cast<const u8*>(out.c_str()), sz)) {
            RAZIX_CORE_ERROR("[Application] Failed to write project file: {0}", projectFullPath);
            RAZIX_DEBUG_BREAK();
            return;
        }

        RAZIX_CORE_INFO("[Application] Project saved successfully: {0}", projectFullPath);
    }

    void RZApplication::renderEngineImGuiElements()
    {
        renderRuntimeAssetsIconsOnImGui();
        renderEngineStatsOnImGui();
    }

    void RZApplication::renderRuntimeAssetsIconsOnImGui()
    {
#if 0
        // Guizmo Controls for an Entity
        if (m_EnableGuizmoEditing) {
            auto currentScene = RZSceneManager::Get().getCurrentSceneMutablePtr();
            //auto&          registry     = currentScene->getRegistry();
            //auto           cameraView   = registry.view<CameraComponent>();
            //RZSceneCamera* cam          = nullptr;
            //if (!cameraView.empty()) {
            //    // By using front we get the one and only or the first one in the list of camera entities
            //    cam = &cameraView.get<CameraComponent>(cameraView.front()).Camera;
            //}

//            auto& cam = currentScene->getSceneCamera();

            // Guizmo Editing Here
            TransformComponent& tc              = m_GuizmoEntity.GetComponent<TransformComponent>();
            float4x4           transformMatrix = tc.GetLocalTransform();
//            float4x4           deltaMatrix     = float4x4(1.0f);

            //ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, value_ptr(transformMatrix));

            // https://github.com/CedricGuillemet/ImGuizmo/issues/237
            //ImGuizmo::Manipulate(value_ptr(cam.getViewMatrix()), value_ptr(cam.getProjectionRaw()), (ImGuizmo::OPERATION) m_GuizmoOperation, (ImGuizmo::MODE) m_GuizmoMode, value_ptr(transformMatrix), value_ptr(deltaMatrix), &m_GuizmoSnapAmount);

            f32 matrixTranslation[3], matrixRotation[3], matrixScale[3];
            //ImGuizmo::DecomposeMatrixToComponents(&(transformMatrix[0][0]), matrixTranslation, matrixRotation, matrixScale);

            tc.Translation = float3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
            tc.Rotation    = float3(radians(matrixRotation[0]), radians(matrixRotation[1]), radians(matrixRotation[2]));
            tc.Scale       = float3(matrixScale[0], matrixScale[1], matrixScale[2]);
            tc.Transform   = transformMatrix;
        }

        // TODO: As for Icons of the components or any other entities we will get them using the entt
        // Get their position in the worldspace and check it against the camera frustum and
        // convert it to screen space and render a non-clickable ImGui::Button with the FontIcon as image
#endif
    }

    void RZApplication::renderEngineStatsOnImGui()
    {
        // Engine Stats
        {
            // Engine stats
            ImGuiWindowFlags     window_flags     = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
            const f32            DISTANCE         = 10.0f;
            const ImGuiViewport* viewport         = ImGui::GetMainViewport();
            ImVec2               work_area_pos    = viewport->WorkPos;    // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2               work_area_size   = viewport->WorkSize;
            ImVec2               window_pos       = ImVec2((1 & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : (work_area_pos.x + DISTANCE), (1 & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
            ImVec2               window_pos_pivot = ImVec2((1 & 1) ? 1.0f : 0.0f, (1 & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
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
        ImGui::Render();
    }
}    // namespace Razix
