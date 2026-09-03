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

#include "Razix/Gfx/RZRenderThread.h"

#include "Razix/Gfx/House/House.h"

#include "Razix/Scene/RZSceneGraph.h"

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

/* Application Serialization */
#define RZ_JSON_KEY_ROOT           "Razix Application"
#define RZ_JSON_KEY_PROJECT_NAME   "Project Name"
#define RZ_JSON_KEY_ENGINE_VERSION "Engine Version"
#define RZ_JSON_KEY_PROJECT_ID     "Project ID"
#define RZ_JSON_KEY_WIDTH          "Width"
#define RZ_JSON_KEY_HEIGHT         "Height"
#define RZ_JSON_KEY_SCENES         "Scenes"

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
            m_ProjectPath        = projectPath;
            RAZIX_CORE_TRACE("[Application] Command line resolved full project path : {0}", fullPath);
            RAZIX_CORE_INFO("[Application] Opening the project file de-serialization...");

            RAZIX_CORE_TRACE("Loading project file...");
            RZString physicalPath;
            if (!RZVirtualFileSystem::Get().resolvePhysicalPath(fullPath, physicalPath)) {
                RAZIX_CORE_ERROR("[Application] Failed to resolve path for *.razixproject: {0}", fullPath);
                RAZIX_DEBUG_BREAK();
                return;
            }
            auto                   jsonStrData = RZFileSystem::ReadTextFile(physicalPath);
            nlohmann::ordered_json data        = nlohmann::ordered_json::parse(jsonStrData);
            loadRazixProject(data);
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
            auto                   jsonStrData = RZFileSystem::ReadTextFile(physicalPath);
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

        // set the render thread to resize
        rz_atomic32_store(&Razix::Gfx::g_ResizePending, true, RZ_MEMORY_ORDER_RELEASE);

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

        // Build the framegraph when we feel it's good to go, maybe once we build a new world, but typically it must be done everyframe and we compile it everytime
        if (!RZEngine::Get().isEngineInTestMode()) {
            Razix::RZEngine::Get().getWorldRenderer().buildFrameGraph(RZWorld{});
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

        // Launch render thread once we know all app start up is done
        Razix::RZSplashScreen::Get().setLogString("Launching Render Thread...");
        // Launch the render thread here
        m_RenderThread = Gfx::RenderThreadCreate();
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
        m_UPSTimestep.Update(currTime);

        // Update the stats
        stats.DeltaTime = m_UPSTimestep.GetTimestepMs();

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

        m_Frames++;

        // Update the window and it's surface/video out
        m_Window->OnWindowUpdate();

        {
            RAZIX_PROFILE_SCOPEC("RZApplication::TimeStepUpdates", RZ_PROFILE_COLOR_APPLICATION);

            // Record the FPS
            if (rz_get_elapsed_ms(m_TotalTimeElapsedInSeconds, currTime) > 1000.0f) {
                m_TotalTimeElapsedInSeconds = currTime;

                stats.UpdatesPerSecond = (u32) m_UPSTimestep.GetCurrentFPS();
                //RAZIX_CORE_TRACE("FPS : {0} (dt: {1}ms) | Avg FPS: {2}", stats.FramesPerSecond, stats.DeltaTime, (u32) m_FPSTimestep.GetAverageFPS());
                //RAZIX_CORE_TRACE("UPS : {0} ms", stats.UpdatesPerSecond);

                // update window signature with FPS
                auto sig = GetAppWindowTitleSignature(m_ProjectName) + " | FPS: " + rz_to_string(stats.FramesPerSecond);
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

    static void HandoffWorldToRenderThread(const RZWorld& world)
    {
        // Begin transactino of game thread world data to render thread
        u64 renderWorldReadIdx = rz_atomic64_load(&Gfx::g_RenderThreadRingBuffer.m_RenderThreadWorldDataReadCounter, RZ_MEMORY_ORDER_ACQUIRE);
        u64 gameWorldWriteIdx  = rz_atomic64_load(&Gfx::g_RenderThreadRingBuffer.m_GameThreadWorldDataWriteCounter, RZ_MEMORY_ORDER_ACQUIRE);

        if (RZEngine::Get().getGlobalEngineSettings().EnableWorldRingbufferDebugLogs)
            RAZIX_CORE_TRACE("[MAIN THREAD] renderWorldReadIdx: {} and gameWorldWriteIdx: {}", renderWorldReadIdx, gameWorldWriteIdx);

        gameWorldWriteIdx++;

        // if this slot is being used by renderworldreandIdx move ahead more
        while ((gameWorldWriteIdx % RAZIX_WORLDS_IN_FLIGHT) == (renderWorldReadIdx % RAZIX_WORLDS_IN_FLIGHT))
            ++gameWorldWriteIdx;

        u32 worldSlotIdx = gameWorldWriteIdx % RAZIX_WORLDS_IN_FLIGHT;

        if (RZEngine::Get().getGlobalEngineSettings().EnableWorldRingbufferDebugLogs)
            RAZIX_CORE_TRACE("[MAIN THREAD] Writing world data from game thread into idx: {0} | gameWriteCouner@ {1}, \n"
                             "updating counter for rendeerer to read from here",
                worldSlotIdx,
                gameWorldWriteIdx);

        Gfx::g_RenderThreadRingBuffer.worldBuffers[worldSlotIdx] = world;

        // mark it ready tor read once we are done writing the data
        rz_atomic64_store(&Gfx::g_RenderThreadRingBuffer.m_GameThreadWorldDataWriteCounter, gameWorldWriteIdx, RZ_MEMORY_ORDER_RELEASE);
    }

    void RZApplication::Update(const RZTimestep& dt)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        // TODO: [Finish Internal Implementation] Update Scene Graph here and then build the RZWorld 
        // for handoff to Render Thread via atomics and ring buffer
        const rz_scene_graph_manager& sceneGraphMgr   = RZEngine::Get().getSceneManager();
        rz_scene_graph*               currActiveScene = rz_scene_graph_get_active_scene(&sceneGraphMgr);
        if (currActiveScene)
            rz_scene_graph_update(currActiveScene, float3(0, 0, 0), dt.GetTimestepMs());

        // Now use the House API to build the RZWorld and update the global render thread world slot using atomics for safe hand-off
        RZWorld world = Gfx::House::BuildRazixWorldFromSceneData(currActiveScene);
        HandoffWorldToRenderThread(world);

        // Client App Update
        OnUpdate(dt);
    }

    void RZApplication::Quit()
    {
        // Kill render thread and the framegraph before client side can clean up it's resources
        Gfx::RenderThreadDestroy(m_RenderThread);

        // Client side quit customization
        OnQuit();

        // destroy world renderer resources after client side custommization is done
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
        root[RZ_JSON_KEY_PROJECT_ID] = std::string(uuid_str);
        root[RZ_JSON_KEY_WIDTH]      = width;
        root[RZ_JSON_KEY_HEIGHT]     = height;

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
        RZString               projectFullPath = m_ProjectPath + m_ProjectName + RZString(".razixproject");
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

}    // namespace Razix
