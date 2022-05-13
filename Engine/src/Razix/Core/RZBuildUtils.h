#pragma once

#include "Razix/Core/RZCore.h"

namespace Razix {

    /**
     * Build Utilities provide information about the build settings for Razix engine that aid in optimization and settings based on build configs
     * Compiler settings, debug settings etc. can be customized and play a crucial role in design and optimizations thought out the code base
     */
    namespace BuildUtils {

        enum class BuildConfig
        {
            DEBUG,
            RELEASE,
            DISTRIBUTION
        };

        /* The platform in which the engine is being built for */
        enum class Platform
        {
            WINDOWS,
            MACOSX,
            LINUX,
            FREE_BSD,
            PROSPERO,
            ORBIS,
            DURANGO,
            PS3,
            PSVITA,
            PSP,
            PS2,
            PS1
        };

        /* Gets the build configuration in which the engine was build */
        BuildConfig getBuildConfig()
        {
#ifdef RAZIX_DEBUG
            return BuildConfig::DEBUG;
#elif RAZIX_RELEASE
            return BuildConfig::RELEASE;
#elif RAZIX_DISTRIBUTION
            return BuildConfig::DISTRIBUTION;
#endif
        }

        Platform getPlatform()
        {
#ifdef RAZIX_PLATFORM_WINDOWS
            return Platform::WINDOWS;
#endif
        }
    }    // namespace BuildUtils

}    // namespace Razix