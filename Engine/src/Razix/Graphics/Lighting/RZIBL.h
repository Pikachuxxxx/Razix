#pragma once

namespace Razix {
    namespace Graphics {

        class RZCubeMap;

        /**
         * Handles Image Based Lighting and Utility functions 
         * Helps generate pre-filtered and irradiance maps for skybox hdr image and caches them
         */
        class RZIBL
        {
        public:
            RZIBL()  = default;
            ~RZIBL() = default;

            // TODO: set CubeMap width and height

            static RZCubeMap* convertEquirectangularToCubemap(const std::string& hdrFilePath);
            static RZCubeMap* generateIrradianceMap(RZCubeMap* cubeMap);
            static RZCubeMap* generatePreFilteredMap(RZCubeMap* cubeMap);
        };
    }    // namespace Graphics
}    // namespace Razix
