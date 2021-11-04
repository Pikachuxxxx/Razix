#include "rzxpch.h"
#include "Swapchain.h"

#include "Razix/Graphics/API/GraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/API/OpenGL/OpenGLSwapchain.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKSwapchain.h"
#endif

namespace Razix {
    namespace Graphics {

        RZSwapchain* RZSwapchain::Create(uint32_t width, uint32_t height) {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:    return new OpenGLSwapchain(width, height); break;
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKSwapchain(width, height); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }

    }
}