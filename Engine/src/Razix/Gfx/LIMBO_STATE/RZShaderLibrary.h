#pragma once

#include "Razix/Core/Utils/TRZSingleton.h"

#include "Razix/Core/RZSTL/smart_pointers.h"

#include "Razix/Gfx/GfxData.h"

namespace Razix {
    namespace Gfx {

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
