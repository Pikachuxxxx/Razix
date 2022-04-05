#include "rzxpch.h"
#include "RZUniformBuffer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#include "Razix/Platform/API/Vulkan/VKUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        RZUniformBuffer* RZUniformBuffer::Create(uint32_t size, const void* data, const std::string& name)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:    break;
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKUniformBuffer(size, data, name); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }
    }
}