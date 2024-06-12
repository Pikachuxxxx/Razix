// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12DrawCommandBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/D3D12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12Context.h"

namespace Razix {
    namespace Graphics {

        DX12DrawCommandBuffer::DX12DrawCommandBuffer()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_State = CommandBufferState::Idle;
        }

        DX12DrawCommandBuffer::DX12DrawCommandBuffer(ID3D12GraphicsCommandList2* commandList)
            : m_CommandList(commandList)
        {
        }

        DX12DrawCommandBuffer::~DX12DrawCommandBuffer()
        {
        }

        void DX12DrawCommandBuffer::Init(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto device = DX12Context::Get()->getDevice();

            auto commandAllocator = DX12Context::Get()->getCommandPool();
            CHECK_HRESULT(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&m_CommandList)));
            CHECK_HRESULT(m_CommandList->Close());
        }

        void DX12DrawCommandBuffer::BeginRecording()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Reset the Command Allocator and Command List using the apt one for the current in-flight frame index
            auto commandAllocator = DX12Context::Get()->getCommandPool();
            CHECK_HRESULT(commandAllocator->Reset());
            CHECK_HRESULT(m_CommandList->Reset(commandAllocator, nullptr));
            // Associate the command allocator with the command list so that it can be
            // retrieved when the command list is executed.
            CHECK_HRESULT(m_CommandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator));
        }

        void DX12DrawCommandBuffer::EndRecording()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            CHECK_HRESULT(m_CommandList->Close());
        }

        void DX12DrawCommandBuffer::Execute()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            ID3D12CommandAllocator* commandAllocator = nullptr;
            UINT                    dataSize         = sizeof(commandAllocator);
            CHECK_HRESULT(m_CommandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

            ID3D12CommandList* const ppCommandLists[] = {
                m_CommandList};

            DX12Context::Get()->getGraphicsQueue()->ExecuteCommandLists(1, ppCommandLists);
        }

        void DX12DrawCommandBuffer::Reset()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto commandAllocator = DX12Context::Get()->getCommandPool();
            CHECK_HRESULT(commandAllocator->Reset());
            CHECK_HRESULT(m_CommandList->Reset(commandAllocator, nullptr));
        }

    }    // namespace Graphics
}    // namespace Razix

#endif