#pragma once

#include "Razix/Graphics/API/RZVertexBuffer.h"

namespace Razix {
    namespace Graphics {
        class OpenGLVertexBuffer : public RZVertexBuffer
        {
        public:
            OpenGLVertexBuffer(BufferUsage usage);
            ~OpenGLVertexBuffer();

            void Bind(const RZCommandBuffer* cmdBuffer) override;
            void Unbind() override;
            void SetData(uint32_t size, const void* data) override;
            void SetSubData(uint32_t size, const void* data, uint32_t offset) override;
        private:
            uint32_t m_VBO;

        };

    }
}