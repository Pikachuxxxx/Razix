#pragma once

#include "RazixVersion.h"
// Using the forward declared the application creating function, that we assume was defined on the client side
extern Razix::Application* Razix::CreateApplication();

/********************************************************************************
 *                              Engine Entry Point                              *
 *******************************************************************************/ 
#ifdef RAZIX_PLATFORM_WINDOWS

#include "Platform/Windows/WindowsOS.h"

// TODO: Change this back to WinMain, since we are use the logging system to output to console we use an Console App instead of an Widowed App
//#pragma comment(linker, "/subsystem:windows")
//
//#ifndef NOMINMAX
//#define NOMINMAX // For windows.h
//#endif
//
//#include <windows.h>

// Windows Entry point - WinMain
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
int main(int argc, char** argv)
{
    // 1.-> Logging System Initialization
    Razix::Debug::Log::InitLogger();
    // Logging the Version details
    RAZIX_CORE_INFO("Version : {0}", Razix::RazixVersion.GetVersionString());
    RAZIX_CORE_INFO("Release Stage : {0}", Razix::RazixVersion.GetReleaseStage());

    // Create the OS Instance
    auto windowsOS = new Razix::WindowsOS();
    Razix::OS::SetInstance(windowsOS);
    windowsOS->Init();

    // Application auto Initialization by the Engine
    Razix::CreateApplication();

    // Run the  Application with the master controlled given to the OS
    windowsOS->Run();
    delete windowsOS;

    // Shutdown the Engine systems
    Razix::Debug::Log::ShutdownLogger();

    return EXIT_SUCCESS;
}
#endif