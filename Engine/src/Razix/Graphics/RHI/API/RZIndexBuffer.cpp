// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZIndexBuffer.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLIndexBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKIndexBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12IndexBuffer.h"
#endif

namespace Razix {
    namespace Graphics {

        GET_INSTANCE_SIZE_IMPL(IndexBuffer)

        void RZIndexBuffer::Create(void* where, const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL: new OpenGLIndexBuffer(data, count, bufferUsage); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKIndexBuffer(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Graphics::RenderAPI::D3D12: new (where) DX12IndexBuffer(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
                default: break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix