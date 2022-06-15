// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPlatformUtils.h"

namespace Razix {

    Endianess PlatformUtils::GetEndianess()
    {
        return Endianess::LITTLE;
    }
}    // namespace Razix
