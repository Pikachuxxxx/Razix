#pragma once

#include "Razix/Core/Utils/TRZSingleton.h"

#include "Razix/Gfx/RZGfxUtil.h"

#include "Razix/Core/Containers/string.h"

namespace Razix {
    namespace Gfx {

        class RAZIX_API RZShaderLibrary : public RZSingleton<RZShaderLibrary>
        {
        public:
            /* Initializes the Shader Library */
            void StartUp();
            /* Shuts down the Shader Library releases all the shaders held by it */
            void ShutDown();

            void reloadShadersFromDisk();
            void loadBuiltInShader(ShaderBuiltin shaderID, RZString shaderPath);

            rz_gfx_shader_handle getBuiltInShader(ShaderBuiltin builtInShaderName);
            rz_gfx_shader_handle getBuiltInShader(RZString shaderName);
            ShaderBuiltin        getBuiltInShaderID(RZString shaderName);

        public:
            RZHashMap<ShaderBuiltin, rz_gfx_shader_handle> m_BuiltinShaders;
            RZHashMap<RZString, ShaderBuiltin>             m_BuiltinShadersReverseNameMap;
            //RZHashMap<ShaderBuiltin, RZShaderHandle> m_UserShaders;
            //RZHashMap<RZString, ShaderBuiltin>    m_UserShadersReverseNameMap;
        };
    }    // namespace Gfx
}    // namespace Razix
