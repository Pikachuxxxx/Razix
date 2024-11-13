// clang-format off
#include "rzxpch.h"
// clang-format on

#include "DX12VertexBuffer.h"

#include "Razix/Graphics/RZVertexFormat.h"

#include "Razix/Graphics/Resources/RZResourceManager.h"

#include "Razix/Graphics/RHI/API/RZDrawCommandBuffer.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12Context.h"

    #include <vendor/d3dx12/d3dx12.h>

namespace Razix {
    namespace Graphics {

        DX12VertexBuffer::DX12VertexBuffer(const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Desc = desc;

            // Use a staging buffer to copy the data (if any)

            size_t bufferSize = desc.size;
            // Create a committed resource for the GPU resource in a default heap.
            CD3DX12_HEAP_PROPERTIES destResourceHeapProps(D3D12_HEAP_TYPE_DEFAULT);
            D3D12_RESOURCE_DESC     destResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(desc.size, D3D12_RESOURCE_FLAG_NONE);
            destResourceDesc.Width                   = desc.size;
            destResourceDesc.Flags                   = D3D12_RESOURCE_FLAG_NONE;
            CHECK_HRESULT(DX12Context::Get()->getDevice()->CreateCommittedResource(&destResourceHeapProps, D3D12_HEAP_FLAG_NONE, &destResourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_VertexBufferResource)));

            // Copy CPU data to this GPU only resource
            if (desc.data != nullptr)
                DX12VertexBuffer::SetData((u32) bufferSize, desc.data);

            // Create the vertex buffer view
            m_VertexBufferView.BufferLocation = m_VertexBufferResource->GetGPUVirtualAddress();
            m_VertexBufferView.SizeInBytes    = desc.size;
            m_VertexBufferView.StrideInBytes  = sizeof(RZVertex);    // TODO: use this in DX12 backend desc.stride;

            D3D12_TAG_OBJECT(m_VertexBufferResource, (LPCWSTR) bufferName.c_str());
        }

        RAZIX_CLEANUP_RESOURCE_IMPL(DX12VertexBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            D3D_SAFE_RELEASE(m_VertexBufferResource);
        }

        void DX12VertexBuffer::Bind(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto commandListD3D = (ID3D12GraphicsCommandList2*) Graphics::RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer)->getAPIBuffer();
            commandListD3D->IASetVertexBuffers(0, 1, &m_VertexBufferView);
        }

        void DX12VertexBuffer::Flush()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void* DX12VertexBuffer::GetMappedBuffer()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12VertexBuffer::Invalidate()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12VertexBuffer::Map(u32 size /*= 0*/, u32 offset /*= 0*/)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12VertexBuffer::Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12VertexBuffer::SetData(u32 size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_Desc.usage == BufferUsage::Static) {
                DX12Utilities::UpdateBufferResource(&m_VertexBufferResource, size, data);
            }
        }

        void DX12VertexBuffer::UnMap()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void DX12VertexBuffer::Unbind()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }    // namespace Graphics
}    // namespace Razix

#endif