// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZStorageBuffer.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKStorageBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/API/OpenGL/OpenGLStorageBuffer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZStorageBuffer* RZStorageBuffer::Create(u32 size, const std::string& name)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLStorageBuffer(size, name); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKStorageBuffer(size, name); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
        }

    }    // namespace Graphics
}    // namespace Razix