// clang-format off
#include "rzxpch.h"
// clang-format on
#include "WindowsOS.h"

#include "Razix/Core/RZApplication.h"

#include "Razix/Core/OS/RZWindow.h"
#ifdef RAZIX_USE_GLFW_WINDOWS
    #include "Razix/Platform/GLFW/GLFWInput.h"
    #include "Razix/Platform/GLFW/GLFWWindow.h"
#else
    #include "Razix/Platform/Windows/WindowsInput.h"
    #include "Razix/Platform/Windows/WindowsWindow.h"
#endif

namespace Razix {
    void WindowsOS::Init()
    {
        RAZIX_CORE_INFO("Creating Windows OS instance");
        RAZIX_CORE_INFO("System OS : {0}", RAZIX_STRINGIZE(RAZIX_BUILD_CONFIG));
#ifdef RAZIX_USE_GLFW_WINDOWS
        // Set GLFW as window when the Engine API will be called to create the window
        GLFWWindow::Construct();
        // Select GLFW as the input manager client
        //RZInput::SelectGLFWInputManager();
#else
        WindowsWindow::Construct();
        Razix::RZInput* Razix::RZInput::sInstance = new WindowsInput();
#endif
    }

    void WindowsOS::Run()
    {
        // TODO: Implement the Application life cycle here! in a more Robust way
        // Initialize the application with additional systems after application constructor is called with project name and VFS mounts
        //Razix::RZApplication::Get().Init();
        // Application auto Initialization by the Engine
        Razix::RZApplication::Get().Run();
    }

}    // namespace Razix