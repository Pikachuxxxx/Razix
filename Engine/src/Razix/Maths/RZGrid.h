#pragma once

#include "Razix/Maths/AABB.h"

#include "glm/common.hpp"
#include "glm/vec3.hpp"

namespace Razix {
    namespace Maths {

        inline constexpr f32 max3(const glm::vec3& v)
        {
            return glm::max(glm::max(v.x, v.y), v.z);
        }

        struct RZGrid
        {
            RZGrid() = default;
            /**
             * The cell size for the grid is defined by the GI and Tiling implementation so it's defined there
             */
            explicit RZGrid(const AABB& aabb);

            AABB     aabb;     /* aabb of the Grid                     */
            glm::uvec3 size;     /* Total dimensions of the grid         */
            f32      cellSize; /* The size of each cell in the grid    */
        };
    }    // namespace Maths
}    // namespace Razix