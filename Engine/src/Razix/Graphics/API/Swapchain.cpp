#include "rzxpch.h"
#include "Swapchain.h"

#include "Razix/Graphics/API/GraphicsContext.h"

#include "Razix/Platform/API/OpenGL/OpenGLSwapchain.h"

namespace Razix {
    namespace Graphics {

        RZSwapchain* RZSwapchain::Create(uint32_t width, uint32_t height) {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:    return new OpenGLSwapchain(width, height); break;
                case Razix::Graphics::RenderAPI::VULKAN:
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }

    }
}