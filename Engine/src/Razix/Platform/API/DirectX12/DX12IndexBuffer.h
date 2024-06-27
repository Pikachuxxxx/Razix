#pragma once

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Graphics {

        /**
         * DirectX 12 implementation of index buffer
         */
        class DX12IndexBuffer final : public RZIndexBuffer
        {
        public:
            DX12IndexBuffer(const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~DX12IndexBuffer() {}

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            void Bind(RZDrawCommandBufferHandle cmdBuffer = {}) override;
            void Unbind() override;

            void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;

            void Map(u32 size = 0, u32 offset = 0) override;
            void UnMap() override;

            void Flush() override;
            void Invalidate() override;

            void* GetMappedBuffer() override;

        private:
            ID3D12Resource*         m_IndexBufferResource = nullptr;
            D3D12_INDEX_BUFFER_VIEW m_IndexBufferView     = {};
        };
    }    // namespace Graphics
}    // namespace Razix

#endif