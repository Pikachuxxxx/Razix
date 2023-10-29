// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZColorUtilities.h"

namespace Razix {
    namespace Utilities {

        // https://www.shadertoy.com/view/sdG3zz
        glm::vec3 GenerateHashedColor(u32 p)
        {
            p              = 1103515245U * ((p >> 1U) ^ (p));
            u32        h32 = 1103515245U * ((p) ^ (p >> 3U));
            u32        n   = h32 ^ (h32 >> 16);
            glm::uvec3 rz  = glm::uvec3(n, n * 16807U, n * 48271U);
            return glm::vec3(rz & glm::uvec3(0x7fffffffU)) / float(0x7fffffff);
        }

        glm::vec4 GenerateHashedColor4(u32 p)
        {
            p                = 1103515245U * ((p >> 1U) ^ (p));
            u32        h32   = 1103515245U * ((p) ^ (p >> 3U));
            u32        n     = h32 ^ (h32 >> 16);
            glm::uvec3 rz    = glm::uvec3(n, n * 16807U, n * 48271U);
            glm::vec3  color = glm::vec3(rz & glm::uvec3(0x7fffffffU)) / float(0x7fffffff);
            return glm::vec4(color, 1.0f);
        }
    }    // namespace Utilities
}    // namespace Razix