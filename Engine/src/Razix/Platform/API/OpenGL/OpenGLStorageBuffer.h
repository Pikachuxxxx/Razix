#pragma once

#include "Razix/Graphics/RHI/API/RZStorageBuffer.h"

namespace Razix {
    namespace Graphics {

        class OpenGLStorageBuffer : public RZStorageBuffer
        {
        public:
            OpenGLStorageBuffer(uint32_t size, const std::string& name);
            ~OpenGLStorageBuffer() {}

            void  Bind() override;
            void* GetData() override;
            void  SetData(uint32_t size, const void* data) override;
            void  Destroy() override;

        private:
            uint32_t m_SSBO;
            void*    m_Mapped = nullptr;
            uint32_t m_Size;
        };
    }    // namespace Graphics
}    // namespace Razix
