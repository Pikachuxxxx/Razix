#pragma once

namespace Razix {

    /**
     * Platform Utility methods to get information about the current platform and it's environment configuration
     */
    namespace PlatformUtils {

        enum class Endianess
        {
            LITTLE,
            BIG
        };

        Endianess GetEndianess();
        void      SetThreadName(std::thread& thread, const std::string& name);

    }    // namespace PlatformUtils
}    // namespace Razix