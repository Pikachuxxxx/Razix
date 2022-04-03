#include "rzxpch.h"
#include "RZImGuiRenderer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKImGuiRenderer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZImGuiRenderer* RZImGuiRenderer::Create(uint32_t width, uint32_t height)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKImGuiRenderer(width, height); break;
                default: return nullptr;  break;
            }
        }
    }
}