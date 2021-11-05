#pragma once

#include "Razix/Graphics/API/Texture.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

#include <d3d11.h>
#include <wrl.h>

namespace Razix {
    namespace Graphics {

        class DX11Texture : public RZTexture2D
        {
        public:
            void SetData(const void* pixels) override;
            void Release(bool deleteImage = true) override;

        protected:
            void Bind(uint32_t slot) override;
            void Unbind(uint32_t slot) override;
            void* GetHandle() const override;

        };
    }
}
#endif

