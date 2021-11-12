#pragma once

namespace Razix {
    namespace Graphics {

        /* The stage which the shader corresponds to in the graphics pipeline */
        enum class ShaderStage
        {
            NONE, VERTEX, PIXEL, GEOMETRY, TCS, TES, DOMAIN_STAGE, COMPUTE
        };

        /* Razix Shader that will be passed to the GPU at various stages */
        class RZShader
        {
        public:
            RZShader() { }
            ~RZShader() { }

            /* Gets the stage of the pipeline that shader is bound/being used with */
            inline const ShaderStage& getStage() { return m_ShaderStage; }

        private:
            ShaderStage m_ShaderStage = ShaderStage::NONE;

        };
    
    } 
}

