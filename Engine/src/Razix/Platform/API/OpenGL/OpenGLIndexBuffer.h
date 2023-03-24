#pragma once

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"

namespace Razix {
    namespace Graphics {
        class OpenGLIndexBuffer : public RZIndexBuffer
        {
        public:
            OpenGLIndexBuffer(u16* data, u32 count, BufferUsage bufferUsage);
            ~OpenGLIndexBuffer();

            void Bind(RZCommandBuffer* commandBuffer = nullptr) override;
            void Unbind() override;
            void Destroy() override {}
            void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;

            void Map(u32 size = 0, u32 offset = 0) override;

            void UnMap() override;

            void* GetMappedBuffer() override;

            void Flush() override;

        private:
            u32 m_IBO;
            void*    m_Mapped;
        };
    }    // namespace Graphics
}    // namespace Razix
