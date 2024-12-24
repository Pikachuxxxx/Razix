#pragma once

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Gfx/RHI/API/RZTexture.h"

    #include <d3d12.h>

namespace Razix {
    namespace Gfx {

        /* DirectX 12 implementation of the RZTexture class */
        class DX12Texture final : public RZTexture
        {
        public:
            DX12Texture(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~DX12Texture() {}

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            void    Resize(u32 width, u32 height) override;
            int32_t ReadPixels(u32 x, u32 y) override;
            void*   GetAPIHandlePtr() const override;

        private:
            friend class DX12Swapchain;
            DX12Texture(ID3D12Resource* backbuffer);    // only for swapchain

            ID3D12Resource* m_ResourceHandle;
        };
    }    // namespace Gfx
}    // namespace Razix

#endif