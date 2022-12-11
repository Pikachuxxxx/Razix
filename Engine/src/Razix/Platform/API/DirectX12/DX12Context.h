#pragma once

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Graphics {
        class DX12Context : public RZGraphicsContext
        {
        public:
            DX12Context(RZWindow* windowHandle);
            ~DX12Context() {}

            void Init() override;
            void Destroy() override;
            void ClearWithColor(float r, float g, float b) override;
        };
    }    // namespace Graphics
}    // namespace Razix

#endif