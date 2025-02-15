// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12DrawCommandBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Context.h"
    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"

namespace Razix {
    namespace Gfx {

        DX12DrawCommandBuffer::DX12DrawCommandBuffer(ID3D12CommandAllocator* commandAllocator)
            : m_CommandList(nullptr), m_CommandAllocator(commandAllocator)
        {
            m_State = CommandBufferState::Idle;
        }

        DX12DrawCommandBuffer::DX12DrawCommandBuffer(ID3D12GraphicsCommandList2* commandList)
            : m_CommandList(commandList)
        {
            UINT dataSize = sizeof(m_CommandAllocator);
            CHECK_HRESULT(m_CommandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &m_CommandAllocator));
        }

        RAZIX_CLEANUP_RESOURCE_IMPL(DX12DrawCommandBuffer)
        {
            D3D_SAFE_RELEASE(m_CommandList);
        }

        void DX12DrawCommandBuffer::Init(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto device = DX12Context::Get()->getDevice();

            // Get the command allocator from the ring buffer pool
            CHECK_HRESULT(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, IID_PPV_ARGS(&m_CommandList)));
            CHECK_HRESULT(m_CommandList->Close());

            D3D12_TAG_OBJECT(m_CommandList, L"Draw Command List");
        }

        void DX12DrawCommandBuffer::BeginRecording()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            m_State = CommandBufferState::Recording;

            // Reset the Command Allocator and Command List using the apt one for the current in-flight frame index
            CHECK_HRESULT(m_CommandAllocator->Reset());
            CHECK_HRESULT(m_CommandList->Reset(m_CommandAllocator, nullptr));
        }

        void DX12DrawCommandBuffer::EndRecording()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            CHECK_HRESULT(m_CommandList->Close());

            m_State = CommandBufferState::Ended;
        }

        void DX12DrawCommandBuffer::Execute()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            ID3D12CommandList* const ppCommandLists[] = {
                m_CommandList};

            DX12Context::Get()->getGraphicsQueue()->ExecuteCommandLists(1, ppCommandLists);
            m_State = CommandBufferState::Submitted;
        }

        void DX12DrawCommandBuffer::Reset()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            CHECK_HRESULT(m_CommandAllocator->Reset());
            CHECK_HRESULT(m_CommandList->Reset(m_CommandAllocator, nullptr));
        }

    }    // namespace Gfx
}    // namespace Razix

#endif