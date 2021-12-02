#pragma once

namespace Razix {
    namespace Graphics {

        /* The stage which the shader corresponds to in the graphics pipeline */
        enum class ShaderStage
        {
            NONE, VERTEX, PIXEL, GEOMETRY, TCS, TES, COMPUTE
        };

        enum class ShaderSourceType
        {
            RAZIX, GLSL, SPIRV, HLSL, PSSL, CG
        };

        /* 
         * Razix Shader that will be passed to the GPU at various stages 
         */
        class RZShader
        {
        public:
            RZShader() { }
            virtual ~RZShader() {};

            static RZShader* Create(const std::string& filePath);

            virtual void Bind() const = 0;
            virtual void Unbind() const = 0;

            void readShader();
            static void ReflectShader(const ShaderSourceType& sourceType);

            /* Gets the stage of the pipeline that shader is bound/being used with */
            inline const ShaderStage& getStage() { return m_ShaderStage; }

        private:
            ShaderStage m_ShaderStage = ShaderStage::NONE;

        private:
            static void ReflectGLSLShader();
            static void ReflectSPIRVShader();
            static void ReflectHLSLShader();
            static void ReflectPSSLShader();
            static void ReflectCgShader();
        };
    
    } 
}

