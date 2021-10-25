#include "rzxpch.h"
#include "DX11Context.h"

#ifdef RAZIX_RENDER_API_DIRECTX11

#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

namespace Razix {
    namespace Graphics {

        DX11Context::DX11Context(RZWindow* windowHandle) : m_Window(windowHandle) {
            RAZIX_CORE_ASSERT(windowHandle, "Window Handle is NULL!");
        }

        void DX11Context::Init() {
            DXGI_SWAP_CHAIN_DESC sd = {};
            sd.BufferCount = 1;
            sd.BufferDesc.Width = 0;
            sd.BufferDesc.Height = 0;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 0;
            sd.BufferDesc.RefreshRate.Denominator = 0;
            sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
            sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;
            sd.OutputWindow = (HWND) glfwGetWin32Window((GLFWwindow*) m_Window->GetNativeWindow());
            sd.Windowed = TRUE;
            sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            sd.Flags = 0;

            // Create the device and swapchain
            D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                D3D11_CREATE_DEVICE_DEBUG,
                nullptr,
                0,
                D3D11_SDK_VERSION,
                &sd,
                &m_Swapchain,
                &m_Device,
                nullptr,
                &m_Context
            );

            // Get the back buffer and render target
            Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
            m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
            m_Device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_Target);

            // Configure the viewport
            D3D11_VIEWPORT viewport = {};
            viewport.Width = (FLOAT) m_Window->GetWidth();
            viewport.Height = (FLOAT) m_Window->GetHeight();
            viewport.MaxDepth = 1;
            viewport.MinDepth = 0;
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            m_Context->RSSetViewports(1, &viewport);

            // Bind the depth stencil view to OM
            m_Context->OMSetRenderTargets(1u, m_Target.GetAddressOf(), nullptr);
        }

        void DX11Context::Destroy() {
            m_Target.Reset();
            m_Swapchain.Reset();
            m_Device.Reset();
            m_Context.Reset();
        }

        void DX11Context::SwapBuffers() {
            m_Swapchain->Present(1u, 0u);
        }

        void DX11Context::ClearWithColor(float r, float g, float b) {
            const float  color[] = { r, g, b, 1.0f };
            m_Context->ClearRenderTargetView(m_Target.Get(), color);
        }

    }
}
#endif