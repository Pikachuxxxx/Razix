#pragma once

#include "Razix/Graphics/RHI/API/RZStorageBuffer.h"

namespace Razix {
    namespace Graphics {

        class OpenGLStorageBuffer : public RZStorageBuffer
        {
        public:
            OpenGLStorageBuffer(u32 size, const std::string& name);
            ~OpenGLStorageBuffer() {}

            void  Bind() override;
            void* GetData() override;
            void  SetData(u32 size, const void* data) override;
            void  Destroy() override;

        private:
            u32 m_SSBO;
            void*    m_Mapped = nullptr;
            u32 m_Size;
        };
    }    // namespace Graphics
}    // namespace Razix
