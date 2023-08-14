#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

    #include <d3d11.h>
    #include <wrl.h>

namespace Razix {
    namespace Graphics {

        class DX11Texture : public RZTexture
        {
        public:
            void SetData(const void* pixels) override;

            int32_t ReadPixels(u32 x, u32 y) override;

            void DestroyResource() override;

        protected:
            void  Bind(u32 slot) override;
            void  Unbind(u32 slot) override;
            void* GetAPIHandlePtr() const override;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif
