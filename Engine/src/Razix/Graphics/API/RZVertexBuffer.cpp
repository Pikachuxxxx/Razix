// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZVertexBuffer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLVertexBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKVertexBuffer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZVertexBuffer* RZVertexBuffer::Create(uint32_t size, const void* data, BufferUsage usage RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLVertexBuffer(size, data, usage); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKVertexBuffer(size, data, usage RZ_DEBUG_E_ARG_NAME); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix