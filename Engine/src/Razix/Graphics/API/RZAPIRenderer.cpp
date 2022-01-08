#include "rzxpch.h"
#include "RZAPIRenderer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKRenderer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZAPIRenderer* RZAPIRenderer::s_APIInstance = nullptr;

        void RZAPIRenderer::Create(uint32_t width, uint32_t height)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:
                case Razix::Graphics::RenderAPI::VULKAN:    s_APIInstance = new VKRenderer(width, height); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: s_APIInstance = nullptr;  break;
            }
        }
    }
}