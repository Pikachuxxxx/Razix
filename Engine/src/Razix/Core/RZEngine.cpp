// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEngine.h"

#include "Razix/Core/SplashScreen/RZSplashScreen.h"
#include "Razix/Core/Version/RazixVersion.h"
#include "Razix/Gfx/Materials/RZMaterial.h"

#include "Razix/Core/Memory/RZCPUMemoryManager.h"

#include "Razix/Utilities/RZiniParser.h"

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
        (void) success;
        RAZIX_CORE_ASSERT(success, "Department/Global Budgets Load Failed!");

        // TODO: Load the Map the default world renderer settings file...the scene can override this
        Utilities::RZiniParser worldSettingsParser;
        success = worldSettingsParser.parse("//RazixConfig/DefaultWorldRendererSettings.ini");
        (void) success;
        RAZIX_CORE_ASSERT(success, "Default World Renderer Settings Load Success!");

        // TODO: Temp code remove this!!!
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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

        // 3. Scene Manager
        RZSceneManager::Get().StartUp();

        // 4. Script Handler
        Scripting::RZLuaScriptHandler::Get().StartUp();

        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Log after all the Engine systems have been successfully Started Up
        RAZIX_CORE_INFO("***********************************");
        RAZIX_CORE_INFO("*          Engine Ignited!        *");
        RAZIX_CORE_INFO("***********************************");

        Razix::RZSplashScreen::Get().setLogString("Engine Ignited!");

        // TODO: Temp code remove this!!!
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Destroy the splash screen since the engine has Ignited successfully!
        //Razix::RZSplashScreen::Get().destroy();

        // TODO: Log the time take to initialize engine using Profiling macros
        auto                                   stop   = std::chrono::high_resolution_clock::now();
        std::chrono::duration<d32, std::milli> ms_d32 = (stop - start);
        RAZIX_CORE_INFO("Engine Ingnited in : {0} ms", ms_d32.count());
    }

    void RZEngine::PostGraphicsIgnite()
    {
        // Post ignition systems that are done after the Graphics are done
        RAZIX_CORE_INFO("***********************************");
        RAZIX_CORE_INFO("*   Post Graphics Ignition....    *");
        RAZIX_CORE_INFO("***********************************");

        // Ignite the shader library after the Graphics has been initialized (Shutdown by RHI when being destroyed)
        Gfx::RZShaderLibrary::Get().StartUp();
    }

    void RZEngine::ShutDown()
    {
        RAZIX_CORE_ERROR("***********************************");
        RAZIX_CORE_ERROR("*    Shutting down Engine....     *");
        RAZIX_CORE_ERROR("***********************************");

        // Shutting down all the sub-systems

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
        Utilities::RZiniParser engineConfigParser;
        // Engine is in the same directory as the executable
        std::string defaultConfigPath    = "//RazixConfig/DefaultEngineConfig.ini";
        bool        skipVFSForConfigLoad = false;
        if (m_CommandLineParser.isSet("engine config filename")) {
            defaultConfigPath    = m_CommandLineParser.getValueAsString("engine config filename");
            skipVFSForConfigLoad = true;
        }
        bool success = engineConfigParser.parse(defaultConfigPath, skipVFSForConfigLoad);
        (void) success;
        if (success) {
            // Rendering Settings
            {
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
                m_EngineSettings.TargetFPSCap = (Gfx::TargetFPS) FPS;

                engineConfigParser.getValue<int>("Rendering", "MaxShadowCascades", m_EngineSettings.MaxShadowCascades);
                engineConfigParser.getValue<int>("Rendering", "MSAASamples", m_EngineSettings.MSAASamples);
            }
        }
    }
}    // namespace Razix
