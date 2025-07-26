#pragma once

// DEPRECATED: This class is no longer used. RZInput now uses static functions with platform-specific implementations.
// This file is kept for compatibility but should not be used in new code.

#include "Razix/Core/OS/RZInput.h"

namespace Razix {
    // DEPRECATED: GLFWInput class - RZInput now implements GLFW functionality directly
    class RAZIX_API GLFWInput
    {
    public:
        // DEPRECATED: Input manager selection is no longer needed
        static void SelectGLFWInputManager() {}

        // All functionality has been moved to RZInput static methods
    };

}    // namespace Razix
