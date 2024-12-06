// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZVertexBuffer.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLVertexBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKVertexBuffer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZVertexBuffer* RZVertexBuffer::Create(u32 size, const void* data, BufferUsage usage RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLVertexBuffer(size, data, usage); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Graphics::RenderAPI::VULKAN: return new VKVertexBuffer(size, data, usage RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Graphics::RenderAPI::D3D12:
#endif
                default: return nullptr; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix