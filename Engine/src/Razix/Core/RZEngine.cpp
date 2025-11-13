// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEngine.h"

#include "Razix/Core/Containers/string_utils.h"
#include "Razix/Core/Memory/RZMemoryBudgets.h"
#include "Razix/Core/SplashScreen/RZSplashScreen.h"
#include "Razix/Core/Utils/RZPlatformUtils.h"
#include "Razix/Core/Version/RazixVersion.h"

#include "Razix/Core/OS/RZWindow.h"

//#include "Razix/Gfx/Materials/RZMaterial.h"

#include "Razix/Core/Memory/RZCPUMemoryManager.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#ifdef RAZIX_USE_GLFW_WINDOWS
    #include "Razix/Platform/GLFW/GLFWInput.h"
    #include "Razix/Platform/GLFW/GLFWWindow.h"
#endif

#include "Razix/Core/Utils/RZiniParser.h"

namespace Razix {
    void RZEngine::Ignite()
    {
        auto start = std::chrono::high_resolution_clock::now();

        // Log the Engine Ignition
        RAZIX_CORE_INFO("***********************************");
        RAZIX_CORE_INFO("*          Igniting Engine....    *");
        RAZIX_CORE_INFO("***********************************");
        Razix::RZSplashScreen::Get().setLogString("Igniting Engine...");

        // Logging the Engine Version details
        RAZIX_CORE_INFO("Engine Stats : [Version : {0} , Release Stage : {1}, Release Date : {2}]", Razix::RazixVersion.getVersionString(), Razix::RazixVersion.getReleaseStageString(), Razix::RazixVersion.getReleaseDateString());

        //------------------------------//
        // Igniting all the sub-systems //
        //------------------------------//

        // Load the memory budgets
        RAZIX_CORE_INFO("Loading Department/Global Budgets...");
        bool success = Memory::ParseBudgetFile("//RazixConfig/RazixDepartmentBudgets.ini");
        RAZIX_CORE_ASSERT(success, "Department/Global Budgets Load Failed!");

        // TODO: Load the Map the default world renderer settings file...the scene can override this
        RZiniParser worldSettingsParser;
        success = worldSettingsParser.parse("//RazixConfig/DefaultWorldRendererSettings.ini");
        RAZIX_CORE_ASSERT(success, "Default World Renderer Settings Load Success!");

        // print engine SSE/AVX support
        // detect and cache the SIMD support of the CPU
        DetectSIMDSupport();
        RAZIX_CORE_TRACE("Checking endianess...");
        RAZIX_CORE_INFO("Endianess: {0}", GetEndianess() == Endianess::BIG ? "BIG" : "LITTLE");

        RAZIX_CORE_INFO("Runtime SIMD Detection");
#if defined RAZIX_ARCHITECTURE_X64
        RAZIX_CORE_INFO("SSE    : {0}", HasSSE());
        RAZIX_CORE_INFO("SSE2   : {0}", HasSSE2());
        RAZIX_CORE_INFO("SSE3   : {0}", HasSSE3());
        RAZIX_CORE_INFO("SSE4.1 : {0}", HasSSE41());
        RAZIX_CORE_INFO("SSE4.2 : {0}", HasSSE42());
        RAZIX_CORE_INFO("AVX    : {0}", HasAVX());
        RAZIX_CORE_INFO("AVX2   : {0}", HasAVX2());
        RAZIX_CORE_INFO("AVX512 : {0}", HasAVX512());
#elif defined(RAZIX_ARCHITECTURE_ARM64)
        RAZIX_CORE_INFO("NEON   : {0}", HasNEON());
        RAZIX_CORE_INFO("ASIMD  : {0}", HasASIMD());
#endif

        // TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO!
        //--------------------------------------------------------------------------
        // Start Up Memory Managers
        //--------------------------
        //u32 SystemHeapSize = Mib(256);    // For now we only manage 256 Mib
        //RZCPUMemoryManager::Get().Init(SystemHeapSize);
        //
        //u32 VRamInitSize = Mib(256);    // Initializing with 256 Mib of GPU memory
        //Graphics::RZGPUMemoryManager::Get().Init(VRamInitSize);
        //--------------------------------------------------------------------------

        // 2. Sound Engine
        //Audio::RZSoundEngine::Get().StartUp();
        // TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO!

        // 3. Scene Manager
        RZSceneManager::Get().StartUp();

        // 4. Script Handler
        Scripting::RZLuaScriptHandler::Get().StartUp();

        // Done once all kind of default or existing engine config file is loaded
        // command line takes precedence over config file
        if (RZEngine::Get().getCommandLineParser().isSet("vulkan"))
            rzGfxCtx_SetRenderAPI(RZ_RENDER_API_VULKAN);
        else if (RZEngine::Get().getCommandLineParser().isSet("dx12"))
            rzGfxCtx_SetRenderAPI(RZ_RENDER_API_D3D12);

        if (RZEngine::Get().getCommandLineParser().isSet("render api"))
            rzGfxCtx_SetRenderAPI((rz_render_api) RZEngine::Get().getCommandLineParser().getValueAsInt("render api"));

        // 5. Graphics API (last one in the engine to fire up)
        rz_gfx_context_desc gfxCtxDesc   = {};
        gfxCtxDesc.opts.enableValidation = m_EngineSettings.EnableAPIValidation;
        gfxCtxDesc.engineVer.major       = (uint32_t) RazixVersion.getVersionMajor();
        gfxCtxDesc.engineVer.minor       = (uint32_t) RazixVersion.getVersionMinor();
        gfxCtxDesc.engineVer.patch       = (uint32_t) RazixVersion.getVersionPatch();
        // TODO: Send application version as well
        // Cmdline always has precedence
        if (RZEngine::Get().getCommandLineParser().isSet("validation"))
            gfxCtxDesc.opts.enableValidation = true;
        rzGfxCtx_StartUp(gfxCtxDesc);

        Gfx::RZResourceManager::Get().StartUp();

        Gfx::RZShaderLibrary::Get().StartUp();

        // Input setup and window pointers
        // TODO: Use #elif for other platforms
#ifdef RAZIX_USE_GLFW_WINDOWS
        // Select GLFW as the input manager client
        RAZIX_CORE_INFO("Setting up input");
        Razix::RZInput::SelectGLFWInputManager();
        RAZIX_CORE_INFO("GLFW Input system selected!");
        // Set GLFW as window when the Engine API will be called to create the window
        RAZIX_CORE_INFO("Setting up windowing system");
        GLFWWindow::Construct();
        RAZIX_CORE_INFO("GLFW Windowing system selected!");
#endif

        // Log after all the Engine systems have been successfully Started Up
        RAZIX_CORE_INFO("***********************************");
        RAZIX_CORE_INFO("*          Engine Ignited!        *");
        RAZIX_CORE_INFO("***********************************");

        Razix::RZSplashScreen::Get().setLogString("Engine Ignited!");

        // TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO!
        // Destroy the splash screen since the engine has Ignited successfully!
        //Razix::RZSplashScreen::Get().destroy();
        // TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO! TODO!

        auto                                   stop   = std::chrono::high_resolution_clock::now();
        std::chrono::duration<d32, std::milli> ms_d32 = (stop - start);
        RAZIX_CORE_INFO("Engine Ingnited in : {0} ms", ms_d32.count());
        RAZIX_UNUSED(ms_d32);
    }

    void RZEngine::ShutDown()
    {
        RAZIX_CORE_ERROR("***********************************");
        RAZIX_CORE_ERROR("*    Shutting down Engine....     *");
        RAZIX_CORE_ERROR("***********************************");

        // Shutting down all the sub-systems

        Gfx::RZShaderLibrary::Get().ShutDown();

        Gfx::RZResourceManager::Get().ShutDown();

        // 5. Graphics API
        rzGfxCtx_ShutDown();

        // Shutdown the lua script handle
        Scripting::RZLuaScriptHandler::Get().ShutDown();
        // Shutdown the Scene Manager
        RZSceneManager::Get().ShutDown();
        // Shutdown memory systems and free all the memory
        //Graphics::RZGPUMemoryManager::Get().ShutDown();
        // Shutdown the VFS last
        RZVirtualFileSystem::Get().ShutDown();

        // Log the completion of engine shutdown
        RAZIX_CORE_ERROR("***********************************");
        RAZIX_CORE_ERROR("*    Engine Shutdown Complete!    *");
        RAZIX_CORE_ERROR("***********************************");
    }

    void RZEngine::Run()
    {
        RAZIX_UNIMPLEMENTED_METHOD
    }

    void RZEngine::LoadEngineConfigFile()
    {
        RZiniParser engineConfigParser;
        // Engine is in the same directory as the executable
        RZString defaultConfigPath    = "//RazixConfig/DefaultEngineConfig.ini";
        bool     skipVFSForConfigLoad = false;
        if (m_CommandLineParser.isSet("engine config filename")) {
            defaultConfigPath    = m_CommandLineParser.getValueAsString("engine config filename");
            skipVFSForConfigLoad = true;
        }
        bool success = engineConfigParser.parse(defaultConfigPath, skipVFSForConfigLoad);
        if (success) {
            // Rendering Settings
            {
                // Rendering API directly set on RHI
                int renderAPI = -1;
                engineConfigParser.getValue<int>("Rendering", "RenderAPI", renderAPI);
                // Set the render API from the config file
                rzGfxCtx_SetRenderAPI((rz_render_api) renderAPI);

                engineConfigParser.getValue<bool>("Rendering", "EnableAPIValidation", m_EngineSettings.EnableAPIValidation);
                engineConfigParser.getValue<bool>("Rendering", "EnableMSAA", m_EngineSettings.EnableMSAA);
                engineConfigParser.getValue<bool>("Rendering", "EnableBindless", m_EngineSettings.EnableBindless);
                engineConfigParser.getValue<bool>("Rendering", "EnableBarrierLogging", m_EngineSettings.EnableBarrierLogging);

                int perfMode = 0;
                engineConfigParser.getValue<int>("Rendering", "PerfMode", perfMode);
                m_EngineSettings.PerformanceMode = (PerfMode) perfMode;

                int GfxQuality = 0;
                engineConfigParser.getValue<int>("Rendering", "GfxQuality", GfxQuality);
                m_EngineSettings.GfxQuality = (GfxQualityMode) GfxQuality;

                engineConfigParser.getValue<int>("Rendering", "MaxShadowCascades", m_EngineSettings.MaxShadowCascades);

                int FPS = 0;
                engineConfigParser.getValue<int>("Rendering", "TargetFPS", FPS);
                m_EngineSettings.TargetFPSCap = (rz_gfx_target_fps) FPS;

                engineConfigParser.getValue<int>("Rendering", "MaxShadowCascades", m_EngineSettings.MaxShadowCascades);
                engineConfigParser.getValue<int>("Rendering", "MSAASamples", m_EngineSettings.MSAASamples);
            }
        }
    }
}    // namespace Razix
