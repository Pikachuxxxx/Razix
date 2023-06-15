// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSwapchain.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLSwapchain.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKContext.h"    
    #include "Razix/Platform/API/Vulkan/VKSwapchain.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX11
    #include "Razix/Platform/API/DirectX11/DX11Context.h"
    #include "Razix/Platform/API/DirectX11/DX11Swapchain.h"
#endif

namespace Razix {
    namespace Graphics {

        RZSwapchain* RZSwapchain::Create(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLSwapchain(width, height); break;
                case Razix::Graphics::RenderAPI::VULKAN: return static_cast<RZSwapchain*>(VKContext::Get()->getSwapchain().get());    //return new VKSwapchain(width, height); break;
                case Razix::Graphics::RenderAPI::D3D11: return DX11Context::Get()->getSwapchain(); break;
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
        }

    }    // namespace Graphics
}    // namespace Razix