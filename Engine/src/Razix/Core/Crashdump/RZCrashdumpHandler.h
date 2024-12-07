#pragma once

namespace Razix::CrashDumpHandler {

    void Initialize();
    void WriteCrashDump(int signal, const std::string& description = "");
}

#define CRASH_DUMP() WriteCrashDump
