#pragma once

#include "Razix/Gfx/RHI/API/RZPipeline.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Gfx {

        class DX12Pipeline : public RZPipeline
        {
        public:
            DX12Pipeline(const RZPipelineDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~DX12Pipeline() {}

            RAZIX_CLEANUP_RESOURCE

            void Bind(RZDrawCommandBufferHandle cmdBuffer) override;

        private:
            ID3D12PipelineState* m_PipelineState  = nullptr;
            ID3D12RootSignature* m_pRootSignature = nullptr;
        };
    }    // namespace Gfx
}    // namespace Razix

#endif