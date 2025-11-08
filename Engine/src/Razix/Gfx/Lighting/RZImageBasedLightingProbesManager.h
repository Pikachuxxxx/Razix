#pragma once

#include "Razix/Gfx/RHI/RHI.h"

#define CUBEMAP_LAYERS                6
#define CUBEMAP_DIM                   1024
#define IRRADIANCE_MAP_DIM            64
#define PREFILTERED_MAP_DIM           128
#define IBL_DISPATCH_THREAD_GROUP_DIM 32
#define HDR_IMAGE_CHANNELS            4

namespace Razix {
    namespace Gfx {

        // https://0fps.net/2013/07/09/texture-atlases-wrapping-and-mip-mapping/
        // FIXME: resolve visible dots artifact, enable mips for source env cubemap and resolve darkening issue in pre-filtered cubemap specular mapping

        /**
         * Handles Image Based Lighting and Utility functions 
         * Helps generate pre-filtered and irradiance maps for skybox hdr image and caches them
         */

        rz_gfx_texture_handle ConvertEquirectangularToCubemap(const RZString& hdrFilePath);
        rz_gfx_texture_handle GenerateIrradianceMap(rz_gfx_texture_handle cubeMap);
        rz_gfx_texture_handle GeneratePreFilteredMap(rz_gfx_texture_handle cubeMap);

    }    // namespace Gfx
}    // namespace Razix
