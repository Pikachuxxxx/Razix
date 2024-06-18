// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12DrawCommandBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/D3D12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12Context.h"

namespace Razix {
    namespace Graphics {

        DX12DrawCommandBuffer::DX12DrawCommandBuffer(ID3D12CommandAllocator* commandAllocator)
            : m_CommandList(nullptr), m_CommandAllocator(commandAllocator)
        {
            m_State = CommandBufferState::Idle;
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
        }

        void DX12DrawCommandBuffer::BeginRecording()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Reset the Command Allocator and Command List using the apt one for the current in-flight frame index
            CHECK_HRESULT(m_CommandAllocator->Reset());
            CHECK_HRESULT(m_CommandList->Reset(m_CommandAllocator, nullptr));
        }

        void DX12DrawCommandBuffer::EndRecording()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            CHECK_HRESULT(m_CommandList->Close());
        }

        void DX12DrawCommandBuffer::Execute()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            ID3D12CommandList* const ppCommandLists[] = {
                m_CommandList};

            DX12Context::Get()->getGraphicsQueue()->ExecuteCommandLists(1, ppCommandLists);
        }

        void DX12DrawCommandBuffer::Reset()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            CHECK_HRESULT(m_CommandAllocator->Reset());
            CHECK_HRESULT(m_CommandList->Reset(m_CommandAllocator, nullptr));
        }

    }    // namespace Graphics
}    // namespace Razix

#endif