// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZColorUtilities.h"

namespace Razix {
    namespace Utilities {

        // https://www.shadertoy.com/view/sdG3zz
        float3 GenerateHashedColor(u32 p)
        {
            p         = 1103515245U * ((p >> 1U) ^ (p));
            u32   h32 = 1103515245U * ((p) ^ (p >> 3U));
            u32   n   = h32 ^ (h32 >> 16);
            uint3 rz  = uint3(n, n * 16807U, n * 48271U);
            return float3(rz & uint3(0x7fffffffU)) / float(0x7fffffff);
        }

        float4 GenerateHashedColor4(u32 p)
        {
            p            = 1103515245U * ((p >> 1U) ^ (p));
            u32    h32   = 1103515245U * ((p) ^ (p >> 3U));
            u32    n     = h32 ^ (h32 >> 16);
            uint3  rz    = uint3(n, n * 16807U, n * 48271U);
            float3 color = float3(rz & uint3(0x7fffffffU)) / float(0x7fffffff);
            return float4(color, 1.0f);
        }

        RAZIX_API uint32_t ColorToARGB(const float4& color)
        {
            return (0xFF << 24) |
                   (static_cast<uint8_t>(clamp(color.r, 0.0f, 1.0f) * 255.0f) << 16) |
                   (static_cast<uint8_t>(clamp(color.g, 0.0f, 1.0f) * 255.0f) << 8) |
                   static_cast<uint8_t>(clamp(color.b, 0.0f, 1.0f) * 255.0f);
        }

    }    // namespace Utilities
}    // namespace Razix