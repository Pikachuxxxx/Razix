#pragma once

namespace Razix {
    namespace Utilities {

        // https://www.shadertoy.com/view/sdG3zz
        RAZIX_API glm::vec3 GenerateHashedColor(u32 p);
        RAZIX_API glm::vec4 GenerateHashedColor4(u32 p);

        RAZIX_API uint32_t ColorToARGB(const glm::vec4& color);

    }    // namespace Utilities
}    // namespace Razix