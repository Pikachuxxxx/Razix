#pragma once

namespace Razix {
    namespace Gfx {

        class RZCubeMap;

        // https://0fps.net/2013/07/09/texture-atlases-wrapping-and-mip-mapping/
        // TODO : FIXME: resolve visible dots artifact, enable mips for source env cubemap and resolve darkening issue in pre-filtered cubemap specular mapping

        /**
         * Handles Image Based Lighting and Utility functions 
         * Helps generate pre-filtered and irradiance maps for skybox hdr image and caches them
         */
        class RZImageBasedLightingProbesManager
        {
        public:
            RZImageBasedLightingProbesManager()  = default;
            ~RZImageBasedLightingProbesManager() = default;

            // TODO: set CubeMap width and height

            static RZTextureHandle convertEquirectangularToCubemap(const std::string& hdrFilePath);
            static RZTextureHandle generateIrradianceMap(RZTextureHandle cubeMap);
            static RZTextureHandle generatePreFilteredMap(RZTextureHandle cubeMap);
        };
    }    // namespace Gfx
}    // namespace Razix
