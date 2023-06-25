// clang-format off
#include "rzxpch.h"
// clang-format on
#include "AABB.h"

namespace Razix {
    namespace Maths {

        glm::vec3 AABB::getExtent() const
        {
            return max - min;
        }

        glm::vec3 AABB::getCenter() const
        {
            return (max + min) * 0.5f;
        }

        f32 AABB::getRadius() const
        {
            return glm::length(getExtent() * 0.5f);
        }

        AABB AABB::transform(const glm::mat4& m) const
        {
            // https://dev.theomader.com/transform-bounding-boxes/

            const auto xa = m[0] * min.x;
            const auto xb = m[0] * max.x;

            const auto ya = m[1] * min.y;
            const auto yb = m[1] * max.y;

            const auto za = m[2] * min.z;
            const auto zb = m[2] * max.z;

            return {
                {glm::min(xa, xb) + glm::min(ya, yb) + glm::min(za, zb) + m[3]},
                {glm::max(xa, xb) + glm::max(ya, yb) + glm::max(za, zb) + m[3]},
            };
        }
    }    // namespace Maths
}    // namespace Razix