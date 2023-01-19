#pragma once

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Graphics {
        class VKIndexBuffer : public RZIndexBuffer, public VKBuffer
        {
        public:
            VKIndexBuffer(uint16_t* data, uint32_t count, BufferUsage bufferUsage RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKIndexBuffer();

            void Bind(RZCommandBuffer* commandBuffer = nullptr) override;
            void Unbind() override;
            void Destroy() override;
            void Resize(uint32_t size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;

            void Map(uint32_t size = 0, uint32_t offset = 0) override;

            void UnMap() override;

            void* GetMappedBuffer() override;

            void Flush() override;

        private:
            bool m_IsBufferMapped = false;
        };
    }    // namespace Graphics
}    // namespace Razix
