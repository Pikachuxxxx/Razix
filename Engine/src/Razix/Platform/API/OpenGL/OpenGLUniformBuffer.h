#pragma once

#include "Razix/Graphics/API/RZUniformBuffer.h"


namespace Razix {
    namespace Graphics {

        class OpenGLUniformBuffer : public RZUniformBuffer
        {
        public:
            OpenGLUniformBuffer(uint32_t size, const void* data, const std::string& name);
            ~OpenGLUniformBuffer();

            void Init(const void* data, const std::string& name) override;
            void SetData(uint32_t size, const void* data) override;
            void Destroy() override;

        private:
            uint32_t m_UBO;
        };

    }
}