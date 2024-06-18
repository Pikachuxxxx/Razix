#pragma once

#include "Razix/Graphics/RHI/API/RZCommandAllocatorPool.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Graphics {

        class DX12CommandAllocatorPool final : public RZCommandAllocatorPool
        {
        public:
            DX12CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type);
            ~DX12CommandAllocatorPool() {}

            RAZIX_CLEANUP_RESOURCE

            void Reset() override;

            void* getAPIHandle() override { return m_CommandListAllocator; }

        private:
            ID3D12CommandAllocator* m_CommandListAllocator = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix

#endif