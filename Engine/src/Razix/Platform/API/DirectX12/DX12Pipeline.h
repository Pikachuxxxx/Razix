#pragma once

#include "Razix/Graphics/RHI/API/RZPipeline.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Graphics {

        class DX12Pipeline : public RZPipeline
        {
        public:
            DX12Pipeline(const RZPipelineDesc& desc);
            ~DX12Pipeline() {}

            RAZIX_CLEANUP_RESOURCE

            void Bind(RZDrawCommandBufferHandle cmdBuffer) override;

        private:
            ID3D12PipelineState* m_PipelineState = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix

#endif