// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZApplication.h"

// ---------- Engine ----------
#include "Razix/Core/RZEngine.h"
// ----------------------------

#include "Razix/Core/SplashScreen/RZSplashScreen.h"
#include "Razix/Core/Version/RazixVersion.h"

#include "Razix/Core/Memory/RZCPUMemoryManager.h"

#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Events/ApplicationEvent.h"

#include "Razix/Scene/Components/CameraComponent.h"
#include "Razix/Scene/Components/LightComponent.h"
#include "Razix/Scene/Components/TransformComponent.h"

#include <backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/plugins/IconsFontAwesome5.h>
//#include <imgui/plugins/ImGuizmo.h>
#include <GLFW/glfw3.h>
#include <cereal/archives/json.hpp>
#include <entt.hpp>

#include <fstream>
#include <iostream>

#define ENABLE_IMGUI_EVENT_DATA_CAPTURE 0

namespace Razix {
    RZApplication* RZApplication::s_AppInstance = nullptr;

    static RZString GetAppWindowTitleSignature(const RZString& projectName)
    {
        RZString SignatureTitle = projectName + " | " + "Razix Engine" + " - " + Razix::RazixVersion.getVersionString() + " " + "[" + Razix::RazixVersion.getReleaseStageString() + "]" + " " + "<" + rzGfxCtx_GetRenderAPIString() + ">" + " | " + " " + RAZIX_STRINGIZE(RAZIX_BUILD_CONFIG);
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
        std::ifstream AppStream;
        if (RZEngine::Get().getCommandLineParser().isSet("project file name") && RZEngine::Get().getCommandLineParser().isSet("project file path")) {
            RZString projectPath = RZEngine::Get().getCommandLineParser().getValueAsString("project file path");
            RZString projectName = RZEngine::Get().getCommandLineParser().getValueAsString("project file name");
            RZString fullPath    = projectPath + projectName + RZString(".razixproject");
            RAZIX_CORE_TRACE("[Application] Command line resolved full project path : {0}", fullPath);
            RAZIX_CORE_INFO("[Application] Opening the project file de-serialization...");
            AppStream.open(fullPath.c_str(), std::ifstream::in);
            m_ProjectPath = projectPath;
        } else {
            RAZIX_CORE_WARN("[Application] command line args for project file path and name are not set...using App args to resolve *.razixproject");
            // TODO: If command line is not provided or doesn't use engine default sandbox project we need some way to resolve the project root directory, make this agnostic we need not redirect to sandbox by default it must be provided as a placeholder value instead as a fall back
            //m_AppFilePath = ??
            RZString projectFullPath = m_ProjectPath + "/" + m_ProjectName + RZString(".razixproject");
            RAZIX_CORE_INFO("[Application] Opening the project file de-serialization... from RZApplication resolved path: {0}", projectFullPath);
            AppStream.open(projectFullPath.c_str(), std::ifstream::in);
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

        // De-serialize the application
        if (AppStream.is_open()) {
            RAZIX_CORE_TRACE("Loading project file...");
            cereal::JSONInputArchive inputArchive(AppStream);
            inputArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }

        m_Timer = rzstl::CreateUniqueRef<RZTimer>();

        // The Razix Application Signature Name is generated here and passed to the window
        // Set the window properties and create the timer
        m_WindowProperties.Title = GetAppWindowTitleSignature(m_ProjectName);
        if (m_Window == nullptr)
            m_Window = RZWindow::Create(m_WindowProperties);
        m_Window->SetEventCallback(RAZIX_BIND_CB_EVENT_FN(RZApplication::OnEvent));

        // Create a default project file file if nothing exists
        if (!AppStream.is_open()) {
            RAZIX_CORE_ERROR("Project File does not exist!");
            RZString                  projectFullPath = m_ProjectPath + m_ProjectName + RZString(".razixproject");
            std::ofstream             opAppStream(projectFullPath.c_str());
            cereal::JSONOutputArchive defArchive(opAppStream);
            RAZIX_CORE_TRACE("Creating a default Project file...");

            defArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }

        // If we change the API, then update the window title
        m_Window->setTitle(GetAppWindowTitleSignature(m_ProjectName).c_str());

        m_CurrentState = AppState::Loading;
    }

    void RZApplication::OnEvent(RZEvent& event)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        m_EventDispatcher.dispatch(event);
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

        Razix::RZSplashScreen::Get().setLogString("Loading Scene...");
        // Now the scenes are loaded onto the scene manger here but they must be STATIC INITIALIZED shouldn't depend on the start up for the graphics context
        for (auto& sceneFilePath: sceneFilePaths)
            RZSceneManager::Get().enqueueSceneFromFile(sceneFilePath);

        // Load a scene into memory
        RZSceneManager::Get().loadScene(0);
        Razix::RZSplashScreen::Get().setLogString("Scene Loading Successful...");

        Razix::RZSplashScreen::Get().setLogString("Creating world renderer");
        Razix::RZEngine::Get().getWorldRenderer().create(m_Window, m_Window->getWidth(), m_Window->getHeight());

        if (RZEngine::Get().isEngineInTestMode() == false) {
            Razix::RZSplashScreen::Get().setLogString("Building FrameGraph...");
            Razix::RZEngine::Get().getWorldRenderer().buildFrameGraph(Razix::RZEngine::Get().getWorldSettings(), RZSceneManager::Get().getCurrentSceneMutablePtr());
        }

        m_CurrentState = AppState::Running;

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
        RAZIX_PROFILE_FRAMEMARKER("RZApplication Main Thread");
        // Naming threads have been inspired from mamoniem on twitter, can't find the particular tweet
        RAZIX_PROFILE_SETTHREADNAME("MainThread::Kratos");

        // TODO: Add Time stamp Queries for calculating GPU time

        // Calculate the delta time
        f32 now = m_Timer->GetElapsedS();
        RZEngine::Get().ResetStats();
        auto& stats = RZEngine::Get().GetStatistics();
        m_FPSTimestep.Update(now);
        m_UPSTimestep.Update(now);

        // Update the stats
        stats.DeltaTime = m_FPSTimestep.GetTimestepMs();

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
            if (now - m_TotalTimeElapsedInSeconds > 1.0f) {
                m_TotalTimeElapsedInSeconds += 1.0f;

                stats.FramesPerSecond  = (u32) m_FPSTimestep.GetCurrentFPS();
                stats.UpdatesPerSecond = (u32) m_UPSTimestep.GetCurrentFPS();
                //RAZIX_CORE_TRACE("FPS : {0} (dt: {1}ms) | Avg FPS: {2}", stats.FramesPerSecond, stats.DeltaTime, (u32) m_FPSTimestep.GetAverageFPS());
                //RAZIX_CORE_TRACE("UPS : {0} ms", stats.UpdatesPerSecond);

                // update window signature with FPS
                auto sig = GetAppWindowTitleSignature(m_ProjectName) + " | FPS: " + rz_to_string(stats.FramesPerSecond) + " Avg. FPS: " + rz_to_string((u32) m_FPSTimestep.GetAverageFPS());
                m_Window->setTitle(sig.c_str());
            }
        }
        return m_CurrentState != AppState::Closing;
    }

    void RZApplication::Start()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        OnStart();

        // Run the OnStart method for all the scripts in the scene
        if (RZSceneManager::Get().getCurrentSceneMutablePtr())
            RZEngine::Get().getScriptHandler().OnStart(RZSceneManager::Get().getCurrentSceneMutablePtr());
    }

    void RZApplication::Update(const RZTimestep& dt)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        // TODO: Check if it's the primary or not and make sure you render only to the Primary Camera, if not then don't render!!!!
        // Update the renderer stuff here
        // Update Scene Graph here
        RZScene* CurrentScene = RZSceneManager::Get().getCurrentSceneMutablePtr();
        CurrentScene->update();
        // Update the Scene Camera Here
        CurrentScene->getSceneCamera().update(dt.GetTimestepMs());
        CurrentScene->getSceneCamera().setAspectRatio(f32(m_Window->getWidth()) / f32(m_Window->getHeight()));

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

        Razix::RZEngine::Get().getWorldRenderer().drawFrame(Razix::RZEngine::Get().getWorldSettings(), RZSceneManager::Get().getCurrentSceneMutablePtr());
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
        if (RZSceneManager::Get().getCurrentSceneMutablePtr())
            RZEngine::Get().getScriptHandler().OnImGui(RZSceneManager::Get().getCurrentSceneMutablePtr());

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
        RZSceneManager::Get().saveAllScenes();
        RZSceneManager::Get().destroyAllScenes();
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
    // Load mechanism for the RZApplication class
    template<class Archive>
    void RZApplication::load(Archive& archive)
    {
        // Load project name as std::string, then convert to RZString
        std::string projectName;
        archive(cereal::make_nvp("Project Name", projectName));
        m_ProjectName = RZString(projectName.c_str());
        RAZIX_ASSERT_MESSAGE((m_ProjectName == RZString(projectName.c_str())), "Project name doesn't match with Executable");

        /**
     * Currently the project name will be verified with the one given in the sandbox or game project
     * If it doesn't match it updates the project name, it's not necessary that the name must match the 
     * executable name, since the Editor can load any *.razixproject file, this should also mean that
     * it should be able to load any project name since the project name is always mutable just all it's properties
     * There's not enforcement on the project names and other properties, the Razix Editor Application 
     * and can load any thing as long it is supplies with the required data to
     */

        // Deserialize engine version from std::string
        std::string versionStr;
        archive(cereal::make_nvp("Engine Version", versionStr));
        RZString storedVersionStr(versionStr.c_str());

        const Razix::Version  loadedVersion  = Version::ParseVersionString(storedVersionStr);
        const Razix::Version& currentVersion = Razix::RazixVersion;

        if (storedVersionStr != currentVersion.getVersionString()) {
            RAZIX_CORE_WARN("[Serialization] Engine version mismatch detected!");
            RAZIX_CORE_WARN("[Serialization] Loaded asset was saved with version: {}", storedVersionStr);
            RAZIX_CORE_WARN("[Serialization] Current engine version is: {}", currentVersion.getVersionString());

            if (loadedVersion.getVersionMajor() < currentVersion.getVersionMajor()) {
                RAZIX_CORE_ERROR("[Serialization] Major version is older! incompatibility likely!");
            } else if (loadedVersion.getVersionMinor() < currentVersion.getVersionMinor()) {
                RAZIX_CORE_ERROR("[Serialization] Minor version is older! may be partially compatible.");
            } else if (loadedVersion.getVersionPatch() < currentVersion.getVersionPatch()) {
                RAZIX_CORE_ERROR("[Serialization] Patch version is older! usually safe, but changes may exist.");
            } else {
                RAZIX_CORE_ERROR("[Serialization] Version is newer than current engine! unsupported forward compatibility.");
            }
        } else {
            RAZIX_CORE_INFO("[Serialization] Engine version matches exactly: {}", storedVersionStr);
        }

        // TODO: Verify these also!
        //archive(cereal::make_nvp("Project Version", 0));
        u32 Width, Height;
        archive(cereal::make_nvp("Width", Width));
        archive(cereal::make_nvp("Height", Height));
        m_WindowProperties.Width  = Width;
        m_WindowProperties.Height = Height;

        // Extract the project UUID from std::string
        std::string uuid_stringStr;
        archive(cereal::make_nvp("Project ID", uuid_stringStr));
        RZString uuid_string(uuid_stringStr.c_str());
        m_ProjectID = RZUUID::FromPrettyStrFactory(uuid_string);

        // Load the scenes from the project file for the engine to load and present
        RAZIX_CORE_TRACE("Loading Scenes...");
        std::vector<std::string> scenePaths;
        archive(cereal::make_nvp("Scenes", scenePaths));

        // Convert std::string to RZString
        sceneFilePaths.clear();
        for (const auto& path: scenePaths) {
            sceneFilePaths.push_back(RZString(path.c_str()));
        }
    }

    // Save mechanism for the RZApplication class
    template<class Archive>
    void RZApplication::save(Archive& archive) const
    {
        RAZIX_TRACE("Window Resize override sandbox application! | W : {0}, H : {1}", m_Window->getWidth(), m_Window->getHeight());

        // Save RZString fields as std::string
        archive(cereal::make_nvp("Project Name", std::string(m_ProjectName.c_str())));
        archive(cereal::make_nvp("Engine Version", std::string(Razix::RazixVersion.getVersionString().c_str())));
        archive(cereal::make_nvp("Project ID", std::string(m_ProjectID.prettyString().c_str())));
        archive(cereal::make_nvp("Width", m_Window->getWidth()));
        archive(cereal::make_nvp("Height", m_Window->getHeight()));

        auto& paths = RZSceneManager::Get().getSceneFilePaths();

        // Convert RZString vector to std::string vector for serialization
        std::vector<std::string> scenePaths;
        for (const auto& path: paths) {
            scenePaths.push_back(std::string(path.c_str()));
        }
        archive(cereal::make_nvp("Scenes", scenePaths));
    }

    void RZApplication::SaveApp()
    {
        // Save the app data before closing
        RAZIX_CORE_WARN("Saving project...");
        RZString                  projectFullPath = m_ProjectPath + m_ProjectName + RZString(".razixproject");
        std::ofstream             opAppStream(projectFullPath.c_str());
        cereal::JSONOutputArchive saveArchive(opAppStream);
        saveArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
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
