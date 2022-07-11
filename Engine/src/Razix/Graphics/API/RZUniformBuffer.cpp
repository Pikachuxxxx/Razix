// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZUniformBuffer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#include "Razix/Platform/API/Vulkan/VKUniformBuffer.h"
#include "Razix/Platform/API/OpenGL/OpenGLUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        RZUniformBuffer* RZUniformBuffer::Create(uint32_t size, const void* data, const std::string& name)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLUniformBuffer(size, data, name); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKUniformBuffer(size, data, name); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix