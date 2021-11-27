#pragma once

#include "Razix/Graphics/API/RZVertexBuffer.h"
#ifdef RAZIX_RENDER_API_VULKAN

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        class VKVertexBuffer : public RZVertexBuffer, public VKBuffer
        {
        public:
            VKVertexBuffer(uint32_t size, const void* data, const BufferUsage& usage);
            ~VKVertexBuffer();

            void Bind(RZCommandBuffer* cmdBuffer) override;
            void Unbind() override;

            void SetSubData(uint32_t size, const void* data, uint32_t offset) override;
        private:
            bool m_IsBufferMapped = false;
        };
    }
}
#endif