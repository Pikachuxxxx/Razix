#pragma once

#include "Razix/Core/RZCore.h"

namespace Razix {
    namespace Utilities {
        RAZIX_API u8* LoadImageData(const std::string& filePath, u32* width, u32* height, u32* bpp);
    }
}    // namespace Razix
