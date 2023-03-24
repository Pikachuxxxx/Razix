#pragma once

namespace Razix {
    namespace Maths {

        struct RZAABB
        {
            glm::vec3 min, max;

            glm::vec3 getExtent() const;
            glm::vec3 getCenter() const;
            f32     getRadius() const;

            RZAABB transform(const glm::mat4& m) const;

            //auto operator<=>(const AABB&) const = default;
        };
    }    // namespace Maths
}    // namespace Razix
