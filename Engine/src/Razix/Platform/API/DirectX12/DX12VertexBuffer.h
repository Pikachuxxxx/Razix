#pragma once

#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

namespace Razix {
    namespace Graphics {

        /**
         * DirectX 12 implementation of vertex buffer
         */
        class DX12VertexBuffer final : public RZVertexBuffer
        {
        public:
            DX12VertexBuffer(u32 size, const void* data, BufferUsage usage RZ_DEBUG_NAME_TAG_E_ARG);
            ~DX12VertexBuffer() {}

            void  AddBufferLayout(RZVertexBufferLayout& layout) override;
            void  Bind(RZDrawCommandBufferHandle cmdBuffer) override;
            void  Destroy() override;
            void  Flush() override;
            void* GetMappedBuffer() override;
            void  Invalidate() override;
            void  Map(u32 size = 0, u32 offset = 0) override;
            void  Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;
            void  SetData(u32 size, const void* data) override;
            void  UnMap() override;
            void  Unbind() override;
        };
    }    // namespace Graphics
}    // namespace Razix

#endif