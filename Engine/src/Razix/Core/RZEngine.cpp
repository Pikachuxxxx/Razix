// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEngine.h"

#include "Razix/Core/RZSplashScreen.h"
#include "Razix/Core/RazixVersion.h"
#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Core/RZCPUMemoryManager.h"
#include "Razix/Graphics/RHI/RZGPUMemoryManager.h"

#include <chrono>

#include <d3d11.h>
#include <vulkan/vulkan.h>

namespace Razix {
    void RZEngine::Ignite()
    {
        auto start = std::chrono::high_resolution_clock::now();

        // Log the Engine Ignition
        RAZIX_CORE_INFO("***********************************");
        RAZIX_CORE_INFO("*          Igniting Engine....    *");
        RAZIX_CORE_INFO("***********************************");
        Razix::RZSplashScreen::Get().setLogString("Igniting Engine...");

        // TODO: Temp code remove this!!!
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Logging the Engine Version details
        RAZIX_CORE_INFO("Engine Stats : [Version : {0} , Release Stage : {1}, Release Date : {2}]", Razix::RazixVersion.getVersionString(), Razix::RazixVersion.getReleaseStageString(), Razix::RazixVersion.getReleaseDateString());

        //------------------------------//
        // Igniting all the sub-systems //
        //------------------------------//
        // 1. Virtual File System
        RZVirtualFileSystem::Get().StartUp();

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

        // Ignite the shader library after the Graphics has been initialized
        Graphics::RZShaderLibrary::Get().StartUp();
        Graphics::RZMaterial::InitDefaultTexture();
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
        std::ifstream config_file("./Engine/content/config/razix_engine.config");
        if (config_file.good()) {
            std::string line;
            std::getline(config_file, line);
            std::cout << line << std::endl;
            auto installationDir    = line.substr(0, line.find("=")).length();
            m_EngineInstallationDir = line.erase(0, installationDir + 1);
            std::cout << m_EngineInstallationDir << std::endl;
        } else {
#ifdef RAZIX_DEBUG
            m_EngineInstallationDir = RAZIX_STRINGIZE(RAZIX_ROOT_DIR);
#elif defined RAZIX_DISTRIBUTION
            m_EngineInstallationDir = "C:/Program Files/Razix";
#endif
        }
    }

}    // namespace Razix