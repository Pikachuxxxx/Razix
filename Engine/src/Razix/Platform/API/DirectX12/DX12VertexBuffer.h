#pragma once

#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Gfx {

        /**
         * DirectX 12 implementation of vertex buffer
         */
        class DX12VertexBuffer final : public RZVertexBuffer
        {
        public:
            DX12VertexBuffer(const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~DX12VertexBuffer() {}

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            void  Bind(RZDrawCommandBufferHandle cmdBuffer) override;
            void  Flush() override;
            void* GetMappedBuffer() override;
            void  Invalidate() override;
            void  Map(u32 size = 0, u32 offset = 0) override;
            void  Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;
            void  SetData(u32 size, const void* data) override;
            void  UnMap() override;
            void  Unbind() override;

        private:
            ID3D12Resource*          m_VertexBufferResource = nullptr;
            D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView     = {};
        };
    }    // namespace Gfx
}    // namespace Razix

#endif