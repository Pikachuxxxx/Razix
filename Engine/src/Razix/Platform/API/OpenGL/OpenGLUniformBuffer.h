#pragma once

#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        class OpenGLUniformBuffer : public RZUniformBuffer
        {
        public:
            OpenGLUniformBuffer(uint32_t size, const void* data);
            ~OpenGLUniformBuffer();

            void Init(const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;
            void Bind() override;
            void SetData(uint32_t size, const void* data) override;
            void Destroy() override;

            inline uint32_t getHandle() { return m_UBO; }

        private:
            uint32_t m_UBO;
        };

    }    // namespace Graphics
}    // namespace Razix