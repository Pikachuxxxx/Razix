#pragma once

#include "Razix/Core/OS/RZInput.h"
#include <GLFW/glfw3.h>

namespace Razix {
    /**
     * GLFW-based input implementation for cross-platform support
     * Provides input handling for Windows, macOS, Linux using GLFW
     */
    class RAZIX_API GLFWInput
    {
    public:
        // Helper methods for GLFW-specific functionality
        static GLFWwindow* GetActiveWindow();
        static GLFWgamepadstate GetGamepadState();
    };

}    // namespace Razix