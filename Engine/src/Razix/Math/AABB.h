#pragma once

namespace Razix {
    namespace Maths {

        struct AABB
        {
            float3 min, max;

            float3 getExtent() const;
            float3 getCenter() const;
            f32       getRadius() const;

            AABB transform(const float4x4& m) const;

            //auto operator<=>(const AABB&) const = default;
        };
    }    // namespace Maths
}    // namespace Razix
