// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCommandBuffer.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLCommandBuffer.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"
#endif

namespace Razix {
    namespace Graphics {

        RZCommandBuffer* RZCommandBuffer::BeginSingleTimeCommandBuffer()
        {
            auto             vkCmdBuffer = VKUtilities::BeginSingleTimeCommandBuffer();
            VKCommandBuffer* cmdBuffer   = new VKCommandBuffer(vkCmdBuffer);
            return (RZCommandBuffer*) cmdBuffer;
        }

        void RZCommandBuffer::EndSingleTimeCommandBuffer(RZCommandBuffer* cmdBuffer)
        {
            return VKUtilities::EndSingleTimeCommandBuffer(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer());
        }

        RZCommandBuffer* RZCommandBuffer::Create()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLCommandBuffer(); break;
                case Razix::Graphics::RenderAPI::VULKAN: return new VKCommandBuffer(); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix