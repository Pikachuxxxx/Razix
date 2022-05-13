// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZIndexBuffer.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#include "Razix/Platform/API/OpenGL/OpenGLIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKIndexBuffer.h"

namespace Razix {
    namespace Graphics {

        RZIndexBuffer* RZIndexBuffer::Create(uint16_t* data, uint32_t count, const std::string& name, BufferUsage bufferUsage /*= BufferUsage::STATIC*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLIndexBuffer(data, count, bufferUsage); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKIndexBuffer(data, count, bufferUsage, name); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix