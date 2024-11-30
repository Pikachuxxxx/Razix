// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFramebuffer.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKFramebuffer.h"
#endif

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLFrameBuffer.h"
#endif

namespace Razix {
    namespace Gfx {

        RZFramebuffer* RZFramebuffer::Create(const FramebufferInfo& frameBufInfo RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Gfx::RenderAPI::OPENGL: return new OpenGLFrameBuffer(frameBufInfo); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: return new VKFramebuffer(frameBufInfo RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12:
#endif
                default: return nullptr; break;
            }
        }

    }    // namespace Gfx
}    // namespace Razix