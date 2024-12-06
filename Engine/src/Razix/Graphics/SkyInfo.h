#pragma once

namespace Razix {
    namespace Graphics {

        struct SkyInfo
        {
            RZMaterialHandle skyboxMaterial;
            u32              azimuthal;
            u32              elevation;
            // TODO: Add weather, rain etc. presets
        };
    }    // namespace Graphics
}    // namespace Razix