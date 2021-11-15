#include "rzxpch.h"
#include "RZVertexBuffer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/API/OpenGL/OpenGLVertexBuffer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZVertexBuffer* RZVertexBuffer::Create(BufferUsage& usage)
        {
            switch (RZGraphicsContext::GetRenderAPI()) { 
                case Razix::Graphics::RenderAPI::OPENGL:    return new OpenGLVertexBuffer(usage); break;
                case Razix::Graphics::RenderAPI::VULKAN:
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }

    }
}