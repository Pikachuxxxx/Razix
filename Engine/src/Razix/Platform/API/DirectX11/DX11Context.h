#pragma once

#include "Razix/Core/OS/Window.h"

#include "Razix/Graphics/API/GraphicsContext.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

#include <d3d11.h>
#include <wrl.h>

namespace Razix {
    namespace Graphics {

        class DX11Context : public GraphicsContext
        {
        public:
            DX11Context(RZWindow* windowHandle);

            void Init() override;
            void Destroy() override;
            void SwapBuffers() override;
            void ClearWithColor(float r, float g, float b) override;

        private:
            /* The window handle */
            RZWindow* m_Window;
            /* DirectX device Context */
            Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_Context;
            /* DirectX handle to the device */
            Microsoft::WRL::ComPtr<ID3D11Device>            m_Device;
            /* DirectX handle to swapchain */
            Microsoft::WRL::ComPtr<IDXGISwapChain>          m_Swapchain;  // Remove this from here
            /* The render target to which the swapchain presents/renders to */
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_Target;     // Remove this from here
        };
    }
}
#endif