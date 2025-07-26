// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZInput.h"

// Platform-specific input implementations
#include "Razix/Platform/GLFW/GLFWInput.h"
#ifdef RAZIX_PLATFORM_WINDOWS
    #include "Razix/Platform/Windows/WindowsInput.h"
#endif

namespace Razix {
    // Initialize the singleton instance
    RZInput* RZInput::s_Instance = nullptr;

    void RZInput::SelectGLFWInputManager()
    {
        if (s_Instance) {
            delete s_Instance;
        }
        s_Instance = new GLFWInput();
    }

#ifdef RAZIX_PLATFORM_WINDOWS
    void RZInput::SelectWindowsInputManager()
    {
        if (s_Instance) {
            delete s_Instance;
        }
        s_Instance = new WindowsInput();
    }
#endif

}
