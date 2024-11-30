#pragma once

namespace Razix {
    namespace Gfx {

        struct SkyInfo
        {
            RZMaterialHandle skyboxMaterial;
            u32              azimuthal;
            u32              elevation;
            // TODO: Add weather, rain etc. presets
        };
    }    // namespace Gfx
}    // namespace Razix