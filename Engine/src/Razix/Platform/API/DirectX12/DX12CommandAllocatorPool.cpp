\    // clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12CommandAllocatorPool.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/D3D12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12Context.h"

    namespace Razix
{
    namespace Graphics {

        DX12CommandAllocatorPool::DX12CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type)
        {
            CHECK_HRESULT(DX12Context::Get()->getDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandListAllocator)));
        }

        //-------------------------------------------------------------------------------------------
        RAZIX_CLEANUP_RESOURCE_IMPL(DX12CommandAllocatorPool)
        {
            D3D_SAFE_RELEASE(m_CommandListAllocator);
        }
        //-------------------------------------------------------------------------------------------

        void DX12CommandAllocatorPool::Reset()
        {
            m_CommandListAllocator->Reset();
        }

    }    // namespace Graphics
}    // namespace Razix
#endif