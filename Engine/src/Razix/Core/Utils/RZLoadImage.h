#pragma once

#include "Razix/Core/RZCore.h"

namespace Razix {
    RAZIX_API u8*  LoadImageData(const RZString& filePath, u32* width, u32* height, u32* bpp, bool flipY = true);
    RAZIX_API f32* LoadImageDataFloat(const RZString& filePath, u32* width, u32* height, u32* bpp, bool flipY = true);
}    // namespace Razix
