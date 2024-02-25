// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPlatformUtils.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <Windows.h>
#elif RAZIX_PLATFORM_LINUX
    #include <pthread.h>
#else

#endif

namespace Razix {

    PlatformUtils::Endianess PlatformUtils::GetEndianess()
    {
        // Create a 2-byte integer with a known pattern
        u16 value = 0x01;

        // Use a pointer to examine the first byte
        u8* bytePtr = reinterpret_cast<u8*>(&value);

        // If the first byte is 0x01, it means the machine is little-endian
        if (*bytePtr == 0x01)
            return PlatformUtils::Endianess::LITTLE;
        else
            return PlatformUtils::Endianess::BIG;
    }

    void PlatformUtils::SetThreadName(std::thread& thread, const std::string& name)
    {
#ifdef RAZIX_PLATFORM_WINDOWS
        const std::wstring threadNameWStr = std::wstring(name.begin(), name.end());
        SetThreadDescription(thread.native_handle(), threadNameWStr.c_str());
#elif RAZIX_PLATFORM_LINUX
#endif
    }
}    // namespace Razix
