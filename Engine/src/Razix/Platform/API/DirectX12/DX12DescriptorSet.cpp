// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12DescriptorSet.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Context.h"
    #include "Razix/Platform/API/DirectX12/DX12Texture.h"
    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"

namespace Razix {
    namespace Gfx {

        D3D12_CPU_DESCRIPTOR_HANDLE& OffsetCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, u32 offsetScaledByIncrementSize)
        {
            cpuHandle.ptr = size_t(i64(cpuHandle.ptr) + i64(offsetScaledByIncrementSize));
            return cpuHandle;
        }

        DX12DescriptorSet::DX12DescriptorSet(const RZDescriptorSetDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Desc = desc;
            // This also works for set index since all the descriptors will have the same set/space idx
            // m_SetIdx = descriptors[0].bindingInfo.location.set;

            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDesc.NumDescriptors             = m_Desc.descriptors.size();
            heapDesc.Type                       = DX12Utilities::DescriptorHeapTypeToDX12(m_Desc.heapType);

            CHECK_HRESULT(DX12Context::Get()->getDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));

            m_HeapElementSize = DX12Context::Get()->getDevice()->GetDescriptorHandleIncrementSize(heapDesc.Type);

            // query and cache the CPU and GPU heap start handles
            m_CPUHeapStart = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            m_GPUHeapStart = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        }

        void DX12DescriptorSet::UpdateSet(const std::vector<RZDescriptor>& descriptors)
        {
            // Warn for non-matching descriptor heap types with resource types
            for (auto& descriptor: descriptors) {
                switch (descriptor.bindingInfo.type) {
                    case DescriptorType::kTexture: {
                        const RZTexture*   texturePtr  = RZResourceManager::Get().getTextureResource(descriptor.texture);
                        const DX12Texture* d3dTex      = static_cast<const DX12Texture*>(texturePtr);
                        auto               d3dResource = (ID3D12Resource*) texturePtr->GetAPIHandlePtr();
                        DX12Context::Get()->getDevice()->CreateShaderResourceView(d3dResource, &d3dTex->getSrv(), m_CPUHeapStart);
                    } break;
                    case DescriptorType::kSampler: {
                        // ??????
                        // User internal sampler heap don't do anything here, we bind that heap when needed or idk let's see when we get there
                    } break;
                    case DescriptorType::kUniformBuffer: {
                        // TODO: DX12UniformBuffer + g_Device->/DX12Device::CreateConstantBufferView do something like this
                    } break;
                    default:
                        break;
                }
                // move to next resource
                m_CPUHeapStart = OffsetCPUHandle(m_CPUHeapStart, m_HeapElementSize);
            }
        }

        //-------------------------------------------------------------------------------------------
        RAZIX_CLEANUP_RESOURCE_IMPL(DX12DescriptorSet)
        {
            if (m_DescriptorHeap)
                m_DescriptorHeap->Release();
        }
        //-------------------------------------------------------------------------------------------
    }    // namespace Gfx
}    // namespace Razix

#endif