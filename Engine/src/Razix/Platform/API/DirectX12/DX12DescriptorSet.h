#pragma once

#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"

namespace Razix {
    namespace Gfx {

        /**
         * DX12 Implementation of Descriptor Set aka Heaps
         * 
         * This class has a single heap to abstract over and we places all descriptors of same type in this heap
         * 
         * For samplers we maintain a internal heap by context and re-use the combinations, RTV heap ??? FG has it? not sure yet
         */
        class DX12DescriptorSet : public RZDescriptorSet
        {
        public:
            DX12DescriptorSet(const RZDescriptorSetDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            void UpdateSet(const std::vector<RZDescriptor>& descriptors) override;

            /* This handle is used to bind the descriptor heap during rendering */
            RAZIX_INLINE D3D12_GPU_DESCRIPTOR_HANDLE getHeapGPUStartHandle() const { return m_GPUHeapStart; }

        private:
            ID3D12DescriptorHeap*       m_DescriptorHeap  = nullptr;
            u32                         m_HeapElementSize = 0;
            D3D12_CPU_DESCRIPTOR_HANDLE m_CPUHeapStart    = {};
            D3D12_GPU_DESCRIPTOR_HANDLE m_GPUHeapStart    = {};
        };
    }    // namespace Gfx
}    // namespace Razix