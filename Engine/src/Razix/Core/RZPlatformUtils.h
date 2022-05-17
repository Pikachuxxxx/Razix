#pragma once

namespace Razix {

    enum class Endianess
    {
        LITTLE,
        BIG
    };

    /**
     * Platform Utility methods to get information about the current platform and it's environment configuration
     */
    class RAZIX_API PlatformUtils
    {
    public:
        PlatformUtils()  = default;
        ~PlatformUtils() = default;

        static Endianess GetEndianess();
    };
}    // namespace Razix