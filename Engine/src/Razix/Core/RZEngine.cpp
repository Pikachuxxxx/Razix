#include "rzxpch.h"
#include "RZEngine.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/RazixVersion.h"
#include "Razix/Core/RZSplashScreen.h"

#include <chrono>

#include <vulkan/vulkan.h>
#include <d3d11.h>

namespace Razix
{
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        //  1.1. Mount engine specific Paths
        RZVirtualFileSystem::Get().mount("RazixRoot", std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Engine/")));
        RZVirtualFileSystem::Get().mount("RazixSource", std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Engine/src/")));
        RZVirtualFileSystem::Get().mount("RazixContent", std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Engine/content/")));

        // Log after all the Engine systems have been successfully Started Up
        RAZIX_CORE_INFO("***********************************");
        RAZIX_CORE_INFO("*          Engine Ignited!        *");
        RAZIX_CORE_INFO("***********************************");
        Razix::RZSplashScreen::Get().setLogString("Engine Ignited!");

        // TODO: Temp code remove this!!!
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Destroy the splash screen since the engine has Ignited successfully!
        //Razix::RZSplashScreen::Get().destroy();
        
        // TODO: Log the time take to initialize engine using Profiling macros
        auto stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_double = (stop - start);
        RAZIX_CORE_INFO("Engine Ingnited in : {0} ms", ms_double.count());
    }

    void RZEngine::ShutDown()
    {
        RAZIX_CORE_ERROR("***********************************");
        RAZIX_CORE_ERROR("*    Shutting down Engine....     *");
        RAZIX_CORE_ERROR("***********************************");

        // Shutting down all the sub-systems
        // Shutdown the VFS last
        RZVirtualFileSystem::ShutDown();

        // Log the completion of engine shutdown
        RAZIX_CORE_ERROR("***********************************");
        RAZIX_CORE_ERROR("*    Engine Shutdown Complete!    *");
        RAZIX_CORE_ERROR("***********************************");
    }

    void RZEngine::Run()
    {
        RAZIX_UNIMPLEMENTED_METHOD
    }
}