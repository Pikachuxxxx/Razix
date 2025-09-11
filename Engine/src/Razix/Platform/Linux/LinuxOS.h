#pragma once

#include "Razix/Core/OS/RZOS.h"

namespace Razix {
    /* Windows specific OS Instance  */
    class RAZIX_API LinuxOS : public RZOS
    {
    public:
        LinuxOS()  = default;
        ~LinuxOS() = default;

        /* Initializes the OS resources required for the engine */
        void Init() override;

        /** <summary>
		 * Initializes the Application and controls it's lifecycle as well as the Engine Runtime systems
		 * @brief This way the OS can still have the master control over the engine's Lifecycle, VFS
		 * By doing this the OS is lending it's resources to the engine while still keeping in control of 
		 * the application to preserve it's master status over the application that it permits to run
		 */
        void Begin() override;
    };
}    // namespace Razix
