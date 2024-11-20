// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSemaphore.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKSemaphore.h"
#endif

namespace Razix {
    namespace Gfx {

        RZSemaphore* RZSemaphore::Create(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Gfx::RenderAPI::VULKAN: return new VKSemaphore(RZ_DEBUG_S_ARG_NAME); break;
                case Razix::Gfx::RenderAPI::D3D12:
                case Razix::Gfx::RenderAPI::GXM:
                case Razix::Gfx::RenderAPI::GCM:
                default: return nullptr; break;
            }
            return nullptr;
        }
    }    // namespace Graphics
}    // namespace Razix
