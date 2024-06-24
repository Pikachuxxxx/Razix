#pragma once

#include "Razix/Graphics/RHI/API/RZDrawCommandBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Graphics {

        class DX12DrawCommandBuffer final : public RZDrawCommandBuffer
        {
        public:
            DX12DrawCommandBuffer(ID3D12CommandAllocator* commandAllocator);
            DX12DrawCommandBuffer(ID3D12GraphicsCommandList2* commandList);
            ~DX12DrawCommandBuffer() {}

            RAZIX_CLEANUP_RESOURCE

            void Init(RZ_DEBUG_NAME_TAG_S_ARG) override;
            void BeginRecording() override;
            void EndRecording() override;
            void Execute() override;
            void Reset() override;

            RAZIX_INLINE void*                       getAPIBuffer() override { return (void*) m_CommandList; }
            RAZIX_INLINE ID3D12GraphicsCommandList2* getD3DCommandList() const { return m_CommandList; }
            RAZIX_INLINE ID3D12CommandAllocator*     getD3DCommandAllocator() const { return m_CommandAllocator; }

        private:
            ID3D12GraphicsCommandList2* m_CommandList      = nullptr; /* Handle to the DX12 command buffer that will be executed by the GPU */
            ID3D12CommandAllocator*     m_CommandAllocator = nullptr; /* Handle to the command allocator from which this buffer was allocated from */
        };
    }    // namespace Graphics
}    // namespace Razix
#endif
