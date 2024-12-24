#pragma once

#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"

namespace Razix {
    namespace Gfx {

        /**
         * DX12 Implementation of Descriptor Set aka Heaps
         */
        class DX12DescriptorSet : public RZDescriptorSet
        {
        public:
            DX12DescriptorSet(const std::vector<RZDescriptor>& descriptors RZ_DEBUG_NAME_TAG_E_ARG);

            void UpdateSet(const std::vector<RZDescriptor>& descriptors) override;
            void Destroy() const override;

        private:
            ID3D12DescriptorHeap* m_CbvSrvUavHeap = nullptr;
        };

    }    // namespace Gfx
}    // namespace Razix