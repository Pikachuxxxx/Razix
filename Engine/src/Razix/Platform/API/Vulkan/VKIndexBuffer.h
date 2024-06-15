#pragma once

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Graphics {
        class VKIndexBuffer : public RZIndexBuffer, public VKBuffer
        {
        public:
            VKIndexBuffer(u32* data, u32 count, BufferUsage bufferUsage RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKIndexBuffer() {}

            void Bind(RZDrawCommandBuffer* commandBuffer = nullptr) override;
            void Unbind() override {}

            void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;

            void Destroy() override;

            void  Map(u32 size = 0, u32 offset = 0) override;
            void  UnMap() override;
            void  Flush() override;
            void  Invalidate() override;
            void* GetMappedBuffer() override;

        private:
            bool m_IsBufferMapped = false;
        };
    }    // namespace Graphics
}    // namespace Razix
