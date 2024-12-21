#pragma once

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Gfx/RHI/API/RZSampler.h"

    #include <d3d12.h>

namespace Razix {
    namespace Gfx {

        /* DirectX 12 implementation of the RZSampler class */
        class DX12Sampler final : public RZSampler
        {
        public:
            DX12Sampler(const RZSamplerDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~DX12Sampler() {}

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

        private:
            D3D12_SAMPLER_DESC* m_SamplerHandle = nullptr;
        };
    }    // namespace Gfx
}    // namespace Razix

#endif