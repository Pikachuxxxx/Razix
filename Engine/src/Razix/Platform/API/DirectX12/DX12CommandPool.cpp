// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12CommandPool.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/D3D12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12Context.h"

namespace Razix {
    namespace Graphics {

        DX12CommandPool::DX12CommandPool(D3D12_COMMAND_LIST_TYPE type)
        {
            CHECK_HRESULT(DX12Context::Get()->getDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandListAllocator)));
            D3D12_TAG_OBJECT(m_CommandListAllocator, L"Command Allocator");
        }

        //-------------------------------------------------------------------------------------------
        RAZIX_CLEANUP_RESOURCE_IMPL(DX12CommandPool)
        {
            D3D_SAFE_RELEASE(m_CommandListAllocator);
        }
        //-------------------------------------------------------------------------------------------

        void DX12CommandPool::Reset()
        {
            m_CommandListAllocator->Reset();
        }

    }    // namespace Graphics
}    // namespace Razix
#endif