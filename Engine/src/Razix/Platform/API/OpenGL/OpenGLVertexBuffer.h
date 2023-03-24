#pragma once

#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

namespace Razix {
    namespace Graphics {
        class OpenGLVertexBuffer : public RZVertexBuffer
        {
        public:
            OpenGLVertexBuffer(u32 size, const void* data, BufferUsage usage);
            ~OpenGLVertexBuffer();

            void Bind(RZCommandBuffer* cmdBuffer) override;
            void Unbind() override;
            void Destroy() override {}
            void SetData(u32 size, const void* data) override;
            void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;
            void AddBufferLayout(RZVertexBufferLayout& layout) override;

            void Map(u32 size = 0, u32 offset = 0) override;

            void UnMap() override;

            void* GetMappedBuffer() override;

            void Flush() override;

        private:
            u32 m_VAO;
            u32 m_VBO;
            void*    m_Mapped;
        };
    }    // namespace Graphics
}    // namespace Razix