#pragma once

#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        class OpenGLUniformBuffer : public RZUniformBuffer
        {
        public:
            OpenGLUniformBuffer(const RZBufferDesc& desc);
            ~OpenGLUniformBuffer() {}

            void Bind() override;
            void SetData(u32 size, const void* data) override;

            void Flush() override {}
            void Invalidate() override {}

            void DestroyResource() override;

            inline u32 getHandle() { return m_UBO; }

        private:
            u32 m_UBO;
        };

    }    // namespace Graphics
}    // namespace Razix