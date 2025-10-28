#pragma once

namespace Razix {
    namespace Utilities {

        // https://www.shadertoy.com/view/sdG3zz
        RAZIX_API float3 GenerateHashedColor(u32 p);
        RAZIX_API float4 GenerateHashedColor4(u32 p);

        RAZIX_API uint32_t ColorToARGB(const float4& color);

    }    // namespace Utilities
}    // namespace Razix