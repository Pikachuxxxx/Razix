#pragma once

#include "Razix/Gfx/RHI/API/RZRootSignature.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

namespace Razix {
    namespace Gfx {

        class DX12RootSignature final : public RZRootSignature
        {
        public:
            DX12RootSignature() {}
            ~DX12RootSignature() {}

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

        private:
            ID3D12RootSignature* pRootSignature           = nullptr;
            ID3DBlob*            pSerializedRootSignature = nullptr;
            ID3DBlob*            pErrorBlob               = nullptr;
        };

    }    // namespace Graphics
}    // namespace Razix

#endif