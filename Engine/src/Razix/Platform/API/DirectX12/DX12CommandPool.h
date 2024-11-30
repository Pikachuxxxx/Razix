#pragma once

#include "Razix/Gfx/RHI/API/RZCommandPool.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Gfx {

        class DX12CommandPool final : public RZCommandPool
        {
        public:
            DX12CommandPool(D3D12_COMMAND_LIST_TYPE type);
            ~DX12CommandPool() {}

            RAZIX_CLEANUP_RESOURCE

            void Reset() override;

            void* getAPIHandle() override { return m_CommandListAllocator; }

        private:
            ID3D12CommandAllocator* m_CommandListAllocator = nullptr;
        };
    }    // namespace Gfx
}    // namespace Razix

#endif