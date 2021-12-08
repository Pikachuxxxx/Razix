#pragma once

#include "Razix/Graphics/API/RZVertexBuffer.h"

namespace Razix {
    namespace Graphics {
        class OpenGLVertexBuffer : public RZVertexBuffer
        {
        public:
            OpenGLVertexBuffer(uint32_t size, const void* data, BufferUsage usage);
            ~OpenGLVertexBuffer();

            void Bind(RZCommandBuffer* cmdBuffer) override;
            void Unbind() override;
            void SetSubData(uint32_t size, const void* data, uint32_t offset) override;
            void AddBufferLayout(RZVertexBufferLayout& layout) override;
        private:
            uint32_t m_VAO;
            uint32_t m_VBO;
        };
    }
}