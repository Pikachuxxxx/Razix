#pragma once

namespace Razix::CrashDumpHandler {

    extern void Initialize();
    extern void WriteCrashDump(int signal, const std::string& description = "");
}    // namespace Razix::CrashDumpHandler

#define CRASH_DUMP() WriteCrashDump
