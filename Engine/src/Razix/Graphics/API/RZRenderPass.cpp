#include "rzxpch.h"
#include "RZRenderPass.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKRenderPass.h"
#endif

namespace Razix {
    namespace Graphics {

        RZRenderPass* RZRenderPass::Create(const RenderPassInfo& renderPassCI)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKRenderPass(renderPassCI); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }

    }
}