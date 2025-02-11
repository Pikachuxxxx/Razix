#pragma once

#include "Razix/Utilities/TRZSingleton.h"

#include "RZSTL/smart_pointers.h"

#include "Razix/Gfx/RHI/API/RZAPIHandles.h"

namespace Razix {
    namespace Gfx {

        class RZShader;

        enum class ShaderBuiltin : u32
        {
            //------------------------------
            Default,
            Skybox,
            ProceduralSkybox,
            PBRLighting,
            PBRIBL,
            PBRDeferredLighting,
            GBuffer,
            //----------------
            VisibilityBufferFill,
            //----------------
            DeferredDecals,
            Composition,
            DepthPreTest,
            CSM,
            EnvToCubemap,
            GenerateIrradianceMap,
            GeneratePreFilteredMap,
            Sprite,
            SpriteTextured,
            DebugPoint,
            DebugLine,
            ImGui,
            // Post Processing FX
            SSAO,
            GaussianBlur,
            ColorGrading,
            TAAResolve,
            FXAA,
            Tonemap,
            //---
            COUNT
        };

#define REGISTER_USER_SHADER_ENUM_MEMBER(EnumType, memberName) \
    ShaderUserRegistered memberName;

#define REGISTER_USER_SHADER(EnumType, ShaderPath)

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

            void reloadShadersFromDisk();
            void loadBuiltInShader(ShaderBuiltin shaderID, std::string shaderPath);

            RZShaderHandle getBuiltInShader(ShaderBuiltin builtInShaderName);
            RZShaderHandle getBuiltInShader(std::string shaderName);
            ShaderBuiltin  getBuiltInShaderID(std::string shaderName);

        public:
            std::unordered_map<ShaderBuiltin, RZShaderHandle> m_BuiltinShaders;
            std::unordered_map<std::string, ShaderBuiltin>    m_BuiltinShadersReverseNameMap;
            //std::unordered_map<ShaderBuiltin, RZShaderHandle> m_UserShaders;
            //std::unordered_map<std::string, ShaderBuiltin>    m_UserShadersReverseNameMap;
        };
    }    // namespace Gfx
}    // namespace Razix
