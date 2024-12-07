#pragma once

#include "RZSTL/smart_pointers.h"
#include "Razix/Core/Profiling/RZProfiling.h"
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Core/Crashdump/RZCrashdumpHandler.h"

/* Using the forward declared the application creating function, that we assume was defined on the client side */
extern Razix::RZApplication* Razix::CreateApplication(int argc, char** argv);

/********************************************************************************
 *                        Razix Engine Entry Point                              *
 *******************************************************************************/
#ifdef RAZIX_PLATFORM_WINDOWS

    #include "Platform/Windows/WindowsOS.h"
    #include "Razix/Core/SplashScreen/RZSplashScreen.h"

// TODO: Change this back to WinMain, since we are use the logging system to output to console we use an Console App instead of an Widowed App
// https://www.gamedev.net/forums/topic/251536-how-to-run-console-window-main-from-winmain/
// https://stackoverflow.com/questions/191842/how-do-i-get-console-output-in-c-with-a-windows-program

static int AttachConsole(void)
{
    if (!AllocConsole()) {
        MessageBox(NULL, L"AllocConsole failed", L"Error", MB_OK | MB_ICONERROR);
        return -1;    // Exit the application or handle the error appropriately
    }

    FILE* pFile;
    if (freopen_s(&pFile, "CONOUT$", "w", stdout) != 0) {
        MessageBox(NULL, L"Failed to redirect stdout", L"Error", MB_OK | MB_ICONERROR);
        return -1;    // Exit the application or handle the error appropriately
    }

    if (freopen_s(&pFile, "CONIN$", "r", stdin) != 0) {
        MessageBox(NULL, L"Failed to redirect stdin", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    if (freopen_s(&pFile, "CONOUT$", "w", stderr) != 0) {
        MessageBox(NULL, L"Failed to redirect stderr", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    return 0;
}

    #ifndef NOMINMAX
        #define NOMINMAX    // For windows.h
    #endif
    //
    #include <windows.h>

    #define RAZIX_PLATFORM_MAIN                                                                           \
        int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) \
        {                                                                                                 \
            AttachConsole();                                                                              \
            ShowWindow(GetConsoleWindow(), SW_SHOW);                                                      \
            EngineMain(__argc, __argv);                                                                   \
            while (Razix::RZApplication::Get().RenderFrame()) {                                           \
            }                                                                                             \
                                                                                                          \
            Razix::RZApplication::Get().Quit();                                                           \
            Razix::RZApplication::Get().SaveApp();                                                        \
                                                                                                          \
            EngineExit();                                                                                 \
            FreeConsole();                                                                                \
            return EXIT_SUCCESS;                                                                          \
        }

/* Windows Entry point - WinMain */
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
static int EngineMain(int argc, char** argv)
{
    Razix::CrashDumpHandler::Initialize();
    
    // Read the command line arguments
    static std::vector<cstr> args;
    for (int32_t i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    };

    // 1.-> Logging System Initialization, start up logging before anything else
    Razix::Debug::RZLog::StartUp();

    // Virtual File System for mapping engine config files
    Razix::RZVirtualFileSystem::Get().StartUp();

    Razix::RZEngine::Get().LoadEngineConfigFile();

    // Splash Screen!
    Razix::RZSplashScreen::Get().StartUp();
    Razix::RZSplashScreen::Get().setVersionString("Version : " + std::string(Razix::RazixVersion.getVersionString()));
    Razix::RZSplashScreen::Get().setLogString("Initializing Razix Engine...");

    // Create the OS Instance
    Razix::rzstl::UniqueRef<Razix::WindowsOS> windowsOS = Razix::rzstl::CreateUniqueRef<Razix::WindowsOS>();
    Razix::RZOS::SetInstance(windowsOS.get());

    //-------------------------------//
    //        Engine Ignition        //
    //-------------------------------//
    Razix::RZEngine::Get().Ignite();
    //-------------------------------//

    // Parse the command line arguments, if any
    if (argc > 1)
        Razix::RZEngine::Get().getCommandLineParser().parse(args);

    Razix::RZSplashScreen::Get().setLogString("Loading Project file...");

    windowsOS->Init();

    // Application auto Initialization by the Engine
    Razix::CreateApplication(argc, argv);

    Razix::RZEngine::Get().PostGraphicsIgnite();

    // Run the  Application with the master controlled given to the OS
    windowsOS->Run();
    windowsOS.release();

    return EXIT_SUCCESS;
}

static void EngineExit()
{
    // Shutdown the Engine
    Razix::RZEngine::Get().ShutDown();

    // Shutdown the Engine systems
    Razix::Debug::RZLog::Shutdown();
}

#elif defined RAZIX_PLATFORM_MACOS

    #include "Platform/MacOS/MacOSOS.h"
    #include "Razix/Core/SplashScreen/RZSplashScreen.h"

    #define RAZIX_PLATFORM_MAIN                                 \
        int main(int argc, char** argv)                         \
        {                                                       \
            EngineMain(argc, argv);                             \
            while (Razix::RZApplication::Get().RenderFrame()) { \
            }                                                   \
                                                                \
            Razix::RZApplication::Get().Quit();                 \
            Razix::RZApplication::Get().SaveApp();              \
                                                                \
            EngineExit();                                       \
                                                                \
            return EXIT_SUCCESS;                                \
        }

/* Windows Entry point - WinMain */
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
static int EngineMain(int argc, char** argv)
{
    Razix::CrashDumpHandler::Initialize();
    
    // Read the command line arguments
    static std::vector<cstr> args;
    for (int32_t i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    };

    // 1.-> Logging System Initialization, start up logging before anything else
    Razix::Debug::RZLog::StartUp();

    // Virtual File System for mapping engine config files
    Razix::RZVirtualFileSystem::Get().StartUp();

    Razix::RZEngine::Get().LoadEngineConfigFile();

    // Splash Screen!
    Razix::RZSplashScreen::Get().StartUp();
    Razix::RZSplashScreen::Get().setVersionString("Version : " + std::string(Razix::RazixVersion.getVersionString()));
    Razix::RZSplashScreen::Get().setLogString("Initializing Razix Engine...");

    // Create the OS Instance
    Razix::rzstl::UniqueRef<Razix::MacOSOS> macosOS = Razix::rzstl::CreateUniqueRef<Razix::MacOSOS>();
    Razix::RZOS::SetInstance(macosOS.get());

    //-------------------------------//
    //        Engine Ignition        //
    //-------------------------------//
    Razix::RZEngine::Get().Ignite();
    //-------------------------------//

    // Parse the command line arguments, if any
    if (argc > 1)
        Razix::RZEngine::Get().getCommandLineParser().parse(args);

    Razix::RZSplashScreen::Get().setLogString("Loading Project file...");

    macosOS->Init();

    // Application auto Initialization by the Engine
    Razix::CreateApplication(argc, argv);

    Razix::RZEngine::Get().PostGraphicsIgnite();

    // Run the  Application with the master controlled given to the OS
    macosOS->Run();
    macosOS.release();

    return EXIT_SUCCESS;
}

static void EngineExit()
{
    // Shutdown the Engine
    Razix::RZEngine::Get().ShutDown();

    // Shutdown the Engine systems
    Razix::Debug::RZLog::Shutdown();
}

#endif
