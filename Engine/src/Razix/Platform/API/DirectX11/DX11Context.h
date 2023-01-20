#pragma once

#include "Razix/Core/OS/RZWindow.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

    #include "Razix/Platform/API/DirectX11/DX11Swapchain.h"

    #include <d3d11.h>
    #include <wrl.h>

namespace Razix {
    namespace Graphics {

        class DX11Context : public RZGraphicsContext
        {
        public:
            DX11Context(RZWindow* windowHandle);

            void Init() override;
            void Destroy() override;
            void ClearWithColor(float r, float g, float b) override;

            static DX11Context* Get() { return static_cast<DX11Context*>(s_Context); }

            inline RZWindow*      getWindow() const { return m_Window; }
            inline DX11Swapchain* getSwapchain() { return &m_Swapchain; }

        private:
            /* The window handle */
            RZWindow* m_Window;
            /* DirectX device Context */
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
            /* DirectX handle to the device */
            Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
            /* DirectX11 implementation of the swapchain */
            DX11Swapchain m_Swapchain;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif