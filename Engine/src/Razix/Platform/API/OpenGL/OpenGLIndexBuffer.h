#pragma once

#include "Razix/Graphics/API/RZIndexBuffer.h"

namespace Razix {
    namespace Graphics {
        class OpenGLIndexBuffer : public RZIndexBuffer
        {
        public:
            OpenGLIndexBuffer(uint16_t* data, uint32_t count, BufferUsage bufferUsage);
            ~OpenGLIndexBuffer();

            void Bind(RZCommandBuffer* commandBuffer = nullptr) override;
            void Unbind() override;
            void Destroy() override {}
            void Resize(uint32_t size, const void* data) override;

        private:
            uint32_t m_IBO;
        };
    }
}
