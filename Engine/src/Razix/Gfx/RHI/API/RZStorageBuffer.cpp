// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZStorageBuffer.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKStorageBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLStorageBuffer.h"
#endif

namespace Razix {
    namespace Gfx {

        RZStorageBuffer* RZStorageBuffer::Create(u32 size, const std::string& name)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Gfx::RenderAPI::OPENGL: return new OpenGLStorageBuffer(size, name); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: return new VKStorageBuffer(size, name); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12:
#endif
                default: return nullptr; break;
            }
        }

    }    // namespace Graphics
}    // namespace Razix