// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSemaphore.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKSemaphore.h"
#endif

namespace Razix {
    namespace Graphics {

        RZSemaphore* RZSemaphore::Create(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKSemaphore(RZ_DEBUG_S_ARG_NAME); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }
    }    // namespace Graphics
}    // namespace Razix
