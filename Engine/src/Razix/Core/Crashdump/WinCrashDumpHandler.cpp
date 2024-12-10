// clang-format off
#include "rzxpch.h"
// clang-format on

#if defined(RAZIX_DEBUG) && defined(RAZIX_PLATFORM_WINDOWS)

namespace Razix::CrashDumpHandler {

    void Initialize()
    {
    }

    void WriteCrashDump(int signal, const std::string& description)
    {
    }

}    // namespace Razix::CrashDumpHandler

#endif
