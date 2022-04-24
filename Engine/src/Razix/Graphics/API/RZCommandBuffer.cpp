#include "rzxpch.h"
#include "RZCommandBuffer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/API/OpenGL/OpenGLCommandBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZCommandBuffer* RZCommandBuffer::Create()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:    return new OpenGLCommandBuffer(); break;
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKCommandBuffer(); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }

        }

    }
}