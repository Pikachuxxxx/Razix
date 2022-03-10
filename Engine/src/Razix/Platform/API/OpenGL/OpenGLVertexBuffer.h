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
            void Destroy() override {}
            void SetData(uint32_t size, const void* data) override;
            void AddBufferLayout(RZVertexBufferLayout& layout) override;
        private:
            uint32_t m_VAO;
            uint32_t m_VBO;
        };
    }
}