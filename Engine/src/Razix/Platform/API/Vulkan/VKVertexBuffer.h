#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

#include "Razix/Graphics/API/RZVertexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Graphics {

        class VKVertexBuffer : public RZVertexBuffer, public VKBuffer
        {
        public:
            VKVertexBuffer(const BufferUsage& usage);
            ~VKVertexBuffer();

            void Bind(const RZCommandBuffer* cmdBuffer) override;
            void Unbind() override;

            void SetData(uint32_t size, const void* data) override;
            void SetSubData(uint32_t size, const void* data, uint32_t offset) override;
        private:
            bool m_IsBufferMapped = false;
        };
    }
}
#endif