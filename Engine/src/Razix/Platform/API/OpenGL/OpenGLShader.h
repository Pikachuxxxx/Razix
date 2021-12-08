#pragma once

#include "Razix/Graphics/API/RZShader.h"

namespace Razix {
    namespace Graphics {

        class OpenGLShader : public RZShader
        {
        public:
            OpenGLShader(const std::string& filePath);
            ~OpenGLShader();

            void Bind() const override;
            void Unbind() const override;

        private:
            uint32_t m_ProgramID;

        private:
            void Init();
        };
    }
}

