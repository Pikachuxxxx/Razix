#pragma once

#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZVertexBufferLayout.h"

#include <SPIRVCross/spirv_glsl.hpp>

namespace Razix {
    namespace Graphics {

        class OpenGLShader : public RZShader
        {
        public:
            OpenGLShader(const std::string& filePath);
            ~OpenGLShader();

            void Bind() const override;
            void Unbind() const override;
            void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) override;

            void pushTypeToBuffer(const spirv_cross::SPIRType type, Graphics::RZVertexBufferLayout& layout, const std::string& name);

        private:
            uint32_t                m_ProgramID;    /* OpenGL ID for the shader program that will bind to the pipeline  */
            RZVertexBufferLayout    m_Layout;       /* The layout of the vertex buffer and it's attributes              */
            std::vector<DescriptorBindingLayout> m_BindingLayouts;  /* The uniform and sampler binding layout information */

        private:
            void Init();
        };
    }
}

