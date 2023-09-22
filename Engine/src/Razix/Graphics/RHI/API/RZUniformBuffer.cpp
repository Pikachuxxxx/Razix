// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZUniformBuffer.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#include "Razix/Platform/API/OpenGL/OpenGLUniformBuffer.h"
#include "Razix/Platform/API/Vulkan/VKUniformBuffer.h"

namespace Razix {
    namespace Graphics {

        GET_INSTANCE_SIZE_IMPL(UniformBuffer)

        void RZUniformBuffer::Create(void* where, const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: new (where) OpenGLUniformBuffer(desc); break;
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKUniformBuffer(desc RZ_DEBUG_E_ARG_NAME); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix