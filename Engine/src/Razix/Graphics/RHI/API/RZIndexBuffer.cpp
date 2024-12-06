// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZIndexBuffer.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#include "Razix/Platform/API/OpenGL/OpenGLIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKIndexBuffer.h"

namespace Razix {
    namespace Graphics {

        RZIndexBuffer* RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_F_ARG u32* data, u32 count, BufferUsage bufferUsage /*= BufferUsage::STATIC*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLIndexBuffer(data, count, bufferUsage); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Graphics::RenderAPI::VULKAN: return new VKIndexBuffer(data, count, bufferUsage RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Graphics::RenderAPI::D3D12:
#endif
                default: return nullptr; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix