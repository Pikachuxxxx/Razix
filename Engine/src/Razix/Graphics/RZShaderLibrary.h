#pragma once

#include "Razix/Graphics/API/RZShader.h"

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    namespace Graphics {

        /**
         * Manages the shaders instances and loads them nicely and gives their references all over the engine
         */
        class RZShaderLibrary : public RZSingleton<RZShaderLibrary>
        {
        public:
            RZShaderLibrary() {}
            ~RZShaderLibrary() {}

            /* Initializes the Shader Library */
            void StartUp();
            /* Shuts down the Shader Library releases all the shaders held by it */
            void ShutDown();

            void loadShader(std::string shaderPath);
            RZShader* getShader(std::string shaderName);

        public:
            // TODO: well... to be more robust use the UUID to find the RZShader as a RZAsset, this is a discussion for another day
            // TODO: Use a packed dynamic memory pool similar to Sentinel Engine
            std::vector<RZShader*> m_Shaders;
        };
    }    // namespace Graphics
}    // namespace Razix
