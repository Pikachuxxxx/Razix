#pragma once

#include "Razix/Core/OS/RZInput.h"

namespace Razix {
    /**
     * Windows-specific input implementation using Win32 API
     * Provides native Windows input handling for better performance and control
     */
    class RAZIX_API WindowsInput
    {
    public:
        // Helper methods for Windows-specific functionality
        static HWND GetActiveWindow();
    };

}