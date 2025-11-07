#pragma once

namespace Razix {
    namespace Gfx {

        // https://0fps.net/2013/07/09/texture-atlases-wrapping-and-mip-mapping/
        //
        // FIXME: resolve visible dots artifact, enable mips for source env cubemap and resolve darkening issue in pre-filtered cubemap specular mapping
        // TODO: Remove it as a class and add only when managing local cubemaps...keep it more simple

        /**
         * Handles Image Based Lighting and Utility functions 
         * Helps generate pre-filtered and irradiance maps for skybox hdr image and caches them
         */
        class RZImageBasedLightingProbesManager
        {
        public:
            RZImageBasedLightingProbesManager()  = default;
            ~RZImageBasedLightingProbesManager() = default;

            //static rz_texture_handle convertEquirectangularToCubemap(const RZString& hdrFilePath);
            //static rz_texture_handle generateIrradianceMap(rz_texture_handle cubeMap);
            //static rz_texture_handle generatePreFilteredMap(rz_texture_handle cubeMap);
        };
    }    // namespace Gfx
}    // namespace Razix
