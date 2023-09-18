#pragma once

#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Core/RZSmartPointers.h"

namespace Razix {
    namespace Graphics {

        class RZShader;

        enum class ShaderBuiltin : u32
        {
            Default,
            Skybox,
            ProceduralSkybox,
            PBR,
            PBRIBL,
            PBRDeferredLighting,
            GBuffer,
            Composition,
            ShadowMapping,
            CSM,
            ImGui,
            EnvToCubemap,
            GenerateIrradianceMap,
            GeneratePreFilteredMap,
            DebugPoint,
            DebugLine,
            BUILTIN_SHADERS_COUNT
        };

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

            void loadBuiltInShader(ShaderBuiltin shaderID, std::string shaderPath);

            RZShaderHandle getBuiltInShader(ShaderBuiltin builtInShaderName);
            RZShaderHandle getBuiltInShader(std::string shaderName);
            ShaderBuiltin  getBuiltInShaderID(std::string shaderName);

        public:
            std::unordered_map<ShaderBuiltin, RZShaderHandle> m_BuiltinShaders;
            std::unordered_map<std::string, ShaderBuiltin>    m_BuiltinShadersReverseNameMap;
        };
    }    // namespace Graphics
}    // namespace Razix
