#include "rzxpch.h"
#include "RZUniformBuffer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

namespace Razix {
    namespace Graphics {

        Razix::Graphics::RZUniformBuffer* RZUniformBuffer::Create(uint32_t size, const void* data)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:
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