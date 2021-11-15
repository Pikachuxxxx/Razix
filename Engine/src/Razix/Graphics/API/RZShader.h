#pragma once

namespace Razix {
    namespace Graphics {

        /* The stage which the shader corresponds to in the graphics pipeline */
        enum class ShaderStage
        {
            NONE, VERTEX, PIXEL, GEOMETRY, TCS, TES, COMPUTE
        };

        /* Razix Shader that will be passed to the GPU at various stages */
        class RZShader
        {
        public:
            RZShader() { }
            virtual ~RZShader() {};

            static RZShader* Create(const std::string& filePath);

            virtual void Bind() const = 0;
            virtual void Unbind() const = 0;
            virtual void ReflectShader() const = 0;

            void readShader();

            /* Gets the stage of the pipeline that shader is bound/being used with */
            inline const ShaderStage& getStage() { return m_ShaderStage; }

        private:
            ShaderStage m_ShaderStage = ShaderStage::NONE;
        };
    
    } 
}

