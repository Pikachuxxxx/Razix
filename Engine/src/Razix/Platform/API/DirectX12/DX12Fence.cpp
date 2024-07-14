// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Fence.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12Context.h"

namespace Razix {
    namespace Graphics {

        DX12Fence::DX12Fence(bool isSignalled /*= false*/)
        {
            auto device = DX12Context::Get()->getDevice();
            CHECK_HRESULT(device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

            m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
            RAZIX_CORE_ASSERT(m_FenceEvent, "[DX12] cannot create fence event handle!.");

            D3D12_TAG_OBJECT(m_Fence, L"Sync Fence");
        }

        DX12Fence::~DX12Fence()
        {
            m_Fence->Release();
            m_Fence = nullptr;
        }

        void DX12Fence::wait(u64 fenceValue)
        {
            if (!isComplete(fenceValue)) {
                m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
                ::WaitForSingleObject(m_FenceEvent, 0xffffffffUL);
            }
        }

        u64 DX12Fence::signal(ID3D12CommandQueue* commandQueue)
        {
            uint64_t fenceValueForSignal = ++m_FenceValue;
            CHECK_HRESULT(commandQueue->Signal(m_Fence, fenceValueForSignal));
            return fenceValueForSignal;
        }

        bool DX12Fence::isComplete(u64 fenceValue)
        {
            return m_Fence->GetCompletedValue() >= fenceValue;
        }

        void DX12Fence::flush(ID3D12CommandQueue* commandQueue)
        {
            wait(signal(commandQueue));
        }
    }    // namespace Graphics
}    // namespace Razix
#endif
