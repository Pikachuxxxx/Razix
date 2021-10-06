#include "rzxpch.h"
#include "Engine.h"

#include "Razix/Core/OS/VFS.h"
#include "Razix/Core/RazixVersion.h"
#include "Razix/Core/SplashScreen.h"

#include <chrono>

#include <vulkan/vulkan.h>
#include <d3d11.h>

namespace Razix
{
    void Engine::Ignite()
    {
        auto start = std::chrono::high_resolution_clock::now();

        // Log the Engine Ignition
        RAZIX_CORE_INFO("***********************************");
        RAZIX_CORE_INFO("*          Igniting Engine....    *");
        RAZIX_CORE_INFO("***********************************");
        Razix::SplashScreen::Get().SetLogString("Igniting Engine...");

        // TODO: Temp code remove this!!!
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Logging the Engine Version details
        RAZIX_CORE_INFO("Engine Stats : [Version : {0} , Release Stage : {1}, Release Date : {2}]", Razix::RazixVersion.GetVersionString(), Razix::RazixVersion.GetReleaseStageString(), Razix::RazixVersion.GetReleaseDateString());

        //------------------------------//
        // Igniting all the sub-systems //
        //------------------------------//
        // 1. Virtual File System
        VFS::Get().StartUp();

        // 2. Mount engine specific Paths
        // TODO: Either use embedded data for these using .inl files or load them from the Application derived data, Whatever it is remove this by loading from source path
        VFS::Get().Mount("EngineSource", std::string(STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Razix/src/Razix")));

        // Log after all the Engine systems have been successfully Started Up
        RAZIX_CORE_INFO("***********************************");
        RAZIX_CORE_INFO("*          Engine Ignited!        *");
        RAZIX_CORE_INFO("***********************************");
        Razix::SplashScreen::Get().SetLogString("Engine Ignited!");
        Razix::SplashScreen::Get().Destroy();
        
        // TODO: Log the time take to initialize engine using Profiling macros
        auto stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_double = (stop - start);
        RAZIX_CORE_INFO("Engine Ingnited in : {0} ms", ms_double.count());
    }

    void Engine::ShutDown()
    {
        RAZIX_CORE_ERROR("***********************************");
        RAZIX_CORE_ERROR("*    Shutting down Engine....     *");
        RAZIX_CORE_ERROR("***********************************");

        // Shutting down all the sub-systems
        // Shutdown the VFS last
        VFS::ShutDown();

        // Log the completion of engine shutdown
        RAZIX_CORE_ERROR("***********************************");
        RAZIX_CORE_ERROR("*    Engine Shutdown Complete!    *");
        RAZIX_CORE_ERROR("***********************************");
    }

    void Engine::Run()
    {
        UNIMPLEMENTED
    }
}