#pragma once

#include "Razix/Graphics/API/RZIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Graphics {
        class VKIndexBuffer : public RZIndexBuffer, public VKBuffer
        {
        public:
            VKIndexBuffer(uint32_t* data, uint32_t count, BufferUsage bufferUsage);
            ~VKIndexBuffer();

            void Bind(RZCommandBuffer* commandBuffer = nullptr) override;
            void Unbind() override;
        private:
            bool m_IsBufferMapped = false;

        };
    }
}
