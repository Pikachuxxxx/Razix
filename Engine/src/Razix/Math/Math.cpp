// clang-format off
#include "rzxpch.h"
// clang-format on

namespace Razix {
    namespace Math {

        static float lerp_internal(float from, float to, float t)
        {
            return from + t * (to - from);
        }

        float lerp(float a, float b, float t, float dt)
        {
            return lerp_internal(a, b, 1.f - powf(1 - t, dt));
        }

        float3 lerp3(const float3& from, const float3& to, f32 t, f32 dt)
        {
            return float3{lerp(from.x, to.x, t, dt), lerp(from.y, to.y, t, dt), lerp(from.z, to.z, t, dt)};
        }

        // https://graphicscompendium.com/opengl/22-interpolation
        float3 lerp(float3 x, float3 y, float t)
        {
            return x * (1.f - t) + y * t;
        }

    }    // namespace Math
}    // namespace Razix