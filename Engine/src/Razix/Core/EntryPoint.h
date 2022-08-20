#pragma once

#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZProfiling.h"

/* Using the forward declared the application creating function, that we assume was defined on the client side */
extern Razix::RZApplication* Razix::CreateApplication(int argc, char** argv);

/********************************************************************************
 *                        Razix Engine Entry Point                              *
 *******************************************************************************/
#ifdef RAZIX_PLATFORM_WINDOWS

    #include "Platform/Windows/WindowsOS.h"
    #include "Razix/Core/RZSplashScreen.h"

// TODO: Change this back to WinMain, since we are use the logging system to output to console we use an Console App instead of an Widowed App
//#pragma comment(linker, "/subsystem:windows")
//
//#ifndef NOMINMAX
//#define NOMINMAX // For windows.h
//#endif
//
//#include <windows.h>

/* Windows Entry point - WinMain */
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
int EngineMain(int argc, char** argv)
{
    // Read the command line arguments
    static std::vector<const char*> args;
    for (int32_t i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    };

    // Splash Screen!
    Razix::RZSplashScreen::Get().init();
    Razix::RZSplashScreen::Get().setVersionString("Version : " + std::string(Razix::RazixVersion.getVersionString()));
    Razix::RZSplashScreen::Get().setLogString("Initializing Razix Engine...");

    // 1.-> Logging System Initialization
    Razix::Debug::RZLog::StartUp();

    // Create the OS Instance
    auto windowsOS = new Razix::WindowsOS();
    Razix::RZOS::SetInstance(windowsOS);

    //-------------------------------//
    //        Engine Ignition        //
    //-------------------------------//
    Razix::RZEngine::Get().Ignite();
    //-------------------------------//

    // Parse the command line arguments, if any
    if (argc > 1)
        Razix::RZEngine::Get().commandLineParser.parse(args);

    Razix::RZSplashScreen::Get().setLogString("Loading Project file...");

    windowsOS->Init();

    // Application auto Initialization by the Engine
    Razix::CreateApplication(argc, argv);

    Razix::RZEngine::Get().PostGraphicsIgnite();

    // Run the  Application with the master controlled given to the OS
    windowsOS->Run();
    delete windowsOS;

    // Shutdown the Engine
    Razix::RZEngine::Get().ShutDown();

    // Shutdown the Engine systems
    Razix::Debug::RZLog::Shutdown();

    return EXIT_SUCCESS;
}

#endif